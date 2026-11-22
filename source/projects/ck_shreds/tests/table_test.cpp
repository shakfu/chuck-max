// SPDX-License-Identifier: MIT
/* table_test.cpp -- ck_shreds rows from chuck~ replies, time formatting. */

#include "test.h"

#include "core/table.h"

using namespace cks;
using std::string;

static string ids(const Table &t) {
	string o;
	for (const Row &r : t.rows())
		o += (o.empty() ? "" : ",") + std::to_string(r.id);
	return o;
}

TEST(table_add_remove) {
	Table t;
	t.added(3, "c.ck");
	t.added(1, "a.ck");
	t.added(2, "b.ck");
	CHECK_EQ(ids(t), string("1,2,3")); /* by id, whatever the arrival order */
	t.removed(2);
	CHECK_EQ(ids(t), string("1,3"));
	t.removed(9);
	CHECK_EQ(ids(t), string("1,3"));
	CHECK_EQ(t.find(3), 1);
	CHECK_EQ(t.find(2), -1);
}

TEST(table_replace_restarts) {
	Table t;
	t.added(1, "a.ck");
	t.advance(5);
	t.added(1, "b.ck"); /* replace keeps the id */
	CHECK_EQ(ids(t), string("1"));
	CHECK_EQ(t.rows()[0].name, string("b.ck"));
	CHECK_EQ(t.rows()[0].elapsed, 0.0);
}

TEST(table_advance) {
	Table t;
	t.added(1, "a.ck");
	t.advance(1.5);
	t.added(2, "b.ck");
	t.advance(0.5);
	t.advance(-3); /* ignored */
	CHECK_EQ(t.rows()[0].elapsed, 2.0);
	CHECK_EQ(t.rows()[1].elapsed, 0.5);
}

TEST(table_dump) {
	Table t;
	t.added(7, "gone.ck");
	Row a, b;
	a.id = 4;
	a.name = "x.ck";
	a.elapsed = 12;
	b.id = 2;
	b.name = "y.ck";
	b.elapsed = 30;
	t.dump({a, b});
	CHECK_EQ(ids(t), string("2,4"));
	CHECK_EQ(t.rows()[0].elapsed, 30.0);
	t.dump({});
	CHECK_EQ(ids(t), string(""));
}

TEST(table_format_elapsed) {
	CHECK_EQ(format_elapsed(0), string("00:00"));
	CHECK_EQ(format_elapsed(35.9), string("00:35"));
	CHECK_EQ(format_elapsed(61), string("01:01"));
	CHECK_EQ(format_elapsed(3599), string("59:59"));
	CHECK_EQ(format_elapsed(3723), string("1:02:03"));
	CHECK_EQ(format_elapsed(-4), string("00:00"));
}

TEST(table_display_name) {
	CHECK_EQ(display_name("/Users/a/otf_01.ck"), string("otf_01.ck"));
	CHECK_EQ(display_name("arp.ck"), string("arp.ck"));
	CHECK_EQ(display_name("<compiled.code>"), string("<compiled.code>"));
	CHECK_EQ(display_name("dir/"), string(""));
}
