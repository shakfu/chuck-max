// SPDX-License-Identifier: MIT
/* syntax_test.cpp -- ck_syntax: highlight roles, brackets, indentation, spans. */

#include "test.h"

#include "ck_syntax.h"

using namespace ckc;
using std::string;

namespace {

int rev = 0;

/* Role of the token covering the first occurrence of `needle` (after `from`). */
string role(Syntax &s, const string &text, const string &needle, size_t from = 0) {
	s.update(text, ++rev);
	const size_t at = text.find(needle, from);
	if (at == string::npos)
		return "<missing>";
	int line = 0, ls = 0;
	for (size_t i = 0; i < at; ++i)
		if (text[i] == '\n') {
			++line;
			ls = (int)i + 1;
		}
	const int col = (int)at - ls;
	for (const Token &t : s.tokens(line))
		if (t.col <= col && col < t.col + t.length) {
			/* the whole needle must sit in one token */
			if (col + (int)needle.size() > t.col + t.length)
				return string("<split>") + token_type_name(t.type);
			return token_type_name(t.type);
		}
	return "<none>";
}

} // namespace

TEST(syntax_query_compiles) {
	Syntax s;
	CHECK(s.query_ok());
}

TEST(syntax_tokens_cover_lines) {
	Syntax s;
	const string text = "SinOsc s => dac;\n\n  // note\nwhile (true) 1::second => now;";
	s.update(text, 1);
	CHECK(!s.has_error());
	int start = 0;
	for (int line = 0; line < s.line_count(); ++line) {
		const size_t nl = text.find('\n', (size_t)start);
		const int len = (int)(nl == string::npos ? text.size() : nl) - start;
		int col = 0;
		for (const Token &t : s.tokens(line)) {
			CHECK_EQ(t.col, col);
			col += t.length;
		}
		CHECK_EQ(col, len);
		start += len + 1;
	}
}

TEST(syntax_roles) {
	Syntax s;
	const string a = "SinOsc s => Gain g => dac;";
	CHECK_EQ(role(s, a, "SinOsc"), string("class"));
	CHECK_EQ(role(s, a, "=>"), string("operator"));
	CHECK_EQ(role(s, a, "dac"), string("builtin"));
	CHECK_EQ(role(s, a, "s", 7), string("default"));

	const string f = "fun void tick(int n) { <<< n >>>; }";
	CHECK_EQ(role(s, f, "fun"), string("keyword"));
	CHECK_EQ(role(s, f, "void"), string("class"));
	CHECK_EQ(role(s, f, "tick"), string("function"));
	CHECK_EQ(role(s, f, "int"), string("class"));
	CHECK_EQ(role(s, f, "{"), string("bracket"));
	CHECK_EQ(role(s, f, "<<<"), string("bracket"));

	const string c = "tick(n); s.gain(0.5);";
	CHECK_EQ(role(s, c, "tick"), string("function"));
	CHECK_EQ(role(s, c, "n)"), string("<split>default")); /* an argument, not the callee */
	CHECK_EQ(role(s, c, "gain"), string("function"));
	CHECK_EQ(role(s, c, "0.5"), string("number"));

	const string t = "while (true) { 1::second => now; spork ~ tick(3); }";
	CHECK_EQ(role(s, t, "while"), string("keyword"));
	CHECK_EQ(role(s, t, "true"), string("builtin"));
	CHECK_EQ(role(s, t, "::"), string("operator"));
	CHECK_EQ(role(s, t, "second"), string("builtin"));
	CHECK_EQ(role(s, t, "now"), string("builtin"));
	CHECK_EQ(role(s, t, "spork"), string("keyword"));

	const string m = "this.x; me.yield(); super.go();";
	CHECK_EQ(role(s, m, "this"), string("self"));
	CHECK_EQ(role(s, m, "me"), string("self"));
	CHECK_EQ(role(s, m, "super"), string("self"));

	const string d = "@import \"Foo\";\nclass Bar extends Object { }";
	CHECK_EQ(role(s, d, "@import"), string("directive"));
	CHECK_EQ(role(s, d, "\"Foo\""), string("string"));
	CHECK_EQ(role(s, d, "class"), string("keyword"));
	CHECK_EQ(role(s, d, "Bar"), string("class"));
	CHECK_EQ(role(s, d, "Object"), string("class"));

	const string n = "0x1f => int h; 1.5e3 => float f; #(1,2) => complex z;";
	CHECK_EQ(role(s, n, "0x1f"), string("number"));
	CHECK_EQ(role(s, n, "1.5e3"), string("number"));
	CHECK_EQ(role(s, n, "#("), string("bracket"));

	const string k = "\"a // b\" => string q; // c\n/* d */";
	CHECK_EQ(role(s, k, "\"a // b\""), string("string")); /* not a comment */
	CHECK_EQ(role(s, k, "// c"), string("comment"));
	CHECK_EQ(role(s, k, "/* d */"), string("comment"));
}

