// SPDX-License-Identifier: MIT
/* ck_syntax.h -- tree-sitter-chuck analysis for `ck_code`.
 *
 * One `Syntax` holds a copy of the document and its parse tree. `update()`
 * reparses incrementally: the edit is recovered as the changed span between
 * the old and new text (common prefix and suffix). Everything else answers
 * from that tree:
 *
 *   - per-line highlight tokens (cached per line per revision),
 *   - bracket pairs (only bracket leaves: none exist inside strings or
 *     comments, which are single tokens),
 *   - the indentation of a new line, from the enclosing brackets,
 *   - the span a key sends to chuck~ (selection, top-level item, document).
 *
 * Offsets are UTF-8 bytes, the same unit as `Editor`. No Max dependency.
 */

#pragma once

#include <string>
#include <vector>

struct TSParser;
struct TSTree;
struct TSQuery;
struct TSQueryCursor;

namespace ckc {

/* Highlight roles. Each maps to one color attribute of the object. */
enum TokenType {
	Default = 0,
	Keyword,
	Builtin,   /* now, dac, me, true, second, ... */
	Function,  /* fun name, call name */
	Class,     /* types: int, SinOsc, Object, ... */
	Directive, /* @import, @doc, @operator, ... */
	String,
	Escape,
	Number,
	Self,      /* this, me, super */
	Comment,
	Bracket,
	Operator,
	TokenTypeCount
};

const char *token_type_name(TokenType t);

/* Line-relative token. The tokens of a line cover it with no gaps. */
struct Token {
	int col;    /* byte offset inside the line */
	int length; /* bytes */
	TokenType type;
};

/* Byte offsets of the two bracket characters; `#(`, `%(` and `@(` count by their `(`. */
struct BracketMatch {
	int first = -1;
	int second = -1;
	bool valid() const { return first >= 0 && second >= 0; }
};

enum EvalMode {
	EvalModeBlock = 0,   /* top-level item under the caret, else the line */
	EvalModeDocument = 1 /* everything */
};

struct EvalSpan {
	int start = -1; /* byte, inclusive */
	int end = -1;   /* byte, exclusive */
	bool has_value() const { return start >= 0 && end > start; }
};

class Syntax {
  public:
	Syntax();
	~Syntax();
	Syntax(const Syntax &) = delete;
	Syntax &operator=(const Syntax &) = delete;

	/* Brings the tree up to date. A no-op when `revision` is unchanged. */
	void update(const std::string &text, int revision);

	const std::string &text() const { return mText; }
	int line_count() const { return (int)mLineStarts.size(); }

	/* Tokens of `line`, cached until the next revision. */
	const std::vector<Token> &tokens(int line);

	/* Partner of the bracket after the caret (an opener) or before it (a closer). */
	BracketMatch match_bracket(int caret) const;

	/* Spaces for a new line inserted at `caret`. */
	int indent_at(int caret, int tabwidth) const;

	/* New indentation for the line of `caret` after a closer was typed at
	 * `caret - 1` as the first character of its line; -1 = leave it. */
	int reindent_at(int caret, int tabwidth) const;

	/* What to send. A selection (`sel_start < sel_end`) always wins. */
	EvalSpan eval_span(int caret, int sel_start, int sel_end, EvalMode mode) const;

	/* Test instrumentation. */
	int parses() const { return mParses; }
	bool has_error() const;
	bool query_ok() const { return mQuery != nullptr; }

  private:
	void build_query();
	void line_index();
	int line_of(int offset) const;
	int line_indent(int line) const;

	TSParser *mParser = nullptr;
	TSTree *mTree = nullptr;
	TSQuery *mQuery = nullptr;
	TSQueryCursor *mCursor = nullptr;
	std::vector<TokenType> mCaptureType; /* query capture index -> role */

	std::string mText;
	int mRevision = -1;
	int mParses = 0;
	std::vector<int> mLineStarts;
	std::vector<std::vector<Token>> mLineTokens;
	std::vector<bool> mLineValid;
};

/* Spaces + tabs at the start of `line`. */
int leading_ws(const std::string &line);

} // namespace ckc
