// SPDX-License-Identifier: MIT
/* shreds.cpp -- see shreds.h. */

#include "core/shreds.h"

#include <algorithm>

namespace ckc {

void OwnedShreds::set_name(const std::string &name) {
	if (name != mName)
		mIds.clear();
	mName = name;
}

bool OwnedShreds::owns(long id) const { return std::find(mIds.begin(), mIds.end(), id) != mIds.end(); }

void OwnedShreds::added(long id, const std::string &name) {
	if (name != mName)
		removed(id);
	else if (!owns(id))
		mIds.push_back(id);
}

void OwnedShreds::removed(long id) { mIds.erase(std::remove(mIds.begin(), mIds.end(), id), mIds.end()); }

void OwnedShreds::dump(const std::vector<std::pair<long, std::string>> &live) {
	mIds.clear();
	for (const auto &s : live)
		if (s.second == mName)
			mIds.push_back(s.first);
	std::sort(mIds.begin(), mIds.end());
}

} // namespace ckc
