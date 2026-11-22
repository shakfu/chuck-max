// SPDX-License-Identifier: MIT
/* ck_syntax.cpp -- see ck_syntax.h. */

#include "ck_syntax.h"

#include <tree_sitter/api.h>

#include <algorithm>
#include <cstring>

extern "C" const TSLanguage *tree_sitter_chuck(void);

namespace ckc {

/* Overlap rule: the smaller node wins; on equal size the later pattern wins.
 * So `@self` follows `@builtin`: `me` is also a special_literal_value. A call's
 * callee is its first named child; later identifiers are arguments. */
static const char kHighlightQuery[] = R"scm(
(class_identifier) @class
(primitive_type) @class
(reference_type) @class

(function_definition name: (variable_identifier) @function)
(method_definition name: (variable_identifier) @function)
(function_call . (variable_identifier) @function)
(function_call . (member_identifier (variable_identifier) @function .))

[(global_unit_generator) (special_literal_value) (boolean_literal_value) (duration_identifier)] @builtin
["me" "this" "super"] @self

[(int) (float) (hexidecimal)] @number
[(string) (character)] @string
(escape_character) @escape
[(line_comment) (block_comment)] @comment

["@import" "@doc" "@operator" "@construct" "@destruct"] @directive

[(chuck_operator) (operator)] @operator
["::" "!" "~" "++" "--" "?" ":"] @operator
["(" ")" "[" "]" "{" "}" "#(" "%(" "@(" "<<<" ">>>"] @bracket

["break" "class" "const" "continue" "do" "else" "extends" "for" "fun" "function"
 "global" "if" "implements" "interface" "new" "private" "protected" "public" "pure"
 "repeat" "return" "spork" "static" "switch" "until" "while"] @keyword
)scm";

const char *token_type_name(TokenType t) {
	static const char *const names[TokenTypeCount] = {
		"default", "keyword", "builtin", "function", "class",   "directive", "string",
		"escape",  "number",  "self",    "comment",  "bracket", "operator"};
	return (t >= 0 && t < TokenTypeCount) ? names[t] : "?";
}

/* TokenTypeCount = an unknown capture name, ignored. */
static TokenType type_for_capture(const std::string &name) {
	for (int t = 0; t < TokenTypeCount; ++t)
		if (name == token_type_name((TokenType)t))
			return (TokenType)t;
	return TokenTypeCount;
}

void Syntax::build_query() {
	uint32_t err_offset = 0;
	TSQueryError err = TSQueryErrorNone;
	mQuery = ts_query_new(tree_sitter_chuck(), kHighlightQuery, (uint32_t)std::strlen(kHighlightQuery), &err_offset,
						  &err);
	if (!mQuery)
		return; /* a bug in kHighlightQuery: tests assert query_ok() */
	const uint32_t ncap = ts_query_capture_count(mQuery);
	mCaptureType.resize(ncap);
	for (uint32_t i = 0; i < ncap; ++i) {
		uint32_t len = 0;
		const char *name = ts_query_capture_name_for_id(mQuery, i, &len);
		mCaptureType[i] = type_for_capture(std::string(name, len));
	}
}

/* --- lifecycle and parsing ----------------------------------------------- */

Syntax::Syntax() {
	mParser = ts_parser_new();
	ts_parser_set_language(mParser, tree_sitter_chuck());
	mCursor = ts_query_cursor_new();
	build_query();
	line_index();
}

Syntax::~Syntax() {
	if (mTree)
		ts_tree_delete(mTree);
	if (mQuery)
		ts_query_delete(mQuery);
	ts_query_cursor_delete(mCursor);
	ts_parser_delete(mParser);
}

void Syntax::line_index() {
	mLineStarts.assign(1, 0);
	for (size_t i = 0; i < mText.size(); ++i)
		if (mText[i] == '\n')
			mLineStarts.push_back((int)i + 1);
	mLineTokens.assign(mLineStarts.size(), std::vector<Token>());
	mLineValid.assign(mLineStarts.size(), false);
}

int Syntax::line_of(int offset) const {
	auto it = std::upper_bound(mLineStarts.begin(), mLineStarts.end(), offset);
	return (int)(it - mLineStarts.begin()) - 1;
}

static TSPoint point_of(const std::vector<int> &starts, int offset) {
	auto it = std::upper_bound(starts.begin(), starts.end(), offset);
	const int row = (int)(it - starts.begin()) - 1;
	return TSPoint{(uint32_t)row, (uint32_t)(offset - starts[(size_t)row])};
}