TEST(syntax_incremental) {
	Syntax s;
	s.update("1 => int a;", 1);
	CHECK_EQ(s.parses(), 1);
	s.update("1 => int a;", 1); /* same revision */
	CHECK_EQ(s.parses(), 1);
	s.update("1 => int ab;", 2);
	CHECK_EQ(s.parses(), 2);
	CHECK_EQ(s.text(), string("1 => int ab;"));
	string out;
	for (const Token &t : s.tokens(0))
		out += token_type_name(t.type) + string(1, ' ');
	CHECK(out.find("class") != string::npos);
}

TEST(syntax_brackets) {
	Syntax s;
	const string a = "foo(bar[1]);";
	s.update(a, 1);
	BracketMatch m = s.match_bracket(3); /* before ( */
	CHECK_EQ(m.first, 3);
	CHECK_EQ(m.second, 10);
	m = s.match_bracket(10); /* after ] */
	CHECK_EQ(m.first, 7);
	CHECK_EQ(m.second, 9);
	m = s.match_bracket(11); /* after ) */
	CHECK_EQ(m.first, 3);
	CHECK_EQ(m.second, 10);
	CHECK(!s.match_bracket(1).valid());

	const string c = "#(1,2) => complex z;";
	s.update(c, 2);
	m = s.match_bracket(6); /* after ) */
	CHECK_EQ(m.first, 1);   /* the ( of #( */
	CHECK_EQ(m.second, 5);
	m = s.match_bracket(1); /* between # and ( */
	CHECK_EQ(m.first, 1);
	CHECK_EQ(m.second, 5);

	const string q = "\"(\" => string p; ( 1 );";
	s.update(q, 3);
	CHECK(!s.match_bracket(1).valid()); /* inside a string */
	m = s.match_bracket(17);
	CHECK_EQ(m.first, 17);
	CHECK_EQ(m.second, 21);
}

TEST(syntax_indent) {
	Syntax s;
	auto at_end = [&](const string &t) {
		s.update(t, ++rev);
		return s.indent_at((int)t.size(), 4);
	};
	CHECK_EQ(at_end("while (true) {"), 4);
	CHECK_EQ(at_end("while (true) {\n    1::ms => now;"), 4);
	CHECK_EQ(at_end("while (true) {\n    if (x) {"), 8);
	CHECK_EQ(at_end("while (true) {\n    if (x) {\n    }"), 4);
	CHECK_EQ(at_end("while (true) {\n}"), 0);
	CHECK_EQ(at_end("foo(a,"), 4);  /* aligned with the first argument */
	CHECK_EQ(at_end("  foo("), 6);  /* nothing after (: one level in */
	CHECK_EQ(at_end("    1 => int a;"), 4);
	CHECK_EQ(at_end("    "), 4);    /* blank: kept */
	CHECK_EQ(at_end("{ // {"), 4);  /* comments do not open */
}

TEST(syntax_reindent) {
	Syntax s;
	string t = "while (true) {\n    1::ms => now;\n    }";
	s.update(t, ++rev);
	CHECK_EQ(s.reindent_at((int)t.size(), 4), 0);
	t = "while (true) {\n}";
	s.update(t, ++rev);
	CHECK_EQ(s.reindent_at((int)t.size(), 4), -1); /* already right */
	t = "if (x) { 1; }";
	s.update(t, ++rev);
	CHECK_EQ(s.reindent_at((int)t.size(), 4), -1); /* not first on its line */
}

TEST(syntax_eval_span) {
	Syntax s;
	const string doc = "fun void f() {\n  1;\n}\na; b;\n\nc;";
	s.update(doc, ++rev);
	EvalSpan e = s.eval_span(0, 0, 0, EvalModeDocument);
	CHECK_EQ(e.start, 0);
	CHECK_EQ(e.end, (int)doc.size());

	e = s.eval_span(0, 2, 5, EvalModeDocument); /* selection wins */
	CHECK_EQ(e.start, 2);
	CHECK_EQ(e.end, 5);

	e = s.eval_span(16, 0, 0, EvalModeBlock); /* inside the function */
	CHECK_EQ(e.start, 0);
	CHECK_EQ(e.end, 21);

	const int line3 = (int)doc.find("a;");
	e = s.eval_span(line3, 0, 0, EvalModeBlock); /* both statements on the line */
	CHECK_EQ(e.start, line3);
	CHECK_EQ(e.end, line3 + 5);

	const int blank = line3 + 6;
	e = s.eval_span(blank, 0, 0, EvalModeBlock);
	CHECK(!e.has_value());

	s.update("", ++rev);
	CHECK(!s.eval_span(0, 0, 0, EvalModeDocument).has_value());
}
