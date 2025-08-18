
add_chugin(
    OTHER_SOURCES
        regex/regcomp.c
        regex/tre-ast.c
        regex/tre-match-approx.c
        regex/tre-mem.c
        regex/xmalloc.c
        regex/regerror.c
        regex/tre-compile.c
        regex/tre-match-backtrack.c
        regex/tre-parse.c
        regex/regexec.c
        regex/tre-filter.c
        regex/tre-match-parallel.c
        regex/tre-stack.c

    INCLUDE_DIRS
        regex
    COMPILE_DEFINITIONS
        RegEx_EXPORTS
        __WINDOWS_MODERN__
        _MBCS
)