#ifndef LIBCDSA_MATH_H
#define LIBCDSA_MATH_H

/**
 * @file math.h
 * @brief Math utilities.
 */

#include "core/types.h"

/**
 * @brief Typed mathematical constants.
 */
#define MATH_E(T)        ((T) 2.718281828459045235360287471352662498L)
#define MATH_PI(T)       ((T) 3.141592653589793238462643383279502884L)
#define MATH_TAU(T)      ((T) 6.283185307179586476925286766559005768L)
#define MATH_SQRT1_2(T)  ((T) 0.707106781186547524400844362104849039L)
#define MATH_SQRT2(T)    ((T) 1.414213562373095048801688724209698079L)
#define MATH_SQRT3(T)    ((T) 1.732050807568877293527446341505872367L)
#define MATH_LN2(T)      ((T) 0.693147180559945309417232121458176568L)
#define MATH_LN10(T)     ((T) 2.302585092994045684017991454684364208L)
#define MATH_LOG2E(T)    ((T) 1.442695040888963407359924681001892137L)
#define MATH_LOG10E(T)   ((T) 0.434294481903251827651128918916605082L)

/**
 * @brief Adds two numbers, checking for arithmetic overflow.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the sum of the numbers
 *
 * @exception ARITHMETIC_ERROR if the addition overflows
 */
#define math_add_exact(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(add_exact, a, b)

/**
 * @brief Subtracts the second number from the first, checking for arithmetic overflow or underflow.
 *
 * @param a the first number
 * @param b the number to subtract
 *
 * @return the difference of the numbers
 *
 * @exception ARITHMETIC_ERROR if the subtraction overflows or underflows
 */
#define math_sub_exact(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(sub_exact, a, b)

/**
 * @brief Multiplies two numbers, checking for arithmetic overflow.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the product of the numbers
 *
 * @exception ARITHMETIC_ERROR if the multiplication overflows
 */
#define math_mul_exact(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(mul_exact, a, b)

/**
 * @brief Divides the first number by the second, checking for division by zero and arithmetic overflow.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the quotient of the numbers
 *
 * @exception ARITHMETIC_ERROR if the divisor is zero or the division overflows
 */
#define math_div_exact(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(div_exact, a, b)

/**
 * @brief Raises a number to a given power.
 *
 * @param base the base value
 * @param exponent the exponent value
 *
 * @return the result of raising the base to the exponent
 */
#define math_pow(base, exponent) MATH_DISPATCH_FLOATS_TWO_ARGS(pow, base, exponent)

/**
 * @brief Computes the natural logarithm of a number.
 *
 * @param n the input value
 *
 * @return the natural logarithm of n
 */
#define math_log(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(log, n)

/**
 * @brief Computes the base-2 logarithm of a number.
 *
 * @param n the input value
 *
 * @return the base-2 logarithm of n
 */
#define math_log2(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(log2, n)

/**
 * @brief Computes the base-10 logarithm of a number.
 *
 * @param n the input value
 *
 * @return the base-10 logarithm of n
 */
#define math_log10(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(log10, n)

/**
 * @brief Computes the natural logarithm of one plus a number.
 *
 * @param n the input value
 *
 * @return the natural logarithm of one plus n
 */
#define math_log1p(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(log1p, n)

/**
 * @brief Computes the exponential of a number.
 *
 * @param n the exponent
 *
 * @return e raised to the power of n
 */
#define math_exp(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(exp, n)

/**
 * @brief Computes two raised to the power of a number.
 *
 * @param n the exponent
 *
 * @return two raised to the power of n
 */
#define math_exp2(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(exp2, n)

/**
 * @brief Computes the exponential of a number minus one.
 *
 * @param n the exponent
 *
 * @return e raised to the power of n, minus one
 */
#define math_expm1(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(expm1, n)

/**
 * @brief Computes the square root of a number.
 *
 * @param n the input value
 *
 * @return the square root of n
 */
#define math_sqrt(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(sqrt, n)

