# ck_code licensing

| Files | License | Origin |
|-|-|-|
| `core/`, `platform/`, `tests/test.h`, `tests/main.cpp`, `tests/{editor,layout,palette,completion,words,paths}_test.cpp` | MIT | `py_code` in [shakfu/py](https://github.com/shakfu/py) at `249a6ec`, namespace `pyc` renamed `ckc` |
| `ck_code*.cpp`, `ck_code.h` | MIT | adapted from `py_code*.cpp` at the same commit |
| `ck_syntax.*`, `core/shreds.*`, `tests/{syntax,shreds}_test.cpp` | MIT | this repository |
| `thirdparty/tree-sitter/` | MIT | [tree-sitter](https://github.com/tree-sitter/tree-sitter), as vendored by `py_code` |
| `thirdparty/tree-sitter-chuck/` | MIT | [tymbalodeon/tree-sitter-chuck](https://github.com/tymbalodeon/tree-sitter-chuck) `68fb7bd`, patched: `thirdparty/tree-sitter-chuck/PATCHES.md` |

Language changes from `py_code`: ChucK keywords and builtins in `core/words.cpp`,
`chuck_string` for dropped paths, the `decorator` color role renamed `directive`,
and `.ck` in the save panel. The LSP client, snippets and Python environment
lookup are not carried over.
