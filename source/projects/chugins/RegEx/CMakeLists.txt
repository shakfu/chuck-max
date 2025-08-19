set(REGEX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/regex)

add_chugin(
    OTHER_SOURCES
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/regcomp.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/regerror.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/regexec.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-ast.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-compile.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-filter.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-match-approx.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-match-backtrack.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-match-parallel.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-mem.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-parse.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/tre-stack.c>
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}/xmalloc.c>
    INCLUDE_DIRS
        $<$<PLATFORM_ID:Windows>:${REGEX_DIR}>
    COMPILE_DEFINITIONS
        RegEx_EXPORTS
        __WINDOWS_MODERN__
        _MBCS
)