/**
 * @brief Computes the cube root of a number.
 *
 * @param n the input value
 *
 * @return the cube root of n
 */
#define math_cbrt(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(cbrt, n)

/**
 * @brief Computes the hypotenuse of a right triangle.
 *
 * @param x the first side
 * @param y the second side
 *
 * @return the length of the hypotenuse
 */
#define math_hypot(x, y) MATH_DISPATCH_FLOATS_TWO_ARGS(hypot, x, y)

/**
 * @brief Returns the sine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the sine of n
 */
#define math_sin(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(sin, n)

/**
 * @brief Returns the cosine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the cosine of n
 */
#define math_cos(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(cos, n)

/**
 * @brief Returns the tangent of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the tangent of n
 */
#define math_tan(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(tan, n)

/**
 * @brief Returns the arc sine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc sine of n in radians
 */
#define math_asin(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(asin, n)

/**
 * @brief Returns the arc cosine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc cosine of n in radians
 */
#define math_acos(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(acos, n)

/**
 * @brief Returns the arc tangent of a value.
 *
 * @param n the value
 *
 * @return the arc tangent of n in radians
 */
#define math_atan(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(atan, n)

/**
 * @brief Returns the arc tangent of y / x, using the signs of both arguments
 * to determine the quadrant.
 *
 * @param y the y-coordinate
 * @param x the x-coordinate
 *
 * @return the arc tangent of y / x in radians
 */
#define math_atan2(y, x) MATH_DISPATCH_FLOATS_TWO_ARGS(atan2, y, x)

/**
 * @brief Returns the hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic sine of n
 */
#define math_sinh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(sinh, n)

/**
 * @brief Returns the hyperbolic cosine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic cosine of n
 */
#define math_cosh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(cosh, n)

/**
 * @brief Returns the hyperbolic tangent of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic tangent of n
 */
#define math_tanh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(tanh, n)

/**
 * @brief Returns the inverse hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the inverse hyperbolic sine of n
 */
#define math_asinh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(asinh, n)

/**
 * @brief Returns the inverse hyperbolic cosine of a value.
 *
 * @param n the value, greater than or equal to 1
 *
 * @return the inverse hyperbolic cosine of n
 */
#define math_acosh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(acosh, n)

/**
 * @brief Returns the inverse hyperbolic tangent of a value.
 *
 * @param n the value, in the range -1..1 (exclusive)
 *
 * @return the inverse hyperbolic tangent of n
 */
#define math_atanh(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(atanh, n)

/**
 * @brief Returns the greatest common divisor (GCD) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the greatest common divisor
 */
#define math_gcd(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(gcd, a, b)

/**
 * @brief Returns the least common multiple (LCM) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the least common multiple
 */
#define math_lcm(a, b) MATH_DISPATCH_INTEGERS_TWO_ARGS(lcm, a, b)

/**
 * @brief Calculates the factorial of a number.
 *
 * @param n the number
 *
 * @return the factorial of n
 *
 * @note Returns the minimum value of the type for negative inputs
 *       and the maximum value if the result overflows.
 */
#define math_factorial(n) MATH_DISPATCH_INTEGERS_SINGLE_ARG(factorial, n)

/**
 * @brief Checks whether a number is even.
 *
 * @param n the number
 *
 * @return true if even, false otherwise
 */
#define math_is_even(n) (n % 2 == 0)

/**
 * @brief Checks whether a number is odd.
 *
 * @param n the number
 *
 * @return true if odd, false otherwise
 */
#define math_is_odd(n) (n % 2 != 0)

/**
 * @brief Checks whether a number is prime.
 *
 * @param n the number
 *
 * @return true if prime, false otherwise
 */
#define math_is_prime(n) MATH_DISPATCH_INTEGERS_SINGLE_ARG(is_prime, n)

/**
 * @brief Checks whether a number is power of two.
 *
 * @param n the number
 *
 * @return true if n is power of two, false otherwise
 */
#define math_is_power_of_two(n) MATH_DISPATCH_INTEGERS_SINGLE_ARG(is_power_of_two, n)

