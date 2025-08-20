#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef _NDEBUG
#include "debugbreak.h"
#endif

// list of predefined macros:
//     https://sourceforge.net/p/predef/wiki/Architectures/
//     https://gcc.gnu.org/onlinedocs/cpp/Predefined-Macros.html
//     https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
//     https://clang.llvm.org/docs/LanguageExtensions.html#predefined-macros

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef NDEBUG
#define ASSERT(expression)                                                             \
    {                                                                                  \
        if (!(expression)) {                                                           \
            printf("Assertion(%s) failed: file \"%s\", line %d\n", #expression,        \
                   __FILE__, __LINE__);                                                \
            debug_break();                                                             \
        }                                                                              \
    }
#else
#define ASSERT(expression) NULL;
#endif

#define UNREACHABLE ASSERT(false);

#define FORCE_CRASH *(int*)0 = 0

#define _CODE(...) #__VA_ARGS__
#define CODE(...) _CODE(__VA_ARGS__)
#define UNUSED_VAR(x) ((void)(x))
#define UNUSED_FUNCTION(x) ((void)(x))
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))
#define ZERO_ARRAY(array) (memset((array), 0, sizeof(array)))
#define COPY_STRUCT_TYPE(dst, src, type) (memcpy((dst), (src), sizeof(type)))
#define COPY_STRUCT(dst, src) (memcpy((dst), (src), sizeof(*src)))
#define COPY_STRING(dst, src) (snprintf(dst, sizeof(dst), "%s", src));

#define WHITESPACE_CHARS " \t\n\r\f\v"

// ============================================================================
// defer
// Note: calls at end of *scope* not function
// ============================================================================

// clang-format off
template <typename F>
struct __privDefer {
	F f;
	__privDefer(F f) : f(f) {}
	~__privDefer() { f(); }
};

template <typename F>
__privDefer<F> __defer_func(F f) {
	return __privDefer<F>(f);
}
// clang-format on

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = __defer_func([&]() { code; })

// ============================================================================
// Types
// ============================================================================

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;

// ============================================================================
// Math
// ============================================================================

// Constants
#define E 2.71828182845904523536f        /* Euler's constant e */
#define LOG2E 1.44269504088896340736f    /* log2(e) */
#define LOG10E 0.434294481903251827651f  /* log10(e) */
#define LN2 0.693147180559945309417f     /* ln(2) */
#define LN10 2.30258509299404568402f     /* ln(10) */
#define PI 3.14159265358979323846f       /* pi */
#define PI2 6.28318530717958647692f      /* pi * 2 */
#define PI_2 1.57079632679489661923f     /* pi/2 */
#define PI_4 0.785398163397448309616f    /* pi/4 */
#define SQRT2 1.41421356237309504880f    /* sqrt(2) */
#define SQRT2_2 0.707106781186547524401f /* sqrt(2)/2 */

#define LINE_SIZE 256
#define PATH_SIZE 256
#define STRMAX 512

#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824

#define EPSILON 1e-5f // min float value

#define TO_RADIANS(degrees) ((PI / 180) * (degrees))
#define TO_DEGREES(radians) ((180 / PI) * (radians))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLAMP(x, lo, hi) (MIN(hi, MAX(lo, x)))

#define NEXT_POW2(x) (1 << (u32)ceil(log2(x)));

// next multiple of m, m must be power of 2
#define NEXT_MULT4(x) (((x) + 3) & ~(3)) // rounds up to multiple of 4
#define NEXT_MULT_POW2(x, m) (((x) + (m) - 1) & ~((m) - 1))

#define ALIGN_NON_POW2(n, m) ((((n) + ((m) - 1)) / (m)) * (m))
#define NEXT_MULT(n, m) ((((n) + ((m) - 1)) / (m)) * (m))