// SPDX-License-Identifier: MIT
/* ck_shreds.cpp -- the ck_shreds Max class: a table of chuck~'s shreds (id,
 * name, elapsed VM time, remove button), fed by chuck~'s reply outlet. It
 * polls with `shreds dump` and removes with `remove <id>`. */

#include "ext.h"
#include "ext_obex.h"
#include "ext_systime.h"
#include "jgraphics.h"
#include "jpatcher_api.h"
#include "z_dsp.h"

#include "core/table.h"

#include <algorithm>
#include <cmath>
#include <string>

using namespace cks;

namespace {

constexpr double kPadX = 8;
constexpr double kTickMs = 250;

struct State {
	Table table;
	double scroll = 0;      /* pixels */
	int hover = -1;         /* row whose button is under the mouse */
	double since_poll = 0;  /* ms */
	unsigned last_tick = 0; /* systime_ms */
	t_clock *clock = nullptr;
	double box_w = 300, box_h = 160;
};

/* Column and row geometry for the current font. */
struct Geom {
	double header_h, row_h;
	double id_x, name_x, time_x, button_x, button_w;
	double rows_y, rows_h;
};

} // namespace

struct t_ck_shreds {
	t_jbox box;
	void *outlet;
	State *st;
	long interval; /* ms between `shreds dump`; 0 = no polling */
	t_jrgba bgcolor, textcolor, headercolor, gridcolor, buttoncolor;
};

static t_class *s_class = nullptr;

static void redraw(t_ck_shreds *x) { jbox_redraw((t_jbox *)x); }

static double font_size(t_ck_shreds *x) { return std::max(6.0, (double)jbox_get_fontsize((t_object *)x)); }

static Geom geometry(t_ck_shreds *x, double w, double h) {
	const double fs = font_size(x);
	Geom g;
	g.header_h = std::round(fs * 2.0);
	g.row_h = std::round(fs * 2.2);
	g.button_w = g.row_h;
	g.id_x = kPadX;
	g.name_x = kPadX + fs * 4.5;
	g.button_x = w - g.button_w;
	g.time_x = g.button_x - fs * 5.0;
	g.rows_y = g.header_h;
	g.rows_h = std::max(0.0, h - g.header_h);
	return g;
}

static double max_scroll(t_ck_shreds *x, const Geom &g) {
	return std::max(0.0, x->st->table.rows().size() * g.row_h - g.rows_h);
}

/* Index of the row under `y`, -1 = none. */
static int row_at(t_ck_shreds *x, const Geom &g, double y) {
	if (y < g.rows_y)
		return -1;
	const int i = (int)std::floor((y - g.rows_y + x->st->scroll) / g.row_h);
	return i >= 0 && i < (int)x->st->table.rows().size() ? i : -1;
}

/* ---- painting ---- */

static void show(t_jgraphics *g, const t_jrgba &c, double x0, double y_mid, const std::string &s) {
	double w = 0, h = 0;
	jgraphics_text_measure(g, s.c_str(), &w, &h);
	t_jrgba k = c;
	jgraphics_set_source_jrgba(g, &k);
	jgraphics_move_to(g, x0, y_mid + h * 0.35);
	jgraphics_show_text(g, s.c_str());
}

/* `s` cut with "..." to fit `width`. */
static std::string fit(t_jgraphics *g, std::string s, double width) {
	double w = 0, h = 0;
	jgraphics_text_measure(g, s.c_str(), &w, &h);
	if (w <= width)
		return s;
	while (!s.empty()) {
		s.pop_back();
		while (!s.empty() && ((unsigned char)s.back() & 0xC0) == 0x80)
			s.pop_back(); /* whole UTF-8 code points */
		const std::string t = s + "...";
		jgraphics_text_measure(g, t.c_str(), &w, &h);
		if (w <= width)
			return t;
	}
	return "";
}