/**
 * @brief Returns the absolute value of a number.
 *
 * @param n the number
 *
 * @return the absolute value of n
 */
#define math_abs(n) MATH_DISPATCH_NUMBERS_SINGLE_ARG(abs, n)

/**
 * @brief Returns the minimum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the minimum number
 */
#define math_min(a, b) MATH_DISPATCH_NUMBERS_TWO_ARGS(min, a, b)

/**
 * @brief Returns the maximum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the maximum number
 */
#define math_max(a, b) MATH_DISPATCH_NUMBERS_TWO_ARGS(max, a, b)

/**
 * @brief Restricts a value to a specified range.
 *
 * @param value the value to be restricted
 * @param min the minimum value
 * @param max the maximum value
 *
 * @return a clamped value within the min..max interval
 */
#define math_clamp(value, min, max) math_min(math_max(value, min), max)

/**
 * @brief Returns the sign of a floating-point number.
 *
 * @param n the number
 *
 * @return -1 if n is negative, 0 if n is zero, or 1 if n is positive
 */
#define math_sign(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(sign, n)

/**
 * @brief Returns a value with the magnitude of one floating-point number and the sign of another.
 *
 * @param value the value whose magnitude is used
 * @param sign the value whose sign is copied
 *
 * @return the value with the magnitude of value and the sign of sign
 */
#define math_copysign(value, sign) MATH_DISPATCH_FLOATS_TWO_ARGS(copysign, value, sign)

/**
 * @brief Decomposes a floating-point number into a normalized fraction and an integer exponent.
 *
 * @param n the number to decompose
 * @param exponent pointer to an integer where the exponent is stored
 *
 * @return the normalized fraction
 */
#define math_frexp(n, exponent) _Generic((n),   \
    float:    math_frexp_float,                 \
    double:   math_frexp_double,                \
    ldouble:  math_frexp_ldouble                \
)(n, exponent)

/**
 * @brief Calculates the IEEE floating-point remainder of two numbers.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the remainder of a divided by b
 */
#define math_remainder(a, b) MATH_DISPATCH_FLOATS_TWO_ARGS(remainder, a, b)

/**
 * @brief Computes the floating-point remainder of a division.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the remainder of dividing a by b
 */
#define math_mod(a, b) MATH_DISPATCH_FLOATS_TWO_ARGS(mod, a, b)

/**
 * @brief Computes the fused multiply-add operation: a * b + c.
 *
 * @param a the first multiplicand
 * @param b the second multiplicand
 * @param c the addend
 *
 * @return the result of a * b + c, computed with a single rounding
 */
#define math_fma(a, b, c) MATH_DISPATCH_FLOATS_THREE_ARGS(fma, a, b, c)

/**
 * @brief Rounds a floating-point number to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_round(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(round, n)

/**
 * @brief Rounds a floating-point number down to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_floor(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(floor, n)

/**
 * @brief Rounds a floating-point number up to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_ceil(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(ceil, n)

/**
 * @brief Truncates the fractional part of a floating-point number.
 *
 * @param n the number
 *
 * @return the truncated number
 */
#define math_trunc(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(trunc, n)

/**
 * @brief Checks whether a floating-point number is NaN.
 *
 * @param n the number
 *
 * @return true if n is NaN, false otherwise
 */
#define math_is_nan(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(is_nan, n)

/**
 * @brief Checks whether a floating-point number is infinite.
 *
 * @param n the number
 *
 * @return true if n is infinite, false otherwise
 */
#define math_is_infinite(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(is_infinite, n)

/**
 * @brief Checks whether a floating-point number is finite.
 *
 * @param n the number
 *
 * @return true if n is finite, false otherwise
 */
#define math_is_finite(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(is_finite, n)

/**
 * @brief Checks whether a floating-point number is normal.
 *
 * @param n the number
 *
 * @return true if n is normal, false otherwise
 */
#define math_is_normal(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(is_normal, n)