void Syntax::update(const std::string &text, int revision) {
	if (mTree && revision == mRevision)
		return;
	mRevision = revision;
	if (mTree && text == mText)
		return;

	if (mTree) {
		/* The edit is the span where old and new differ. */
		const size_t n0 = mText.size(), n1 = text.size(), lim = std::min(n0, n1);
		size_t p = 0;
		while (p < lim && mText[p] == text[p])
			++p;
		size_t s = 0;
		while (s < lim - p && mText[n0 - 1 - s] == text[n1 - 1 - s])
			++s;
		TSInputEdit e;
		e.start_byte = (uint32_t)p;
		e.old_end_byte = (uint32_t)(n0 - s);
		e.new_end_byte = (uint32_t)(n1 - s);
		e.start_point = point_of(mLineStarts, (int)p);
		e.old_end_point = point_of(mLineStarts, (int)(n0 - s));
		mText = text;
		line_index();
		e.new_end_point = point_of(mLineStarts, (int)(n1 - s));
		ts_tree_edit(mTree, &e);
	} else {
		mText = text;
		line_index();
	}
	TSTree *t = ts_parser_parse_string(mParser, mTree, mText.data(), (uint32_t)mText.size());
	if (mTree)
		ts_tree_delete(mTree);
	mTree = t;
	++mParses;
	std::fill(mLineValid.begin(), mLineValid.end(), false);
}

bool Syntax::has_error() const { return mTree && ts_node_has_error(ts_tree_root_node(mTree)); }

/* --- highlight ------------------------------------------------------------ */

const std::vector<Token> &Syntax::tokens(int line) {
	static const std::vector<Token> kEmpty;
	if (line < 0 || line >= line_count() || !mTree)
		return kEmpty;
	if (mLineValid[(size_t)line])
		return mLineTokens[(size_t)line];

	const int ls = mLineStarts[(size_t)line];
	int le = line + 1 < line_count() ? mLineStarts[(size_t)line + 1] - 1 : (int)mText.size();
	std::vector<Token> &out = mLineTokens[(size_t)line];
	out.clear();
	mLineValid[(size_t)line] = true;
	if (le <= ls)
		return out;

	struct Span {
		int a, b;
		uint32_t size, pattern;
		TokenType type;
	};
	std::vector<Span> spans;
	if (mQuery) {
		ts_query_cursor_set_byte_range(mCursor, (uint32_t)ls, (uint32_t)le);
		ts_query_cursor_exec(mCursor, mQuery, ts_tree_root_node(mTree));
		TSQueryMatch m;
		while (ts_query_cursor_next_match(mCursor, &m)) {
			for (uint16_t k = 0; k < m.capture_count; ++k) {
				const TokenType t = mCaptureType[m.captures[k].index];
				if (t == TokenTypeCount)
					continue;
				const int a = (int)ts_node_start_byte(m.captures[k].node);
				const int b = (int)ts_node_end_byte(m.captures[k].node);
				if (b <= ls || a >= le)
					continue;
				spans.push_back(Span{std::max(a, ls), std::min(b, le), (uint32_t)(b - a), m.pattern_index, t});
			}
		}
	}
	std::sort(spans.begin(), spans.end(), [](const Span &x, const Span &y) {
		return x.size != y.size ? x.size > y.size : x.pattern < y.pattern;
	});
	std::vector<unsigned char> role((size_t)(le - ls), (unsigned char)Default);
	for (const Span &s : spans)
		std::fill(role.begin() + (s.a - ls), role.begin() + (s.b - ls), (unsigned char)s.type);

	int i = 0;
	const int n = le - ls;
	while (i < n) {
		int j = i + 1;
		while (j < n && role[(size_t)j] == role[(size_t)i])
			++j;
		out.push_back(Token{i, j - i, (TokenType)role[(size_t)i]});
		i = j;
	}
	return out;
}

/* --- brackets ------------------------------------------------------------- */

static char partner_of(char c) {
	switch (c) {
	case '(': return ')';
	case '[': return ']';
	case '{': return '}';
	case ')': return '(';
	case ']': return '[';
	case '}': return '{';
	}
	return 0;
}