static void fill(t_jgraphics *g, const t_jrgba &c, double x0, double y0, double w, double h) {
	t_jrgba k = c;
	jgraphics_set_source_jrgba(g, &k);
	jgraphics_rectangle(g, x0, y0, w, h);
	jgraphics_fill(g);
}

static void hline(t_jgraphics *g, const t_jrgba &c, double x0, double x1, double y) {
	t_jrgba k = c;
	jgraphics_set_source_jrgba(g, &k);
	jgraphics_set_line_width(g, 1);
	jgraphics_move_to(g, x0, y);
	jgraphics_line_to(g, x1, y);
	jgraphics_stroke(g);
}

static void ck_shreds_paint(t_ck_shreds *x, t_object *pv) {
	t_jgraphics *g = (t_jgraphics *)patcherview_get_jgraphics(pv);
	if (!g || !x->st)
		return;
	State &st = *x->st;
	t_rect r;
	jbox_get_rect_for_view((t_object *)x, pv, &r);
	st.box_w = r.width;
	st.box_h = r.height;
	const Geom gm = geometry(x, r.width, r.height);
	st.scroll = std::max(0.0, std::min(st.scroll, max_scroll(x, gm)));

	jgraphics_select_font_face(g, jbox_get_fontname((t_object *)x)->s_name, JGRAPHICS_FONT_SLANT_NORMAL,
							   JGRAPHICS_FONT_WEIGHT_NORMAL);
	jgraphics_set_font_size(g, font_size(x));

	fill(g, x->bgcolor, 0, 0, r.width, r.height);
	fill(g, x->headercolor, 0, 0, r.width, gm.header_h);
	const double hm = gm.header_h / 2;
	show(g, x->textcolor, gm.id_x, hm, "Shred");
	show(g, x->textcolor, gm.name_x, hm, "Code");
	show(g, x->textcolor, gm.time_x, hm, "Time");
	hline(g, x->gridcolor, 0, r.width, gm.header_h - 0.5);

	const std::vector<Row> &rows = st.table.rows();
	if (rows.empty()) {
		t_jrgba dim = x->textcolor;
		dim.alpha *= 0.45;
		show(g, dim, gm.id_x, gm.rows_y + gm.row_h / 2, "no shreds");
		return;
	}
	jgraphics_save(g);
	jgraphics_clip(g, 0, gm.rows_y, r.width, gm.rows_h);
	for (size_t i = 0; i < rows.size(); ++i) {
		const double y0 = gm.rows_y + i * gm.row_h - st.scroll;
		if (y0 + gm.row_h < gm.rows_y || y0 > r.height)
			continue;
		const Row &row = rows[i];
		const double mid = y0 + gm.row_h / 2;
		show(g, x->textcolor, gm.id_x, mid, std::to_string(row.id));
		show(g, x->textcolor, gm.name_x, mid, fit(g, display_name(row.name), gm.time_x - gm.name_x - kPadX));
		show(g, x->textcolor, gm.time_x, mid, format_elapsed(row.elapsed));

		/* remove button: a rounded square with a bar */
		const double inset = gm.row_h * 0.2, bs = gm.row_h - 2 * inset;
		t_jrgba b = x->buttoncolor;
		if ((int)i != st.hover)
			b.alpha *= 0.75;
		jgraphics_set_source_jrgba(g, &b);
		jgraphics_rectangle_rounded(g, gm.button_x + inset, y0 + inset, bs, bs, 4, 4);
		jgraphics_fill(g);
		t_jrgba bar = x->bgcolor;
		jgraphics_set_source_jrgba(g, &bar);
		jgraphics_set_line_width(g, 2);
		jgraphics_move_to(g, gm.button_x + inset + bs * 0.28, mid);
		jgraphics_line_to(g, gm.button_x + inset + bs * 0.72, mid);
		jgraphics_stroke(g);

		hline(g, x->gridcolor, 0, r.width, y0 + gm.row_h - 0.5);
	}
	jgraphics_restore(g);
}

/* ---- clock: elapsed time and polling ---- */