/**
 * @brief Checks whether two floating-point numbers are approximately equal.
 *
 * @param a the first number
 * @param b the second number
 * @param tolerance the maximum allowed absolute difference
 *
 * @return true if the absolute difference between a and b is within tolerance, false otherwise
 */
#define math_is_close(a, b, tolerance) MATH_DISPATCH_FLOATS_THREE_ARGS(is_close, a, b, tolerance)

/**
 * @brief Linearly interpolates between two floating-point numbers.
 *
 * @param start the starting value
 * @param end the ending value
 * @param amount the interpolation amount (0.0 to 1.0)
 *
 * @return the interpolated value
 */
#define math_lerp(start, end, amount) MATH_DISPATCH_FLOATS_THREE_ARGS(lerp, start, end, amount)

/**
 * @brief Calculates a percentage of a floating-point number.
 *
 * @param value the value
 * @param percentage_ the percentage to calculate
 *
 * @return the calculated percentage of the value
 */
#define math_percentage(value, percentage_) MATH_DISPATCH_FLOATS_TWO_ARGS(percentage, value, percentage_)

/**
 * @brief Generates a pseudo-random floating-point number.
 *
 * @param T the floating-point type of the generated number
 *
 * @return a pseudo-random number in the range [0.0, 1.0)
 */
#define math_random(T) _Generic((T) 0,  \
    float:    math_random_float,        \
    double:   math_random_double,       \
    ldouble:  math_random_ldouble       \
)()

/**
 * @brief Converts degrees to radians.
 *
 * @param degrees the value in degrees
 *
 * @return the value in radians
 */
#define math_to_radians(degrees) MATH_DISPATCH_FLOATS_SINGLE_ARG(to_radians, degrees)

/**
 * @brief Converts radians to degrees.
 *
 * @param radians the value in radians
 *
 * @return the value in degrees
 */
#define math_to_degrees(radians) MATH_DISPATCH_FLOATS_SINGLE_ARG(to_degrees, radians)

/*---------------------------------------------------------------------------------*/

#define MATH_INVALID_TYPE ((void) 0)

#define MATH_INTEGER_TYPES(operation)       \
    char:    math_##operation##_char,       \
    schar:   math_##operation##_schar,      \
    uchar:   math_##operation##_uchar,      \
    short:   math_##operation##_short,      \
    ushort:  math_##operation##_ushort,     \
    int:     math_##operation##_int,        \
    uint:    math_##operation##_uint,       \
    long:    math_##operation##_long,       \
    ulong:   math_##operation##_ulong,      \
    llong:   math_##operation##_llong,      \
    ullong:  math_##operation##_ullong

#define MATH_FLOAT_TYPES(operation)         \
    float:   math_##operation##_float,      \
    double:  math_##operation##_double,     \
    ldouble: math_##operation##_ldouble

#define MATH_NUMBER_TYPES(operation)        \
    MATH_INTEGER_TYPES(operation),          \
    MATH_FLOAT_TYPES(operation)

