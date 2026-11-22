// SPDX-License-Identifier: MIT
/* ck_code.cpp -- the ck_code Max class: attributes, lifecycle, files, shred
 * tracking and what it sends to chuck~. Painting and input live in
 * ck_code_paint.cpp and ck_code_input.cpp. */

#include "ck_code.h"

#include "core/paths.h"
#include "ext_systime.h"
#include "jpatcher_syms.h"
#include "platform/clipboard.h"
#include "platform/filedialog.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unordered_map>

using namespace ckc;

static t_class *s_class = nullptr;
static long s_next_id = 1;

static const char *const kColorAttr[RoleCount] = {
	"bgcolor",		"textcolor",	 "selcolor",	 "caretcolor",	  "curlinecolor",  "keywordcolor",
	"builtincolor", "functioncolor", "classcolor",	 "directivecolor", "stringcolor",  "escapecolor",
	"numbercolor",	"selfcolor",	 "commentcolor", "bracketcolor",  "operatorcolor", "matchcolor",
	"blinkcolor",	"barcolor",		 "barhighlight", "errorcolor",	  "warningcolor"};

/* Live boxes by id: the context menu looks its box up here after it returns. */
static std::unordered_map<long, t_ck_code *> &boxes() {
	static auto *m = new std::unordered_map<long, t_ck_code *>;
	return *m;
}

t_ck_code *ckc::find_box(long id) {
	auto it = boxes().find(id);
	return it == boxes().end() ? nullptr : it->second;
}

/* ---- small helpers ---- */

static std::string sym_str(t_symbol *s) { return s && s->s_name ? s->s_name : ""; }

/* HOME, else USERPROFILE (Windows sets only that), with '/' separators. */
std::string ckc::home_dir() {
	const char *h = getenv("HOME");
	if (!h || !*h)
		h = getenv("USERPROFILE");
	std::string s = h ? h : "";
	std::replace(s.begin(), s.end(), '\\', '/');
	return s;
}

/* Attribute path -> filesystem path. */
static std::string resolve(t_symbol *s) { return expand_home(home_dir(), posix_path(unquote(sym_str(s)))); }

static std::string join_atoms(long argc, t_atom *argv) {
	std::string out;
	for (long i = 0; i < argc; ++i) {
		if (i)
			out += ' ';
		char buf[64];
		switch (atom_gettype(argv + i)) {
		case A_SYM:
			out += sym_str(atom_getsym(argv + i));
			break;
		case A_LONG:
			snprintf(buf, sizeof buf, "%lld", (long long)atom_getlong(argv + i));
			out += buf;
			break;
		case A_FLOAT:
			snprintf(buf, sizeof buf, "%g", atom_getfloat(argv + i));
			out += buf;
			break;
		default:
			break;
		}
	}
	return out;
}

static std::string normalize_newlines(const std::string &s) {
	std::string o;
	o.reserve(s.size());
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == '\r') {
			o += '\n';
			if (i + 1 < s.size() && s[i + 1] == '\n')
				++i;
		} else
			o += s[i];
	}
	return o;
}

static bool read_file(const std::string &path, std::string *out) {
	std::ifstream f(fs_path(path), std::ios::binary);
	if (!f)
		return false;
	std::ostringstream ss;
	ss << f.rdbuf();
	if (f.bad())
		return false;
	*out = ss.str();
	return true;
}

static bool write_file(const std::string &path, const std::string &text) {
	std::ofstream f(fs_path(path), std::ios::binary | std::ios::trunc);
	if (!f)
		return false;
	f.write(text.data(), (std::streamsize)text.size());
	f.close();
	return !f.fail();
}

static std::string basename_of(const std::string &path) {
	const size_t k = path.rfind('/');
	return k == std::string::npos ? path : path.substr(k + 1);
}

int ckc::cells_between(const std::string &s, int a, int b) {
	int n = 0;
	for (int p = a; p < b && p < (int)s.size(); p = utf8_next(s, p))
		++n;
	return n;
}

void ckc::redraw(t_ck_code *x) { jbox_redraw((t_jbox *)x); }

t_jrgba ckc::color(t_ck_code *x, int role, double alpha_mul) {
	t_jrgba c = x->colors[role];
	c.alpha *= alpha_mul;
	return c;
}