/* The bracket a leaf type stands for; `#(` `%(` `@(` open a `(`. 0 = none. */
static char bracket_char(const char *t) {
	const size_t n = t ? std::strlen(t) : 0;
	if (n == 1 && partner_of(t[0]))
		return t[0];
	if (n == 2 && (t[0] == '#' || t[0] == '%' || t[0] == '@') && t[1] == '(')
		return '(';
	return 0;
}

/* The bracket leaf whose bracket character is at `off`, or a null node. */
static TSNode bracket_leaf(TSNode root, int off, char *c) {
	TSNode n = ts_node_descendant_for_byte_range(root, (uint32_t)off, (uint32_t)off + 1);
	if (ts_node_is_null(n) || ts_node_child_count(n) != 0 || ts_node_is_named(n) || ts_node_is_missing(n) ||
		ts_node_end_byte(n) != (uint32_t)off + 1)
		return TSNode{};
	const char b = bracket_char(ts_node_type(n));
	if (!b)
		return TSNode{};
	*c = b;
	return n;
}

BracketMatch Syntax::match_bracket(int caret) const {
	BracketMatch r;
	if (!mTree)
		return r;
	const TSNode root = ts_tree_root_node(mTree);
	char c = 0;
	TSNode n{};
	const int len = (int)mText.size();
	if (caret < len && (mText[(size_t)caret] == '(' || mText[(size_t)caret] == '[' || mText[(size_t)caret] == '{'))
		n = bracket_leaf(root, caret, &c);
	if (ts_node_is_null(n) && caret > 0 &&
		(mText[(size_t)caret - 1] == ')' || mText[(size_t)caret - 1] == ']' || mText[(size_t)caret - 1] == '}'))
		n = bracket_leaf(root, caret - 1, &c);
	if (ts_node_is_null(n))
		return r;

	/* Brackets pair up among siblings: nested pairs live inside child nodes, and
	 * an ERROR parent keeps flat tokens, so count depth over the siblings. */
	const TSNode parent = ts_node_parent(n);
	if (ts_node_is_null(parent))
		return r;
	const bool open = c == '(' || c == '[' || c == '{';
	const char want = partner_of(c);
	const uint32_t count = ts_node_child_count(parent);
	int self = -1;
	for (uint32_t i = 0; i < count; ++i)
		if (ts_node_eq(ts_node_child(parent, i), n))
			self = (int)i;
	if (self < 0)
		return r;
	int depth = 0;
	for (int i = self + (open ? 1 : -1); i >= 0 && i < (int)count; i += open ? 1 : -1) {
		const TSNode s = ts_node_child(parent, (uint32_t)i);
		if (ts_node_is_named(s) || ts_node_is_missing(s))
			continue;
		const char t = bracket_char(ts_node_type(s));
		if (t == c)
			++depth;
		else if (t == want && depth-- == 0) {
			const int a = (int)ts_node_end_byte(n) - 1, b = (int)ts_node_end_byte(s) - 1;
			r.first = std::min(a, b);
			r.second = std::max(a, b);
			return r;
		}
	}
	return r;
}

/* --- indentation ---------------------------------------------------------- */

int leading_ws(const std::string &line) {
	int i = 0;
	while (i < (int)line.size() && (line[(size_t)i] == ' ' || line[(size_t)i] == '\t'))
		++i;
	return i;
}

int Syntax::line_indent(int line) const {
	const int a = mLineStarts[(size_t)line];
	int i = a;
	while (i < (int)mText.size() && (mText[(size_t)i] == ' ' || mText[(size_t)i] == '\t'))
		++i;
	return i - a;
}

static bool is_comment(const char *t) { return !std::strcmp(t, "line_comment") || !std::strcmp(t, "block_comment"); }

