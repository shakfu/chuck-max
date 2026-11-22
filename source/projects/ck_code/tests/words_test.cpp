// SPDX-License-Identifier: MIT
/* words_test.cpp -- local completion source: scan, ranking, kinds. */

#include "test.h"

#include "core/words.h"

using namespace ckc;
using std::string;

static string labels(const std::vector<CompletionItem> &v) {
	string o;
	for (const CompletionItem &it : v)
		o += (o.empty() ? "" : ",") + it.label;
	return o;
}

TEST(words_scan) {
	Words w;
	w.sync("foo = foo + bar_1\n_x = 1abc + 0x1f + a.b  # foo\n", 1);
	CHECK_EQ(w.count("foo"), 3); /* comments count: plain text scan */
	CHECK_EQ(w.count("bar_1"), 1);
	CHECK_EQ(w.count("_x"), 1);
	CHECK_EQ(w.count("abc"), 0); /* part of a number */
	CHECK_EQ(w.count("x1f"), 0);
	CHECK_EQ(w.count("a"), 1);
	CHECK_EQ(w.count("b"), 1);
	CHECK_EQ(w.count("1abc"), 0);
	Words u;
	u.sync("caf\xC3\xA9 x\xC3\xA9y", 1);
	CHECK_EQ(u.count("caf"), 1); /* identifiers are ASCII */
	CHECK_EQ(u.count("y"), 1);
}

TEST(words_sync_by_revision) {
	Words w;
	w.sync("alpha", 1);
	w.sync("alpha beta", 1); /* same revision: ignored */
	CHECK_EQ(w.scans(), 1);
	CHECK_EQ(w.count("beta"), 0);
	w.sync("beta", 2);
	CHECK_EQ(w.scans(), 2);
	CHECK_EQ(w.count("beta"), 1);
	CHECK_EQ(w.count("alpha"), 0); /* vanished words go */
	Words z;
	z.sync("q", 0); /* first sync always scans, even at revision 0 */
	CHECK_EQ(z.count("q"), 1);
}

TEST(words_ranking) {
	Words w;
	w.sync("alpha alpha alphabet al alp alq", 1);
	/* freq desc, length asc, name; `al` itself excluded */
	CHECK_EQ(labels(w.query("al", 0)), string("alpha,alp,alq,alphabet"));
	CHECK_EQ(labels(w.query("al", 2)), string("alpha,alp"));
	CHECK_EQ(labels(w.query("Al", 0)), string("")); /* case-sensitive */
	CHECK_EQ(labels(w.query("alphabet", 0)), string(""));
	CHECK_EQ(labels(w.query("zz", 0)), string(""));
	/* a builtin with no occurrence ranks after document words */
	w.sync("seconds seconds", 2);
	CHECK_EQ(labels(w.query("sec", 0)), string("seconds,second"));
}

TEST(words_builtins_keywords) {
	Words w;
	w.sync("", 1);
	CHECK_EQ(labels(w.query("whi", 0)), string("while"));
	CHECK_EQ(labels(w.query("spo", 0)), string("spork"));
	CHECK_EQ(labels(w.query("SinO", 0)), string("SinOsc"));
	CHECK_EQ(labels(w.query("blackh", 0)), string("blackhole"));
	const auto ret = w.query("retu", 0);
	CHECK_EQ(ret.size(), (size_t)1);
	CHECK_EQ(ret[0].kind, 14);
	CHECK_EQ(w.query("SinO", 0)[0].kind, 3);
	CHECK_EQ(w.count("SinOsc"), 0);
	CHECK((int)w.query("", 0).size() >= (int)(chuck_keywords().size() + chuck_builtins().size()));
}

TEST(words_items) {
	Words w;
	w.sync("SinOscBank b;\nSinOscBank c;\nSinOsc s => dac;\nwhile_ => now;\nwhile (true) 1::second => now;\n", 1);
	const auto items = w.query("SinO", 0);
	CHECK_EQ(labels(items), string("SinOscBank,SinOsc"));
	CHECK_EQ(items[0].kind, 1); /* document word */
	CHECK_EQ(items[1].kind, 3); /* builtin, counted in the document */
	for (const CompletionItem &it : items) {
		CHECK_EQ(it.insert, it.label);
		CHECK_EQ(it.replace_start, -1);
		CHECK_EQ(it.replace_end, -1);
	}
	const auto wh = w.query("whi", 0);
	CHECK_EQ(labels(wh), string("while,while_"));
	CHECK_EQ(wh[0].kind, 14);
	CHECK_EQ(wh[1].kind, 1);
}