/* ---- metrics and geometry ---- */

void ckc::ensure_metrics(t_ck_code *x) {
	Metrics &m = x->st->m;
	const double size = std::max(6.0, std::min(48.0, x->fontsize));
	if (m.font && m.size == size)
		return;
	if (m.font)
		jfont_destroy(m.font);
	m.font = jfont_create(kMonoFont, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_NORMAL, size);
	m.size = size;
	double ascent = size * 0.8, descent = size * 0.2;
	m.cw = size * 0.6;
	if (m.font) {
		t_jgraphics_font_extents e;
		jfont_extents(m.font, &e);
		double w = 0, h = 0;
		jfont_text_measure(m.font, "0000000000", &w, &h);
		if (w > 0)
			m.cw = w / 10;
		if (e.ascent > 0 && e.descent >= 0) {
			ascent = e.ascent;
			descent = e.descent;
		}
	}
	m.lh = std::ceil(ascent + descent) + 3;
	m.base = (m.lh - ascent - descent) / 2 + ascent;
}

Geom ckc::geometry(t_ck_code *x, double w, double h) {
	ensure_metrics(x);
	const Metrics &m = x->st->m;
	Geom g;
	g.w = w;
	g.h = h;
	g.title_h = x->titlebar ? m.lh : 0;
	if (x->linenumbers) {
		int n = x->st->ed.line_count(), d = 1;
		while (n >= 10) {
			n /= 10;
			++d;
		}
		g.gutter_w = std::max(d, 2) * m.cw + 2 * kPadX;
	}
	g.text_x = g.gutter_w + kPadX;
	g.sb_x = w - kScrollW;
	g.text_r = g.sb_x - 2; /* always reserves the scroll bar */
	g.view_y = g.title_h;
	g.view_h = std::max(0.0, h - g.title_h);
	g.cols = x->wrap ? std::max(1, (int)std::floor((g.text_r - g.text_x) / m.cw)) : 0;
	return g;
}

Geom ckc::sync_layout(t_ck_code *x) {
	State &st = *x->st;
	const Geom g = geometry(x, st.box_w, st.box_h);
	st.layout.update(st.ed, g.cols);
	st.layout.set_view(g.view_h, st.m.lh, kPadY);
	if (x->wrap)
		st.hscroll = 0;
	return g;
}

void ckc::view_size(t_ck_code *x, t_object *patcherview) {
	t_rect r;
	if (patcherview && jbox_get_rect_for_view((t_object *)x, patcherview, &r) == MAX_ERR_NONE) {
		x->st->box_w = r.width;
		x->st->box_h = r.height;
	}
}

void ckc::reveal_caret(t_ck_code *x) {
	State &st = *x->st;
	const Geom g = sync_layout(x);
	st.layout.ensure_row_visible(st.layout.row_of(st.ed.caret()));
	if (!x->wrap) {
		const double cx = st.ed.cells_before(st.ed.caret()) * st.m.cw;
		const double area = std::max(st.m.cw, g.text_r - g.text_x);
		if (cx - st.hscroll > area - st.m.cw)
			st.hscroll = cx - area + st.m.cw;
		if (cx < st.hscroll)
			st.hscroll = std::max(0.0, cx - area / 3);
	}
}

/* ---- change notifications ---- */

static void set_dirty(t_ck_code *x) {
	if (t_object *p = jbox_get_patcher((t_object *)x))
		jpatcher_set_dirty(p, 1);
}

void ckc::after_edit(t_ck_code *x) {
	State &st = *x->st;
	st.goal = -1;
	set_dirty(x);
	if (x->autowrite && !resolve(x->file).empty()) {
		st.write_pending = true;
		clock_fdelay(st.write_clock, 400);
	}
}

/* ---- clocks ---- */

static void write_now(t_ck_code *x) {
	State &st = *x->st;
	st.write_pending = false;
	const std::string path = resolve(x->file);
	if (path.empty())
		return;
	if (!write_file(path, st.ed.text()))
		object_error((t_object *)x, "cannot write %s", path.c_str());
}

static void write_tick(t_ck_code *x) {
	if (x->st->write_pending)
		write_now(x);
}