#define MATH_INTEGER_MATCH(operation, T, b) \
    T: _Generic((b), T: math_##operation##_##T, default: MATH_INVALID_TYPE)

#define MATH_FLOAT_MATCH(operation, T, b) \
    T: _Generic((b), T: math_##operation##_##T, default: MATH_INVALID_TYPE)

#define MATH_INTEGER_MATCH_THREE(operation, T, b, c) \
    T: _Generic((b), T: _Generic((c), T: math_##operation##_##T, default: MATH_INVALID_TYPE), default: MATH_INVALID_TYPE)

#define MATH_FLOAT_MATCH_THREE(operation, T, b, c) \
    T: _Generic((b), T: _Generic((c), T: math_##operation##_##T, default: MATH_INVALID_TYPE), default: MATH_INVALID_TYPE)

/* Integers */

#define MATH_DISPATCH_INTEGERS_SINGLE_ARG(operation, a) \
    _Generic((a), MATH_INTEGER_TYPES(operation))(a)

#define MATH_DISPATCH_INTEGERS_TWO_ARGS(operation, a, b)    \
    _Generic((a),                                           \
        MATH_INTEGER_MATCH(operation, char, b),             \
        MATH_INTEGER_MATCH(operation, schar, b),            \
        MATH_INTEGER_MATCH(operation, uchar, b),            \
        MATH_INTEGER_MATCH(operation, short, b),            \
        MATH_INTEGER_MATCH(operation, ushort, b),           \
        MATH_INTEGER_MATCH(operation, int, b),              \
        MATH_INTEGER_MATCH(operation, uint, b),             \
        MATH_INTEGER_MATCH(operation, long, b),             \
        MATH_INTEGER_MATCH(operation, ulong, b),            \
        MATH_INTEGER_MATCH(operation, llong, b),            \
        MATH_INTEGER_MATCH(operation, ullong, b)            \
    )(a, b)

/* Floats */

#define MATH_DISPATCH_FLOATS_SINGLE_ARG(operation, a, ...) \
    _Generic((a), MATH_FLOAT_TYPES(operation))(a __VA_OPT__(,) __VA_ARGS__)

#define MATH_DISPATCH_FLOATS_TWO_ARGS(operation, a, b)  \
    _Generic((a),                                       \
        MATH_FLOAT_MATCH(operation, float, b),          \
        MATH_FLOAT_MATCH(operation, double, b),         \
        MATH_FLOAT_MATCH(operation, ldouble, b)         \
    )(a, b)

#define MATH_DISPATCH_FLOATS_THREE_ARGS(operation, a, b, c)     \
    _Generic((a),                                               \
        MATH_FLOAT_MATCH_THREE(operation, float, b, c),         \
        MATH_FLOAT_MATCH_THREE(operation, double, b, c),        \
        MATH_FLOAT_MATCH_THREE(operation, ldouble, b, c)        \
    )(a, b, c)

/* Numbers */

#define MATH_DISPATCH_NUMBERS_SINGLE_ARG(operation, a, ...) \
    _Generic((a), MATH_NUMBER_TYPES(operation))(a __VA_OPT__(,) __VA_ARGS__)

#define MATH_DISPATCH_NUMBERS_TWO_ARGS(operation, a, b)     \
    _Generic((a),                                           \
        MATH_INTEGER_MATCH(operation, char, b),             \
        MATH_INTEGER_MATCH(operation, schar, b),            \
        MATH_INTEGER_MATCH(operation, uchar, b),            \
        MATH_INTEGER_MATCH(operation, short, b),            \
        MATH_INTEGER_MATCH(operation, ushort, b),           \
        MATH_INTEGER_MATCH(operation, int, b),              \
        MATH_INTEGER_MATCH(operation, uint, b),             \
        MATH_INTEGER_MATCH(operation, long, b),             \
        MATH_INTEGER_MATCH(operation, ulong, b),            \
        MATH_INTEGER_MATCH(operation, llong, b),            \
        MATH_INTEGER_MATCH(operation, ullong, b),           \
        MATH_FLOAT_MATCH(operation, float, b),              \
        MATH_FLOAT_MATCH(operation, double, b),             \
        MATH_FLOAT_MATCH(operation, ldouble, b)             \
    )(a, b)

/* char */

char math_add_exact_char(char a, char b);
char math_sub_exact_char(char a, char b);
char math_mul_exact_char(char a, char b);
char math_div_exact_char(char a, char b);
char math_gcd_char(char a, char b);
char math_lcm_char(char a, char b);
char math_factorial_char(char n);
bool math_is_prime_char(char n);
bool math_is_power_of_two_char(char n);
char math_abs_char(char n);
char math_min_char(char a, char b);
char math_max_char(char a, char b);

/* schar */

schar math_add_exact_schar(schar a, schar b);
schar math_sub_exact_schar(schar a, schar b);
schar math_mul_exact_schar(schar a, schar b);
schar math_div_exact_schar(schar a, schar b);
schar math_gcd_schar(schar a, schar b);
schar math_lcm_schar(schar a, schar b);
schar math_factorial_schar(schar n);
bool math_is_prime_schar(schar n);
bool math_is_power_of_two_schar(schar n);
schar math_abs_schar(schar n);
schar math_min_schar(schar a, schar b);
schar math_max_schar(schar a, schar b);

/* uchar */

uchar math_add_exact_uchar(uchar a, uchar b);
uchar math_sub_exact_uchar(uchar a, uchar b);
uchar math_mul_exact_uchar(uchar a, uchar b);
uchar math_div_exact_uchar(uchar a, uchar b);
uchar math_gcd_uchar(uchar a, uchar b);
uchar math_lcm_uchar(uchar a, uchar b);
uchar math_factorial_uchar(uchar n);
bool math_is_prime_uchar(uchar n);
bool math_is_power_of_two_uchar(uchar n);
uchar math_abs_uchar(uchar n);
uchar math_min_uchar(uchar a, uchar b);
uchar math_max_uchar(uchar a, uchar b);

/* short */

short math_add_exact_short(short a, short b);
short math_sub_exact_short(short a, short b);
short math_mul_exact_short(short a, short b);
short math_div_exact_short(short a, short b);
short math_gcd_short(short a, short b);
short math_lcm_short(short a, short b);
short math_factorial_short(short n);
bool math_is_prime_short(short n);
bool math_is_power_of_two_short(short n);
short math_abs_short(short n);
short math_min_short(short a, short b);
short math_max_short(short a, short b);

/* ushort */

ushort math_add_exact_ushort(ushort a, ushort b);
ushort math_sub_exact_ushort(ushort a, ushort b);
ushort math_mul_exact_ushort(ushort a, ushort b);
ushort math_div_exact_ushort(ushort a, ushort b);
ushort math_gcd_ushort(ushort a, ushort b);
ushort math_lcm_ushort(ushort a, ushort b);
ushort math_factorial_ushort(ushort n);
bool math_is_prime_ushort(ushort n);
bool math_is_power_of_two_ushort(ushort n);
ushort math_abs_ushort(ushort n);
ushort math_min_ushort(ushort a, ushort b);
ushort math_max_ushort(ushort a, ushort b);

/* int */

int math_add_exact_int(int a, int b);
int math_sub_exact_int(int a, int b);
int math_mul_exact_int(int a, int b);
int math_div_exact_int(int a, int b);
int math_gcd_int(int a, int b);
int math_lcm_int(int a, int b);
int math_factorial_int(int n);
bool math_is_prime_int(int n);
bool math_is_power_of_two_int(int n);
int math_abs_int(int n);
int math_min_int(int a, int b);
int math_max_int(int a, int b);

/* uint */

uint math_add_exact_uint(uint a, uint b);
uint math_sub_exact_uint(uint a, uint b);
uint math_mul_exact_uint(uint a, uint b);
uint math_div_exact_uint(uint a, uint b);
uint math_gcd_uint(uint a, uint b);
uint math_lcm_uint(uint a, uint b);
uint math_factorial_uint(uint n);
bool math_is_prime_uint(uint n);
bool math_is_power_of_two_uint(uint n);
uint math_abs_uint(uint n);
uint math_min_uint(uint a, uint b);
uint math_max_uint(uint a, uint b);

/* long */

long math_add_exact_long(long a, long b);
long math_sub_exact_long(long a, long b);
long math_mul_exact_long(long a, long b);
long math_div_exact_long(long a, long b);
long math_gcd_long(long a, long b);
long math_lcm_long(long a, long b);
long math_factorial_long(long n);
bool math_is_prime_long(long n);
bool math_is_power_of_two_long(long n);
long math_abs_long(long n);
long math_min_long(long a, long b);
long math_max_long(long a, long b);

/* ulong */

ulong math_add_exact_ulong(ulong a, ulong b);
ulong math_sub_exact_ulong(ulong a, ulong b);
ulong math_mul_exact_ulong(ulong a, ulong b);
ulong math_div_exact_ulong(ulong a, ulong b);
ulong math_gcd_ulong(ulong a, ulong b);
ulong math_lcm_ulong(ulong a, ulong b);
ulong math_factorial_ulong(ulong n);
bool math_is_prime_ulong(ulong n);
bool math_is_power_of_two_ulong(ulong n);
ulong math_abs_ulong(ulong n);
ulong math_min_ulong(ulong a, ulong b);
ulong math_max_ulong(ulong a, ulong b);

/* llong */

llong math_add_exact_llong(llong a, llong b);
llong math_sub_exact_llong(llong a, llong b);
llong math_mul_exact_llong(llong a, llong b);
llong math_div_exact_llong(llong a, llong b);
llong math_gcd_llong(llong a, llong b);
llong math_lcm_llong(llong a, llong b);
llong math_factorial_llong(llong n);
bool math_is_prime_llong(llong n);
bool math_is_power_of_two_llong(llong n);
llong math_abs_llong(llong n);
llong math_min_llong(llong a, llong b);
llong math_max_llong(llong a, llong b);

/* ullong */

ullong math_add_exact_ullong(ullong a, ullong b);
ullong math_sub_exact_ullong(ullong a, ullong b);
ullong math_mul_exact_ullong(ullong a, ullong b);
ullong math_div_exact_ullong(ullong a, ullong b);
ullong math_gcd_ullong(ullong a, ullong b);
ullong math_lcm_ullong(ullong a, ullong b);
ullong math_factorial_ullong(ullong n);
bool math_is_prime_ullong(ullong n);
bool math_is_power_of_two_ullong(ullong n);
ullong math_abs_ullong(ullong n);
ullong math_min_ullong(ullong a, ullong b);
ullong math_max_ullong(ullong a, ullong b);

/* float */

float math_pow_float(float base, float exponent);
float math_log_float(float n);
float math_log2_float(float n);
float math_log10_float(float n);
float math_log1p_float(float n);

float math_exp_float(float n);
float math_exp2_float(float n);
float math_expm1_float(float n);
float math_sqrt_float(float n);
float math_cbrt_float(float n);
float math_hypot_float(float x, float y);

float math_sin_float(float n);
float math_cos_float(float n);
float math_tan_float(float n);

float math_asin_float(float n);
float math_acos_float(float n);
float math_atan_float(float n);
float math_atan2_float(float y, float x);

float math_sinh_float(float n);
float math_cosh_float(float n);
float math_tanh_float(float n);

float math_asinh_float(float n);
float math_acosh_float(float n);
float math_atanh_float(float n);

float math_abs_float(float n);
float math_min_float(float a, float b);
float math_max_float(float a, float b);
float math_sign_float(float n);
float math_copysign_float(float value, float sign);
float math_frexp_float(float n, int* exponent);
float math_remainder_float(float a, float b);
float math_mod_float(float a, float b);
float math_fma_float(float a, float b, float c);

float math_round_float(float n);
float math_floor_float(float n);
float math_ceil_float(float n);
float math_trunc_float(float n);

bool math_is_nan_float(float n);
bool math_is_infinite_float(float n);
bool math_is_finite_float(float n);
bool math_is_normal_float(float n);

float math_is_close_float(float a, float b, float tolerance);
float math_lerp_float(float start, float end, float amount);
float math_percentage_float(float value, float percentage);
float math_random_float(void);

float math_to_radians_float(float degrees);
float math_to_degrees_float(float radians);

/* double */

double math_pow_double(double base, double exponent);
double math_log_double(double n);
double math_log2_double(double n);
double math_log10_double(double n);
double math_log1p_double(double n);

double math_exp_double(double n);
double math_exp2_double(double n);
double math_expm1_double(double n);
double math_sqrt_double(double n);
double math_cbrt_double(double n);
double math_hypot_double(double x, double y);

double math_sin_double(double n);
double math_cos_double(double n);
double math_tan_double(double n);

double math_asin_double(double n);
double math_acos_double(double n);
double math_atan_double(double n);
double math_atan2_double(double y, double x);

double math_sinh_double(double n);
double math_cosh_double(double n);
double math_tanh_double(double n);

double math_asinh_double(double n);
double math_acosh_double(double n);
double math_atanh_double(double n);

double math_abs_double(double n);
double math_min_double(double a, double b);
double math_max_double(double a, double b);
double math_sign_double(double n);
double math_copysign_double(double value, double sign);
double math_frexp_double(double n, int* exponent);
double math_remainder_double(double a, double b);
double math_mod_double(double a, double b);
double math_fma_double(double a, double b, double c);

double math_round_double(double n);
double math_floor_double(double n);
double math_ceil_double(double n);
double math_trunc_double(double n);

bool math_is_nan_double(double n);
bool math_is_infinite_double(double n);
bool math_is_finite_double(double n);
bool math_is_normal_double(double n);

double math_is_close_double(double a, double b, double tolerance);
double math_lerp_double(double start, double end, double amount);
double math_percentage_double(double value, double percentage);
double math_random_double(void);

double math_to_radians_double(double degrees);
double math_to_degrees_double(double radians);

/* ldouble */

ldouble math_pow_ldouble(ldouble base, ldouble exponent);
ldouble math_log_ldouble(ldouble n);
ldouble math_log2_ldouble(ldouble n);
ldouble math_log10_ldouble(ldouble n);
ldouble math_log1p_ldouble(ldouble n);

ldouble math_exp_ldouble(ldouble n);
ldouble math_exp2_ldouble(ldouble n);
ldouble math_expm1_ldouble(ldouble n);
ldouble math_sqrt_ldouble(ldouble n);
ldouble math_cbrt_ldouble(ldouble n);
ldouble math_hypot_ldouble(ldouble x, ldouble y);

ldouble math_sin_ldouble(ldouble n);
ldouble math_cos_ldouble(ldouble n);
ldouble math_tan_ldouble(ldouble n);

ldouble math_asin_ldouble(ldouble n);
ldouble math_acos_ldouble(ldouble n);
ldouble math_atan_ldouble(ldouble n);
ldouble math_atan2_ldouble(ldouble y, ldouble x);

ldouble math_sinh_ldouble(ldouble n);
ldouble math_cosh_ldouble(ldouble n);
ldouble math_tanh_ldouble(ldouble n);

ldouble math_asinh_ldouble(ldouble n);
ldouble math_acosh_ldouble(ldouble n);
ldouble math_atanh_ldouble(ldouble n);

ldouble math_abs_ldouble(ldouble n);
ldouble math_min_ldouble(ldouble a, ldouble b);
ldouble math_max_ldouble(ldouble a, ldouble b);
ldouble math_sign_ldouble(ldouble n);
ldouble math_copysign_ldouble(ldouble value, ldouble sign);
ldouble math_frexp_ldouble(ldouble n, int* exponent);
ldouble math_remainder_ldouble(ldouble a, ldouble b);
ldouble math_mod_ldouble(ldouble a, ldouble b);
ldouble math_fma_ldouble(ldouble a, ldouble b, ldouble c);

ldouble math_round_ldouble(ldouble n);
ldouble math_floor_ldouble(ldouble n);
ldouble math_ceil_ldouble(ldouble n);
ldouble math_trunc_ldouble(ldouble n);

bool math_is_nan_ldouble(ldouble n);
bool math_is_infinite_ldouble(ldouble n);
bool math_is_finite_ldouble(ldouble n);
bool math_is_normal_ldouble(ldouble n);

ldouble math_is_close_ldouble(ldouble a, ldouble b, ldouble tolerance);
ldouble math_lerp_ldouble(ldouble start, ldouble end, ldouble amount);
ldouble math_percentage_ldouble(ldouble value, ldouble percentage);
ldouble math_random_ldouble(void);

ldouble math_to_radians_ldouble(ldouble degrees);
ldouble math_to_degrees_ldouble(ldouble radians);

#endif