int Syntax::indent_at(int caret, int tab) const {
	caret = std::max(0, std::min(caret, (int)mText.size()));
	const int line = line_of(caret);
	const int ls = mLineStarts[(size_t)line];
	const int own = std::min(line_indent(line), caret - ls);
	/* Blank up to the caret: keep the indentation. */
	if (own == caret - ls || !mTree)
		return own;

	const TSNode root = ts_tree_root_node(mTree);
	for (TSNode n = ts_node_descendant_for_byte_range(root, (uint32_t)caret, (uint32_t)caret); !ts_node_is_null(n);
		 n = ts_node_parent(n)) {
		const char *t = ts_node_type(n);
		if ((!std::strcmp(t, "string") || !std::strcmp(t, "block_comment")) && (int)ts_node_start_byte(n) < caret &&
			caret < (int)ts_node_end_byte(n))
			return own;
	}

	struct Open {
		char c;
		int row;
		int follow; /* column of the first leaf after it on its row, -1 = none */
	};
	std::vector<Open> stack;
	TSTreeCursor cur = ts_tree_cursor_new(root);
	bool done = false;
	while (!done) {
		const TSNode n = ts_tree_cursor_current_node(&cur);
		if ((int)ts_node_start_byte(n) >= caret)
			break;
		if (ts_node_child_count(n) > 0 && ts_tree_cursor_goto_first_child(&cur))
			continue;
		/* a leaf */
		const int a = (int)ts_node_start_byte(n), b = (int)ts_node_end_byte(n);
		const char *t = ts_node_type(n);
		if (b <= caret && b > a && !is_comment(t)) {
			const TSPoint p = ts_node_start_point(n);
			if (!stack.empty() && stack.back().follow < 0 && stack.back().row == (int)p.row)
				stack.back().follow = (int)p.column;
			const char c = ts_node_is_named(n) ? 0 : bracket_char(t);
			if (c == '(' || c == '[' || c == '{')
				stack.push_back(Open{c, (int)p.row, -1});
			else if (c && !stack.empty() && stack.back().c == partner_of(c))
				stack.pop_back();
		}
		while (!ts_tree_cursor_goto_next_sibling(&cur)) {
			if (!ts_tree_cursor_goto_parent(&cur)) {
				done = true;
				break;
			}
		}
	}
	ts_tree_cursor_delete(&cur);

	if (stack.empty())
		return line_indent(line);
	const Open &o = stack.back();
	if (o.c == '{' || o.follow < 0)
		return line_indent(o.row) + tab;
	return o.follow; /* align with the first argument after `(` or `[` */
}

int Syntax::reindent_at(int caret, int tab) const {
	if (caret <= 0 || caret > (int)mText.size())
		return -1;
	const char c = mText[(size_t)caret - 1];
	if (c != ')' && c != ']' && c != '}')
		return -1;
	const int line = line_of(caret - 1);
	const int cur = line_indent(line);
	if (mLineStarts[(size_t)line] + cur != caret - 1)
		return -1; /* the closer is not the first thing on the line */
	const BracketMatch m = match_bracket(caret);
	if (!m.valid())
		return -1;
	const int want = line_indent(line_of(m.first));
	return want != cur ? want : -1;
}

/* --- evaluation span ------------------------------------------------------ */

EvalSpan Syntax::eval_span(int caret, int sel_start, int sel_end, EvalMode mode) const {
	EvalSpan s;
	const int len = (int)mText.size();
	if (sel_start >= 0 && sel_end > sel_start) {
		s.start = std::min(sel_start, len);
		s.end = std::min(sel_end, len);
		return s;
	}
	if (mode == EvalModeDocument) {
		if (len > 0) {
			s.start = 0;
			s.end = len;
		}
		return s;
	}
	caret = std::max(0, std::min(caret, len));
	const int line = line_of(caret);

	/* Every top-level item on the rows spanned so far, so `a; b;` on one line
	 * goes together and a multi-line item pulls in its neighbours' rows. */
	if (mTree) {
		const TSNode root = ts_tree_root_node(mTree);
		const uint32_t count = ts_node_child_count(root);
		int lo = line, hi = line, end = -1;
		for (uint32_t i = 0; i < count; ++i) {
			const TSNode c = ts_node_child(root, i);
			if (is_comment(ts_node_type(c)) || ts_node_end_byte(c) <= ts_node_start_byte(c))
				continue;
			const int r0 = (int)ts_node_start_point(c).row, r1 = (int)ts_node_end_point(c).row;
			if (r1 < lo || r0 > hi)
				continue;
			lo = std::min(lo, r0);
			hi = std::max(hi, r1);
			end = std::max(end, (int)ts_node_end_byte(c));
		}
		if (end >= 0) {
			s.start = mLineStarts[(size_t)lo];
			s.end = end;
			return s;
		}
	}

	const int a = mLineStarts[(size_t)line];
	const int b = line + 1 < line_count() ? mLineStarts[(size_t)line + 1] - 1 : len;
	if (a + line_indent(line) < b) {
		s.start = a;
		s.end = b;
	}
	return s;
}

} // namespace ckc