static void blink_tick(t_ck_code *x) {
	State &st = *x->st;
	if (!st.blinking)
		return;
	if ((double)(systime_ms() - st.blink_t0) < (double)x->blinkduration)
		clock_fdelay(st.blink_clock, 25);
	else
		st.blinking = false;
	redraw(x);
}

/* ---- completion ---- */

bool ckc::complete_open(t_ck_code *x) {
	State &st = *x->st;
	st.comp.close();
	const std::string &t = st.ed.text();
	const int caret = st.ed.caret();
	int ws = caret;
	auto word = [](char c) { return std::isalnum((unsigned char)c) || c == '_'; };
	while (ws > 0 && word(t[(size_t)ws - 1]))
		--ws;
	if (ws == caret)
		return false;
	st.words.sync(t, st.ed.revision());
	return st.comp.open(t, caret, st.words.query(t.substr((size_t)ws, (size_t)(caret - ws)), 200));
}

void ckc::complete_accept(t_ck_code *x) {
	State &st = *x->st;
	const CompletionEdit e = st.comp.accept();
	if (e.start < 0 || x->readonly)
		return;
	st.ed.select(e.start, e.end);
	st.ed.insert(e.text);
}

/* ---- clipboard ---- */

void ckc::paste(t_ck_code *x) {
	State &st = *x->st;
	const std::string s = normalize_newlines(clipboard_text());
	if (!s.empty() && !x->readonly)
		st.ed.insert(s);
}

void ckc::copy(t_ck_code *x, bool cut) {
	State &st = *x->st;
	if (!st.ed.has_selection())
		return;
	set_clipboard_text(st.ed.selected_text());
	if (cut && !x->readonly)
		st.ed.insert("");
}

/* ---- shreds ---- */

/* @shredname, else the file's name, else one unique to this box. */
static std::string derive_name(t_ck_code *x) {
	std::string n = unquote(sym_str(x->shredname));
	if (n.empty())
		n = basename_of(resolve(x->file));
	if (n.empty())
		n = "untitled-" + std::to_string(x->st->id) + ".ck";
	return n;
}

static void rename_shreds(t_ck_code *x) {
	x->st->shreds.set_name(derive_name(x));
	redraw(x);
}

static void flash(t_ck_code *x, const EvalSpan &sp) {
	State &st = *x->st;
	if (!x->blink)
		return;
	st.blinking = true;
	st.blink_start = sp.start;
	st.blink_end = sp.end;
	st.blink_t0 = systime_ms();
	clock_fdelay(st.blink_clock, 25);
}

void ckc::send(t_ck_code *x, Action a) {
	State &st = *x->st;
	st.comp.close();
	const std::string name = st.shreds.name();
	const long latest = st.shreds.latest();
	t_atom av[3];
	if (a == Action::Remove) {
		if (!latest) {
			object_warn((t_object *)x, "no shred named %s to remove", name.c_str());
			return;
		}
		atom_setlong(av, latest);
		outlet_anything((t_outlet *)x->outlet, gensym("remove"), 1, av);
		return;
	}
	st.syntax.update(st.ed.text(), st.ed.revision());
	const EvalSpan sp = st.syntax.eval_span(st.ed.caret(), st.ed.sel_start(), st.ed.sel_end(),
											a == Action::AddBlock ? EvalModeBlock : EvalModeDocument);
	std::string code;
	if (sp.has_value())
		code = st.ed.text().substr((size_t)sp.start, (size_t)(sp.end - sp.start));
	if (code.find_first_not_of(" \t\n") == std::string::npos) {
		object_post((t_object *)x, "nothing to send");
		return;
	}
	flash(x, sp);
	t_symbol *sname = gensym(name.c_str());
	t_symbol *scode = gensym(code.c_str());
	if (a == Action::Replace && latest) {
		atom_setlong(av, latest);
		atom_setsym(av + 1, sname);
		atom_setsym(av + 2, scode);
		outlet_anything((t_outlet *)x->outlet, gensym("replacecode"), 3, av);
	} else {
		atom_setsym(av, sname);
		atom_setsym(av + 1, scode);
		outlet_anything((t_outlet *)x->outlet, gensym("addcode"), 2, av);
	}
}

