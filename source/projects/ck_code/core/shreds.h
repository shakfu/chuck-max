// SPDX-License-Identifier: MIT
/* shreds.h -- the shreds a ck_code box owns, tracked from chuck~'s reply
 * outlet (`shred add <id> <name>`, `shred remove <id>`, `shred dump ...`).
 * A box owns the shreds named after it, so boxes editing the same file share
 * them. Max-independent. */

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace ckc {

class OwnedShreds {
  public:
	/* A new name owns none of the old name's shreds. */
	void set_name(const std::string &name);
	const std::string &name() const { return mName; }

	/* A shred took `id`: owned if `name` is ours. A replace reports an id it
	 * already holds; another box's replace takes the id away. */
	void added(long id, const std::string &name);
	void removed(long id);

	/* Every live shred, as (id, name); rebuilds the owned list in id order. */
	void dump(const std::vector<std::pair<long, std::string>> &live);

	long latest() const { return mIds.empty() ? 0 : mIds.back(); } /* 0 = none */
	int count() const { return (int)mIds.size(); }
	bool owns(long id) const;

  private:
	std::string mName;
	std::vector<long> mIds; /* in spork order */
};

} // namespace ckc
