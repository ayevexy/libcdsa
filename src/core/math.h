#ifndef LIBCDSA_MATH_H
#define LIBCDSA_MATH_H

/**
 * @file math.h
 * @brief Math utilities.
 */

#include "core/types.h"
#include "core/limits.h"
#include "core/errors.h"

/**
 * @brief Typed mathematical constants.
 */
#define MATH_E(T)       ((T) 2.718281828459045235360287471352662498L)
#define MATH_PI(T)      ((T) 3.141592653589793238462643383279502884L)
#define MATH_TAU(T)     ((T) 6.283185307179586476925286766559005768L)
#define MATH_SQRT2(T)   ((T) 1.414213562373095048801688724209698079L)
#define MATH_SQRT3(T)   ((T) 1.732050807568877293527446341505872367L)
#define MATH_LN2(T)     ((T) 0.693147180559945309417232121458176568L)
#define MATH_LN10(T)    ((T) 2.302585092994045684017991454684364208L)
#define MATH_LOG2E(T)   ((T) 1.442695040888963407359924681001892137L)
#define MATH_LOG10E(T)  ((T) 0.434294481903251827651128918916605082L)

/**
 * @brief Adds two numbers, checking for arithmetic overflow.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the sum of the numbers
 *
 * @throws ARITHMETIC_ERROR if the addition overflows
 */
#define math_add(a, b) MATH_DISPATCH(add, a, b)

/**
 * @brief Subtracts the second number from the first, checking for arithmetic overflow or underflow.
 *
 * @param a the first number
 * @param b the number to subtract
 *
 * @return the difference of the numbers
 *
 * @throws ARITHMETIC_ERROR if the subtraction overflows or underflows
 */
#define math_sub(a, b) MATH_DISPATCH(sub, a, b)

/**
 * @brief Multiplies two numbers, checking for arithmetic overflow.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the product of the numbers
 *
 * @throws ARITHMETIC_ERROR if the multiplication overflows
 */
#define math_mul(a, b) MATH_DISPATCH(mul, a, b)

/**
 * @brief Divides the first number by the second, checking for division by zero and arithmetic overflow.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the quotient of the numbers
 *
 * @throws ARITHMETIC_ERROR if the divisor is zero or the division overflows
 */
#define math_div(a, b) MATH_DISPATCH(div, a, b)

/*---------------------------------------------------------------------------------*/

#define MATH_DISPATCH(operation, a, b) _Generic((a),    \
    char:     math_##operation##_char,                  \
    schar:    math_##operation##_schar,                 \
    uchar:    math_##operation##_uchar,                 \
    short:    math_##operation##_short,                 \
    ushort:   math_##operation##_ushort,                \
    int:      math_##operation##_int,                   \
    uint:     math_##operation##_uint,                  \
    long:     math_##operation##_long,                  \
    ulong:    math_##operation##_ulong,                 \
    llong:    math_##operation##_llong,                 \
    ullong:   math_##operation##_ullong                 \
)(a, b)

#define DEFINE_SIGNED_MATH(T, suffix, min, max)                             \
                                                                            \
static inline T math_add_##suffix(T a, T b) {                               \
    if ((b > 0 && a > (max) - b) || (b < 0 && a < (min) - b)) {             \
        set_error(ARITHMETIC_ERROR, "addition overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a + b;                                                           \
}                                                                           \
                                                                            \
static inline T math_sub_##suffix(T a, T b) {                               \
    if ((b < 0 && a > (max) + b) || (b > 0 && a < (min) + b)) {             \
        set_error(ARITHMETIC_ERROR, "subtraction overflow");                \
        return 0;                                                           \
    }                                                                       \
    return a - b;                                                           \
}                                                                           \
                                                                            \
static inline T math_mul_##suffix(T a, T b) {                               \
    if (a > 0) {                                                            \
        if ((b > 0 && a > (max) / b) || (b < 0 && b < (min) / a))           \
            goto overflow;                                                  \
    } else if (a < 0) {                                                     \
        if ((b > 0 && a < (min) / b) || (b < 0 && a < (max) / b))           \
            goto overflow;                                                  \
    }                                                                       \
    return a * b;                                                           \
                                                                            \
    overflow:                                                               \
        set_error(ARITHMETIC_ERROR, "multiplication overflow");             \
        return 0;                                                           \
}                                                                           \
                                                                            \
static inline T math_div_##suffix(T a, T b) {                               \
    if (b == 0) {                                                           \
        set_error(ARITHMETIC_ERROR, "division by zero");                    \
        return 0;                                                           \
    }                                                                       \
    if (a == (min) && b == -1) {                                            \
        set_error(ARITHMETIC_ERROR, "division overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a / b;                                                           \
}


#define DEFINE_UNSIGNED_MATH(T, suffix, max)                                \
                                                                            \
static inline T math_add_##suffix(T a, T b) {                               \
    if (a > (max) - b) {                                                    \
        set_error(ARITHMETIC_ERROR, "addition overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a + b;                                                           \
}                                                                           \
                                                                            \
static inline T math_sub_##suffix(T a, T b) {                               \
    if (a < b) {                                                            \
        set_error(ARITHMETIC_ERROR, "subtraction underflow");               \
        return 0;                                                           \
    }                                                                       \
    return a - b;                                                           \
}                                                                           \
                                                                            \
static inline T math_mul_##suffix(T a, T b) {                               \
    if (b != 0 && a > (max) / b) {                                          \
        set_error(ARITHMETIC_ERROR, "multiplication overflow");             \
        return 0;                                                           \
    }                                                                       \
    return a * b;                                                           \
}                                                                           \
                                                                            \
static inline T math_div_##suffix(T a, T b) {                               \
    if (b == 0) {                                                           \
        set_error(ARITHMETIC_ERROR, "division by zero");                    \
        return 0;                                                           \
    }                                                                       \
    return a / b;                                                           \
}

#if CHAR_MIN < 0
DEFINE_SIGNED_MATH(char, char, CHAR_MIN_VALUE, CHAR_MAX_VALUE)
#else
DEFINE_UNSIGNED_MATH(char, char, CHAR_MAX_VALUE)
#endif

DEFINE_SIGNED_MATH(schar, schar, SCHAR_MIN_VALUE, SCHAR_MAX_VALUE)
DEFINE_UNSIGNED_MATH(uchar, uchar, UCHAR_MAX_VALUE)

DEFINE_SIGNED_MATH(short, short, SHORT_MIN_VALUE, SHORT_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ushort, ushort, USHORT_MAX_VALUE)

DEFINE_SIGNED_MATH(int, int, INT_MIN_VALUE, INT_MAX_VALUE)
DEFINE_UNSIGNED_MATH(uint, uint, UINT_MAX_VALUE)

DEFINE_SIGNED_MATH(long, long, LONG_MIN_VALUE, LONG_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ulong, ulong, ULONG_MAX_VALUE)

DEFINE_SIGNED_MATH(llong, llong, LLONG_MIN_VALUE, LLONG_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ullong, ullong, ULLONG_MAX_VALUE)

#endif
