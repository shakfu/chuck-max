# tree-sitter-chuck patches

Base: [tymbalodeon/tree-sitter-chuck](https://github.com/tymbalodeon/tree-sitter-chuck) `68fb7bd` (2026-03-27).
`grammar.js` here is the patched grammar; `src/parser.c` is generated from it by
`tree-sitter generate` (CLI 0.27.2, ABI 15). The upstream test corpus (784 parses)
passes after every patch.

| # | Construct | Change |
|-|-|-|
| 1 | `@import "X";` | `import_expression` takes an optional `;` |
| 2 | `"a // b"` | `string` is one `token()`, so `//` inside it no longer starts a comment |
| 3 | `` `...` `` | backtick strings (Faust code) |
| 4 | `"\\"` | a string body is `(\\.\|[^"\\])*`, so an escaped backslash before the closing quote ends it |
| 5 | `super.f()` | `super` heads a `member_identifier`, as `this` and `me` do |
| 6 | `~a`, `@operator ~` | `~` as a prefix operator and as an overloadable operator |
| 7 | `while (x => now);` | `;` is a control-structure body |
| 8 | `1e3`, `1.5e3`, `.5e-2` | float exponents, as `chuck.lex` accepts |

Effect on the 554 `.ck` files under `examples/`: 62 files with parse errors
before, 3 after. The 3 left: `@example` annotations, a bare `{ }` block in a
class body, and chucking into a chained member (`x => a.b(0).c`).

To regenerate, from a checkout of the base commit with `grammar.js` replaced:

```sh
tree-sitter generate
tree-sitter test
cp src/parser.c src/tree_sitter/*.h <here>/src/
```
