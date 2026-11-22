// SPDX-License-Identifier: MIT
/* shreds_test.cpp -- OwnedShreds: ownership by name from chuck~ replies. */

#include "test.h"

#include "core/shreds.h"

using namespace ckc;

TEST(shreds_owns_by_name) {
	OwnedShreds o;
	o.set_name("a.ck");
	CHECK_EQ(o.latest(), 0L);
	o.added(1, "a.ck");
	o.added(2, "b.ck");
	o.added(3, "a.ck");
	CHECK_EQ(o.count(), 2);
	CHECK_EQ(o.latest(), 3L);
	CHECK(o.owns(1));
	CHECK(!o.owns(2));
}

TEST(shreds_replace) {
	OwnedShreds o;
	o.set_name("a.ck");
	o.added(1, "a.ck");
	o.added(3, "a.ck");
	o.added(1, "a.ck"); /* our replace keeps the id and its place */
	CHECK_EQ(o.count(), 2);
	CHECK_EQ(o.latest(), 3L);
	o.added(3, "b.ck"); /* another box replaced it */
	CHECK_EQ(o.count(), 1);
	CHECK_EQ(o.latest(), 1L);
}

TEST(shreds_removed) {
	OwnedShreds o;
	o.set_name("a.ck");
	o.added(1, "a.ck");
	o.added(2, "a.ck");
	o.removed(2);
	CHECK_EQ(o.latest(), 1L);
	o.removed(9); /* not ours: no effect */
	CHECK_EQ(o.count(), 1);
	o.removed(1);
	CHECK_EQ(o.latest(), 0L);
}

TEST(shreds_dump) {
	OwnedShreds o;
	o.set_name("a.ck");
	o.added(7, "a.ck"); /* gone by the time of the dump */
	o.dump({{5, "a.ck"}, {2, "a.ck"}, {4, "c.ck"}});
	CHECK_EQ(o.count(), 2);
	CHECK(!o.owns(7));
	CHECK_EQ(o.latest(), 5L);
}

TEST(shreds_rename) {
	OwnedShreds o;
	o.set_name("a.ck");
	o.added(1, "a.ck");
	o.set_name("a.ck"); /* unchanged: kept */
	CHECK_EQ(o.count(), 1);
	o.set_name("b.ck");
	CHECK_EQ(o.count(), 0);
	CHECK_EQ(o.name(), std::string("b.ck"));
}