/* shred add <id> <name> | remove <id> | dump [<id> <name> <elapsed>]... */
static void ck_code_shred(t_ck_code *x, t_symbol *s, long argc, t_atom *argv) {
	State &st = *x->st;
	if (argc < 1 || atom_gettype(argv) != A_SYM)
		return;
	const std::string what = sym_str(atom_getsym(argv));
	const int before = st.shreds.count();
	const long latest = st.shreds.latest();
	if (what == "add" && argc >= 3)
		st.shreds.added((long)atom_getlong(argv + 1), sym_str(atom_getsym(argv + 2)));
	else if (what == "remove" && argc >= 2)
		st.shreds.removed((long)atom_getlong(argv + 1));
	else if (what == "dump") {
		std::vector<std::pair<long, std::string>> live;
		for (long i = 1; i + 1 < argc; i += 3)
			live.emplace_back((long)atom_getlong(argv + i), sym_str(atom_getsym(argv + i + 1)));
		st.shreds.dump(live);
	}
	if (st.shreds.count() != before || st.shreds.latest() != latest)
		redraw(x);
}

/* The rest of chuck~'s reply vocabulary arrives on the same cord. */
static void ck_code_ignore(t_ck_code *x, t_symbol *s, long argc, t_atom *argv) {}

static void ck_code_add(t_ck_code *x) { send(x, Action::Add); }
static void ck_code_replace(t_ck_code *x) { send(x, Action::Replace); }
static void ck_code_remove(t_ck_code *x) { send(x, Action::Remove); }

/* ---- files ---- */

static void reset_text(t_ck_code *x, const std::string &text) {
	State &st = *x->st;
	st.ed.reset(normalize_newlines(text));
	st.comp.close();
	st.layout.set_scroll(0);
	st.hscroll = 0;
	st.goal = -1;
	st.blinking = false;
}

/* Reads @file into the editor; on failure the document is unchanged. */
static bool load_file(t_ck_code *x) {
	const std::string path = resolve(x->file);
	std::string text;
	if (!read_file(path, &text)) {
		object_error((t_object *)x, "cannot read %s", path.c_str());
		return false;
	}
	reset_text(x, text);
	return true;
}

static void set_file_quiet(t_ck_code *x, const std::string &path) {
	x->st->no_reload = true;
	object_attr_setsym(x, gensym("file"), gensym(path.c_str()));
	x->st->no_reload = false;
}

void ckc::menu_save(t_ck_code *x) {
	State &st = *x->st;
	std::string path = resolve(x->file);
	const bool ask = path.empty();
	if (ask) {
		path = save_dialog("untitled.ck");
		if (path.empty())
			return;
	}
	if (!write_file(path, st.ed.text())) {
		object_error((t_object *)x, "cannot write %s", path.c_str());
		return;
	}
	if (!ask)
		st.write_pending = false;
	object_post((t_object *)x, "wrote %s", path.c_str());
	if (ask)
		set_file_quiet(x, path);
}

void ckc::menu_load(t_ck_code *x) {
	if (x->readonly)
		return;
	const std::string path = open_dialog();
	if (path.empty())
		return;
	std::string text;
	if (!read_file(path, &text)) {
		object_error((t_object *)x, "cannot read %s", path.c_str());
		return;
	}
	reset_text(x, text);
	x->st->write_pending = false;
	set_dirty(x);
	set_file_quiet(x, path);
	redraw(x);
}

/* ---- attribute setters ---- */

static void apply_theme(t_ck_code *x, const Theme &t, t_dictionary *keep) {
	for (int i = 0; i < RoleCount; ++i) {
		if (keep && dictionary_hasentry(keep, gensym(kColorAttr[i])))
			continue;
		x->colors[i].red = t[i].r;
		x->colors[i].green = t[i].g;
		x->colors[i].blue = t[i].b;
		x->colors[i].alpha = t[i].a;
	}
}

static std::string theme_names() {
	std::string s;
	for (int i = 0; i < theme_count(); ++i)
		s += (i ? " " : "") + theme_at(i).name;
	return s;
}

