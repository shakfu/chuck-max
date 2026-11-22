// SPDX-License-Identifier: MIT
/* ck_code.h -- shared state of the ck_code Max object. */

#pragma once

#include "ext.h"
#include "ext_obex.h"
#include "jgraphics.h"
#include "jpatcher_api.h"

#include "ck_syntax.h"
#include "core/completion.h"
#include "core/editor.h"
#include "core/layout.h"
#include "core/palette.h"
#include "core/shreds.h"
#include "core/words.h"

#include <string>

namespace ckc {

/* Font metrics, measured once per font size. */
struct Metrics {
	double size = -1;
	double cw = 7, lh = 15, base = 12; /* cell width, line height, baseline from row top */
	t_jfont *font = nullptr;
};

/* Box-local geometry derived from the box size, metrics and attributes. */
struct Geom {
	double w = 0, h = 0;
	double title_h = 0;
	double gutter_w = 0;
	double text_x = 0, text_r = 0; /* text area left edge (row cell 0) and right edge */
	double view_y = 0, view_h = 0; /* rows area */
	double sb_x = 0;               /* scroll bar left edge */
	int cols = 0;                  /* wrap width in cells, 0 = no wrap */
};

/* What a key or message sends to chuck~. */
enum class Action { Add, AddBlock, Replace, Remove };

/* C++ state of one box; heap allocated, owned by the Max struct. */
struct State {
	long id = 0; /* key in the box registry */

	Editor ed; /* declared before `layout`: Layout points to it */
	Layout layout;
	Syntax syntax;
	Words words;
	CompletionSession comp;
	OwnedShreds shreds;
	Metrics m;
	double box_w = 420, box_h = 200; /* last known size (clocks have no view) */
	double hscroll = 0;              /* pixels, only when wrap is off */
	int goal = -1;                   /* goal cell for visual Up/Down */

	bool ready = false;     /* new() finished */
	bool focused = false;
	bool no_reload = false; /* set @file without reading it */

	/* Clocks (owned, freed in free). */
	t_clock *write_clock = nullptr;  /* autowrite 400 ms after the last edit */
	t_clock *blink_clock = nullptr;  /* send flash, every 25 ms */
	t_clock *scroll_clock = nullptr; /* autoscroll while drag-selecting */
	bool write_pending = false;

	/* Send flash. */
	bool blinking = false;
	int blink_start = 0, blink_end = 0;
	unsigned blink_t0 = 0;

	/* Mouse. */
	bool selecting = false, thumb_drag = false;
	double thumb_grab = 0;
	double mouse_x = 0, mouse_y = 0;
	int clicks = 0;
	unsigned click_t = 0;
	double click_x = 0, click_y = 0;
	bool autoscroll = false; /* scroll_clock armed */
};

} // namespace ckc

struct t_ck_code {
	t_jbox box;
	void *outlet;
	ckc::State *st;

	/* Attributes. */
	t_symbol *theme;
	double fontsize;
	long tabwidth;
	long linenumbers, titlebar, matchbrackets, wrap, blink, completion, autoclose, autoload;
	long readonly, autowrite;
	long blinkduration;
	double scrollspeed;
	t_symbol *file;
	t_symbol *shredname;
	t_jrgba colors[ckc::RoleCount];
};

namespace ckc {

constexpr double kPadY = 4;    /* Layout pad above the first and below the last row */
constexpr double kPadX = 6;    /* text inset from the gutter */
constexpr double kScrollW = 7; /* scroll bar width */
constexpr int kPopupRows = 8;

/* ck_code.cpp */
t_ck_code *find_box(long id); /* null once freed */
void ensure_metrics(t_ck_code *x);
Geom geometry(t_ck_code *x, double w, double h);
Geom sync_layout(t_ck_code *x); /* layout updated to the last known size */
void view_size(t_ck_code *x, t_object *patcherview);
t_jrgba color(t_ck_code *x, int role, double alpha_mul = 1);
void redraw(t_ck_code *x);
void after_edit(t_ck_code *x); /* text changed: clocks, dirty flag */
void reveal_caret(t_ck_code *x);
void send(t_ck_code *x, Action a);
bool complete_open(t_ck_code *x); /* true: popup open */
void complete_accept(t_ck_code *x);
void paste(t_ck_code *x);
void copy(t_ck_code *x, bool cut);
void menu_save(t_ck_code *x);
void menu_load(t_ck_code *x);
std::string home_dir();
int cells_between(const std::string &s, int a, int b);

/* ck_code_paint.cpp */
void paint(t_ck_code *x, t_object *patcherview);
void getdrawparams(t_ck_code *x, t_object *patcherview, t_jboxdrawparams *params);

/* ck_code_input.cpp */
long key(t_ck_code *x, t_object *patcherview, long keycode, long modifiers, long textcharacter);
void mousedown(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mousedrag(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mouseup(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mouseenter(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mousemove(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mouseleave(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers);
void mousewheel(t_ck_code *x, t_object *patcherview, t_pt pt, long modifiers, double dx, double dy);
void scroll_tick(t_ck_code *x);
long acceptsdrag(t_ck_code *x, t_object *drag, t_object *view);
void dropfile(t_ck_code *x, t_object *drag, void *arg);

} // namespace ckc
