// SPDX-License-Identifier: MIT
/* table.h -- the rows of a ck_shreds box, built from chuck~'s reply outlet
 * (`shred add <id> <name>`, `shred remove <id>`, `shred dump ...`).
 * Max-independent. */

#pragma once

#include <string>
#include <vector>

namespace cks {

struct Row {
	long id = 0;
	std::string name;
	double elapsed = 0; /* seconds of VM time */
};

class Table {
  public:
	/* A new shred starts at 0 s. A known id is a replace: new name, restarted. */
	void added(long id, const std::string &name);
	void removed(long id);

	/* Every live shred with its exact elapsed time; replaces all rows. */
	void dump(const std::vector<Row> &live);

	/* VM time moved on by `seconds` since the last call. */
	void advance(double seconds);

	void clear() { mRows.clear(); }
	const std::vector<Row> &rows() const { return mRows; } /* by id */
	int find(long id) const;                               /* index, -1 = absent */

  private:
	std::vector<Row> mRows;
};

/* `mm:ss`, or `h:mm:ss` from one hour; negative counts as 0. */
std::string format_elapsed(double seconds);

/* A file shred's name is its path: the part after the last '/'. */
std::string display_name(const std::string &name);

} // namespace cks