static t_max_err theme_set(t_ck_code *x, void *attr, long argc, t_atom *argv) {
	const std::string name = join_atoms(argc, argv);
	const Theme *t = theme_find(name);
	if (!t) {
		object_error((t_object *)x, "unknown theme '%s'; available: %s", name.c_str(), theme_names().c_str());
		return MAX_ERR_GENERIC;
	}
	x->theme = gensym(t->name.c_str());
	if (x->st && x->st->ready) {
		apply_theme(x, *t, nullptr);
		for (int i = 0; i < RoleCount; ++i)
			object_attr_touch((t_object *)x, gensym(kColorAttr[i]));
		redraw(x);
	}
	return MAX_ERR_NONE;
}

static t_max_err file_set(t_ck_code *x, void *attr, long argc, t_atom *argv) {
	x->file = gensym(join_atoms(argc, argv).c_str());
	if (!x->st || !x->st->ready)
		return MAX_ERR_NONE; /* new() loads after the saved text is known */
	if (!x->st->no_reload && x->autoload && !resolve(x->file).empty())
		load_file(x);
	rename_shreds(x);
	return MAX_ERR_NONE;
}

static t_max_err shredname_set(t_ck_code *x, void *attr, long argc, t_atom *argv) {
	x->shredname = gensym(join_atoms(argc, argv).c_str());
	if (x->st && x->st->ready)
		rename_shreds(x);
	return MAX_ERR_NONE;
}

/* ---- messages ---- */

static void ck_code_themes(t_ck_code *x) {
	object_post((t_object *)x, "themes: %s (in use: %s)", theme_names().c_str(), sym_str(x->theme).c_str());
}

static void ck_code_assist(t_ck_code *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		snprintf(s, 256, "chuck~ reply outlet (shred ownership), messages, attributes");
	else
		snprintf(s, 256, "addcode / replacecode / remove messages for chuck~");
}

static t_max_err ck_code_notify(t_ck_code *x, t_symbol *s, t_symbol *msg, void *sender, void *data) {
	return jbox_notify((t_jbox *)x, s, msg, sender, data);
}

static void ck_code_focusgained(t_ck_code *x, t_object *patcherview) {
	x->st->focused = true;
	redraw(x);
}

static void ck_code_focuslost(t_ck_code *x, t_object *patcherview) {
	x->st->focused = false;
	x->st->comp.close();
	redraw(x);
}

/* ---- persistence ---- */

static void ck_code_jsave(t_ck_code *x, t_dictionary *d) {
	State &st = *x->st;
	dictionary_appendstring(d, gensym("ck_code_text"), st.ed.text().c_str());
	dictionary_appendlong(d, gensym("ck_code_caret"), st.ed.caret());
	dictionary_appendfloat(d, gensym("ck_code_scroll"), st.layout.scroll_y());
}

/* Restores saved text, caret and scroll; true when there was saved text. */
static bool jload(t_ck_code *x, t_dictionary *d) {
	State &st = *x->st;
	t_symbol *k = gensym("ck_code_text");
	const char *s = nullptr;
	t_atom a;
	if (dictionary_getstring(d, k, &s) != MAX_ERR_NONE || !s) {
		s = nullptr;
		if (dictionary_getatom(d, k, &a) == MAX_ERR_NONE && atom_gettype(&a) == A_SYM)
			s = atom_getsym(&a)->s_name;
	}
	if (!s || !*s)
		return false;
	st.ed.reset(normalize_newlines(s));
	if (dictionary_getatom(d, gensym("ck_code_caret"), &a) == MAX_ERR_NONE)
		st.ed.set_caret((int)atom_getlong(&a));
	if (dictionary_getatom(d, gensym("ck_code_scroll"), &a) == MAX_ERR_NONE)
		st.layout.set_scroll(std::max(0.0, (double)atom_getfloat(&a)));
	return true;
}

/* ---- new / free ---- */

static void set_defaults(t_ck_code *x) {
	x->theme = gensym(theme_at(0).name.c_str());
	x->fontsize = 12;
	x->tabwidth = 4;
	x->linenumbers = x->titlebar = x->matchbrackets = x->wrap = x->blink = 1;
	x->completion = x->autoclose = x->autoload = 1;
	x->readonly = x->autowrite = 0;
	x->blinkduration = 500;
	x->scrollspeed = 3;
	x->file = gensym("");
	x->shredname = gensym("");
	apply_theme(x, theme_at(0), nullptr);
}

