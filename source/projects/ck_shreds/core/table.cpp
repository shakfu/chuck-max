// SPDX-License-Identifier: MIT
/* table.cpp -- see table.h. */

#include "core/table.h"

#include <algorithm>
#include <cstdio>

namespace cks {

int Table::find(long id) const {
	for (size_t i = 0; i < mRows.size(); ++i)
		if (mRows[i].id == id)
			return (int)i;
	return -1;
}

void Table::added(long id, const std::string &name) {
	const int i = find(id);
	if (i >= 0) {
		mRows[(size_t)i].name = name;
		mRows[(size_t)i].elapsed = 0;
		return;
	}
	Row r;
	r.id = id;
	r.name = name;
	auto at = std::lower_bound(mRows.begin(), mRows.end(), id, [](const Row &a, long b) { return a.id < b; });
	mRows.insert(at, r);
}

void Table::removed(long id) {
	mRows.erase(std::remove_if(mRows.begin(), mRows.end(), [id](const Row &r) { return r.id == id; }), mRows.end());
}

void Table::dump(const std::vector<Row> &live) {
	mRows = live;
	std::sort(mRows.begin(), mRows.end(), [](const Row &a, const Row &b) { return a.id < b.id; });
}

void Table::advance(double seconds) {
	if (seconds <= 0)
		return;
	for (Row &r : mRows)
		r.elapsed += seconds;
}

std::string format_elapsed(double seconds) {
	const long s = seconds > 0 ? (long)seconds : 0;
	char buf[32];
	if (s >= 3600)
		snprintf(buf, sizeof buf, "%ld:%02ld:%02ld", s / 3600, (s / 60) % 60, s % 60);
	else
		snprintf(buf, sizeof buf, "%02ld:%02ld", s / 60, s % 60);
	return buf;
}

std::string display_name(const std::string &name) {
	const size_t k = name.rfind('/');
	return k == std::string::npos ? name : name.substr(k + 1);
}

} // namespace cks