static void ck_shreds_tick(t_ck_shreds *x) {
	State &st = *x->st;
	const unsigned now = systime_ms();
	const double dt = (double)(now - st.last_tick);
	st.last_tick = now;
	/* VM time only moves while DSP runs; the next dump corrects any drift */
	if (sys_getdspstate())
		st.table.advance(dt / 1000.0);
	if (x->interval > 0) {
		st.since_poll += dt;
		if (st.since_poll >= x->interval) {
			st.since_poll = 0;
			t_atom a;
			atom_setsym(&a, gensym("dump"));
			outlet_anything(x->outlet, gensym("shreds"), 1, &a);
		}
	}
	if (!st.table.rows().empty())
		redraw(x);
	clock_fdelay(st.clock, kTickMs);
}

/* ---- messages ---- */

/* shred add <id> <name> | remove <id> | dump [<id> <name> <elapsed>]... */
static void ck_shreds_shred(t_ck_shreds *x, t_symbol *s, long argc, t_atom *argv) {
	Table &t = x->st->table;
	if (argc < 1 || atom_gettype(argv) != A_SYM)
		return;
	const std::string what = atom_getsym(argv)->s_name;
	if (what == "add" && argc >= 3)
		t.added((long)atom_getlong(argv + 1), atom_getsym(argv + 2)->s_name);
	else if (what == "remove" && argc >= 2)
		t.removed((long)atom_getlong(argv + 1));
	else if (what == "dump") {
		std::vector<Row> live;
		for (long i = 1; i + 2 < argc; i += 3) {
			Row r;
			r.id = (long)atom_getlong(argv + i);
			r.name = atom_getsym(argv + i + 1)->s_name;
			r.elapsed = atom_getfloat(argv + i + 2);
			live.push_back(r);
		}
		t.dump(live);
	} else
		return;
	redraw(x);
}

/* The rest of chuck~'s reply vocabulary arrives on the same cord. */
static void ck_shreds_ignore(t_ck_shreds *x, t_symbol *s, long argc, t_atom *argv) {}

static void ck_shreds_bang(t_ck_shreds *x) {
	t_atom a;
	atom_setsym(&a, gensym("dump"));
	outlet_anything(x->outlet, gensym("shreds"), 1, &a);
}

static void ck_shreds_clear(t_ck_shreds *x) {
	x->st->table.clear();
	redraw(x);
}

/* ---- mouse ---- */

static void ck_shreds_mousedown(t_ck_shreds *x, t_object *pv, t_pt pt, long modifiers) {
	State &st = *x->st;
	const Geom gm = geometry(x, st.box_w, st.box_h);
	const int i = row_at(x, gm, pt.y);
	if (i < 0 || pt.x < gm.button_x)
		return;
	t_atom a;
	atom_setlong(&a, st.table.rows()[(size_t)i].id);
	outlet_anything(x->outlet, gensym("remove"), 1, &a);
}

static void ck_shreds_mousemove(t_ck_shreds *x, t_object *pv, t_pt pt, long modifiers) {
	State &st = *x->st;
	const Geom gm = geometry(x, st.box_w, st.box_h);
	const int i = pt.x >= gm.button_x ? row_at(x, gm, pt.y) : -1;
	if (i != st.hover) {
		st.hover = i;
		redraw(x);
	}
}

static void ck_shreds_mouseleave(t_ck_shreds *x, t_object *pv, t_pt pt, long modifiers) {
	if (x->st->hover >= 0) {
		x->st->hover = -1;
		redraw(x);
	}
}

static void ck_shreds_mousewheel(t_ck_shreds *x, t_object *pv, t_pt pt, long modifiers, double dx, double dy) {
	State &st = *x->st;
	const Geom gm = geometry(x, st.box_w, st.box_h);
	st.scroll = std::max(0.0, std::min(st.scroll - dy * gm.row_h, max_scroll(x, gm)));
	redraw(x);
}