static void *ck_code_new(t_symbol *s, long argc, t_atom *argv) {
	t_dictionary *d = object_dictionaryarg(argc, argv);
	if (!d)
		return nullptr;
	t_ck_code *x = (t_ck_code *)object_alloc(s_class);
	if (!x)
		return nullptr;
	x->st = new State; /* before jbox_new: setters may run from there on */
	set_defaults(x);
	const long flags = JBOX_DRAWFIRSTIN | JBOX_TRANSPARENT | JBOX_DRAWINLAST | JBOX_HILITE | JBOX_GROWBOTH;
	jbox_new(&x->box, flags, argc, argv);
	x->box.b_firstin = (t_object *)x;
	x->outlet = outlet_new(x, nullptr);

	State &st = *x->st;
	st.id = s_next_id++;
	boxes()[st.id] = x;
	st.write_clock = (t_clock *)clock_new(x, (method)write_tick);
	st.blink_clock = (t_clock *)clock_new(x, (method)blink_tick);
	st.scroll_clock = (t_clock *)clock_new(x, (method)scroll_tick);

	attr_dictionary_process(x, d);
	if (const Theme *t = theme_find(sym_str(x->theme)))
		apply_theme(x, *t, d); /* colors in the dictionary keep their values */
	if (!jload(x, d) && x->autoload && !resolve(x->file).empty())
		load_file(x);
	st.shreds.set_name(derive_name(x));
	st.ready = true;
	jbox_ready(&x->box);
	return x;
}

static void ck_code_free(t_ck_code *x) {
	State *st = x->st;
	if (st) {
		boxes().erase(st->id);
		for (t_clock *c : {st->write_clock, st->blink_clock, st->scroll_clock})
			if (c) {
				clock_unset(c);
				object_free(c);
			}
		if (st->write_pending)
			write_now(x);
		if (st->m.font)
			jfont_destroy(st->m.font);
		st->m.font = nullptr;
	}
	jbox_free(&x->box);
	delete st;
	x->st = nullptr;
}

/* ---- class ---- */

#define CKC_FLAG(name, member, def)                                                                                    \
	CLASS_ATTR_LONG(c, name, 0, t_ck_code, member);                                                                    \
	CLASS_ATTR_STYLE(c, name, 0, "onoff");                                                                             \
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, name, 0, def)