static void ck_shreds_assist(t_ck_shreds *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		snprintf(s, 256, "chuck~ reply outlet; bang: dump now; clear");
	else
		snprintf(s, 256, "shreds dump / remove <id> for chuck~");
}

static t_max_err ck_shreds_interval_set(t_ck_shreds *x, void *attr, long argc, t_atom *argv) {
	x->interval = argc ? std::max(0L, (long)atom_getlong(argv)) : 1000;
	if (x->st)
		x->st->since_poll = 0;
	return MAX_ERR_NONE;
}

/* ---- new / free ---- */

static void *ck_shreds_new(t_symbol *s, long argc, t_atom *argv) {
	t_dictionary *d = object_dictionaryarg(argc, argv);
	if (!d)
		return nullptr;
	t_ck_shreds *x = (t_ck_shreds *)object_alloc(s_class);
	if (!x)
		return nullptr;
	x->st = new State;
	x->interval = 1000;
	const long flags = JBOX_DRAWFIRSTIN | JBOX_DRAWINLAST | JBOX_FONTATTR | JBOX_GROWBOTH;
	jbox_new(&x->box, flags, argc, argv);
	x->box.b_firstin = (t_object *)x;
	x->outlet = outlet_new(x, nullptr);
	attr_dictionary_process(x, d);
	x->st->last_tick = systime_ms();
	x->st->clock = (t_clock *)clock_new(x, (method)ck_shreds_tick);
	clock_fdelay(x->st->clock, kTickMs);
	jbox_ready(&x->box);
	return x;
}

static void ck_shreds_free(t_ck_shreds *x) {
	if (x->st && x->st->clock) {
		clock_unset(x->st->clock);
		object_free(x->st->clock);
	}
	jbox_free(&x->box);
	delete x->st;
	x->st = nullptr;
}

#define CKS_COLOR(name, member, def)                                                                                   \
	CLASS_ATTR_RGBA(c, name, 0, t_ck_shreds, member);                                                                  \
	CLASS_ATTR_STYLE_LABEL(c, name, 0, "rgba", name);                                                                  \
	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, name, 0, def)

void ext_main(void *r) {
	t_class *c = class_new("ck_shreds", (method)ck_shreds_new, (method)ck_shreds_free, sizeof(t_ck_shreds), 0L,
						   A_GIMME, 0);
	c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
	jbox_initclass(c, JBOX_FONTATTR);

	class_addmethod(c, (method)ck_shreds_paint, "paint", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_mousedown, "mousedown", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_mousemove, "mousemove", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_mouseleave, "mouseleave", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_mousewheel, "mousewheel", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_assist, "assist", A_CANT, 0);
	class_addmethod(c, (method)ck_shreds_shred, "shred", A_GIMME, 0);
	class_addmethod(c, (method)ck_shreds_bang, "bang", 0);
	class_addmethod(c, (method)ck_shreds_clear, "clear", 0);
	for (const char *sel : {"val", "event", "global", "msg", "clockprobe"})
		class_addmethod(c, (method)ck_shreds_ignore, sel, A_GIMME, 0);

	CLASS_ATTR_LONG(c, "interval", 0, t_ck_shreds, interval);
	CLASS_ATTR_ACCESSORS(c, "interval", NULL, ck_shreds_interval_set);
	CLASS_ATTR_DEFAULT_SAVE(c, "interval", 0, "1000");

	CKS_COLOR("bgcolor", bgcolor, "0.157 0.173 0.204 1.");
	CKS_COLOR("textcolor", textcolor, "0.671 0.698 0.749 1.");
	CKS_COLOR("headercolor", headercolor, "0.129 0.145 0.169 1.");
	CKS_COLOR("gridcolor", gridcolor, "1. 1. 1. 0.08");
	CKS_COLOR("buttoncolor", buttoncolor, "0.878 0.424 0.459 1.");

	CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 320. 160.");

	class_register(CLASS_BOX, c);
	s_class = c;
}