void ext_main(void *r) {
	jpatcher_syms_init();

	t_class *c = class_new("ck_code", (method)ck_code_new, (method)ck_code_free, sizeof(t_ck_code), 0L, A_GIMME, 0);
	c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
	jbox_initclass(c, 0);

	class_addmethod(c, (method)ckc::paint, "paint", A_CANT, 0);
	class_addmethod(c, (method)ckc::getdrawparams, "getdrawparams", A_CANT, 0);
	class_addmethod(c, (method)ckc::key, "key", A_CANT, 0);
	class_addmethod(c, (method)ckc::mousedown, "mousedown", A_CANT, 0);
	class_addmethod(c, (method)ckc::mousedrag, "mousedrag", A_CANT, 0);
	class_addmethod(c, (method)ckc::mouseup, "mouseup", A_CANT, 0);
	class_addmethod(c, (method)ckc::mouseenter, "mouseenter", A_CANT, 0);
	class_addmethod(c, (method)ckc::mousemove, "mousemove", A_CANT, 0);
	class_addmethod(c, (method)ckc::mouseleave, "mouseleave", A_CANT, 0);
	class_addmethod(c, (method)ckc::mousewheel, "mousewheel", A_CANT, 0);
	class_addmethod(c, (method)ckc::acceptsdrag, "acceptsdrag_locked", A_CANT, 0);
	class_addmethod(c, (method)ckc::acceptsdrag, "acceptsdrag_unlocked", A_CANT, 0);
	class_addmethod(c, (method)ckc::dropfile, "dropfile", A_CANT, 0);
	class_addmethod(c, (method)ck_code_focusgained, "focusgained", A_CANT, 0);
	class_addmethod(c, (method)ck_code_focuslost, "focuslost", A_CANT, 0);
	class_addmethod(c, (method)ck_code_jsave, "jsave", A_CANT, 0);
	class_addmethod(c, (method)ck_code_notify, "notify", A_CANT, 0);
	class_addmethod(c, (method)ck_code_assist, "assist", A_CANT, 0);
	class_addmethod(c, (method)ck_code_themes, "themes", 0);
	class_addmethod(c, (method)ck_code_add, "add", 0);
	class_addmethod(c, (method)ck_code_replace, "replace", 0);
	class_addmethod(c, (method)ck_code_remove, "remove", 0);
	class_addmethod(c, (method)ck_code_shred, "shred", A_GIMME, 0);
	for (const char *sel : {"val", "event", "global", "msg", "clockprobe"})
		class_addmethod(c, (method)ck_code_ignore, sel, A_GIMME, 0);

	CLASS_ATTR_SYM(c, "theme", 0, t_ck_code, theme);
	CLASS_ATTR_ACCESSORS(c, "theme", NULL, theme_set);
	CLASS_ATTR_ENUM(c, "theme", 0, theme_names().c_str());
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "theme", 0, theme_at(0).name.c_str());

	CLASS_ATTR_DOUBLE(c, "fontsize", 0, t_ck_code, fontsize);
	CLASS_ATTR_FILTER_CLIP(c, "fontsize", 6, 48);
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "fontsize", 0, "12");

	CLASS_ATTR_LONG(c, "tabwidth", 0, t_ck_code, tabwidth);
	CLASS_ATTR_FILTER_CLIP(c, "tabwidth", 1, 16);
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "tabwidth", 0, "4");

	CKC_FLAG("linenumbers", linenumbers, "1");
	CKC_FLAG("titlebar", titlebar, "1");
	CKC_FLAG("matchbrackets", matchbrackets, "1");
	CKC_FLAG("wrap", wrap, "1");
	CKC_FLAG("blink", blink, "1");
	CKC_FLAG("completion", completion, "1");
	CKC_FLAG("autoclose", autoclose, "1");
	CKC_FLAG("autoload", autoload, "1");
	CKC_FLAG("readonly", readonly, "0");
	CKC_FLAG("autowrite", autowrite, "0");

	CLASS_ATTR_LONG(c, "blinkduration", 0, t_ck_code, blinkduration);
	CLASS_ATTR_FILTER_CLIP(c, "blinkduration", 50, 5000);
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "blinkduration", 0, "500");

	CLASS_ATTR_DOUBLE(c, "scrollspeed", 0, t_ck_code, scrollspeed);
	CLASS_ATTR_FILTER_CLIP(c, "scrollspeed", 0.05, 50);
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "scrollspeed", 0, "3");

	CLASS_ATTR_SYM(c, "file", 0, t_ck_code, file);
	CLASS_ATTR_ACCESSORS(c, "file", NULL, file_set);
	CLASS_ATTR_SAVE(c, "file", 0);

	CLASS_ATTR_SYM(c, "shredname", 0, t_ck_code, shredname);
	CLASS_ATTR_ACCESSORS(c, "shredname", NULL, shredname_set);
	CLASS_ATTR_SAVE(c, "shredname", 0);

	for (int i = 0; i < RoleCount; ++i) {
		const char *n = kColorAttr[i];
		class_addattr(c, attr_offset_array_new(n, gensym("float64"), 4, 0, (method)0L, (method)0L, 0,
											   (long)(calcoffset(t_ck_code, colors) + i * sizeof(t_jrgba))));
		CLASS_ATTR_ACCESSORS(c, n, NULL, jgraphics_attr_setrgba);
		CLASS_ATTR_STYLE(c, n, 0, "rgba");
		const Color &k = theme_at(0)[i];
		char def[128];
		snprintf(def, sizeof def, "%.4f %.4f %.4f %.4f", k.r, k.g, k.b, k.a);
		CLASS_ATTR_DEFAULT_SAVE_PAINT(c, n, 0, def);
	}

	CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 420. 200.");

	class_register(CLASS_BOX, c);
	s_class = c;
}
