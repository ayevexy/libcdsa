#ifndef LIBCDSA_MATH_H
#define LIBCDSA_MATH_H

/**
 * @file math.h
 * @brief Math utilities.
 */

#include "core/types.h"
#include "core/macros.h"

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
 * @brief Computes the natural or base-specific logarithm of a number.
 *
 * @param n the input value
 * @param base the logarithm base (optional)
 *
 * @return the logarithm of `n` to the specified base, or base e by default
 */
#define math_log(...) dispatch_math_log(__VA_ARGS__, math_log_base, math_log_natural)(__VA_ARGS__)

#define dispatch_math_log(_1, _2, NAME, ...) NAME

#define math_log_natural(n) MATH_DISPATCH_FLOATS_SINGLE_ARG(log_natural, n)

#define math_log_base(n, base) MATH_DISPATCH_FLOATS_TWO_ARGS(log_base, n, base)

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

/**
 * @brief Calculates the arithmetic mean of an array of values.
 *
 * @param values the array of values
 * @param size the number of values
 *
 * @return the arithmetic mean
 */
#define math_mean(values, size) MATH_DISPATCH_NUMBERS_POINTER(mean, values, size)

/**
 * @brief Calculates the arithmetic mean of variadic values of a specified type.
 *
 * @param T the type of values
 * @param ... the values to calculate the arithmetic mean of
 *
 * @return the arithmetic mean
 *
 * @note All values must be compatible with the specified type.
 */
#define math_mean_of(T, ...) MATH_DISPATCH_NUMBERS_POINTER(mean, ((T[]) {__VA_ARGS__}), count_args(__VA_ARGS__))

/**
 * @brief Calculates the median of a sorted array of values.
 *
 * @param values the array of values sorted in ascending order
 * @param size the number of elements
 *
 * @return the median value, or NAN if the array is empty
 *
 * @note The input array must be sorted in ascending order.
 * @note The input array is not modified.
 */
#define math_median(values, size) MATH_DISPATCH_NUMBERS_POINTER(median, values, size)

/**
 * @brief Calculates the median of a sorted sequence of values.
 *
 * @param T the type of the values
 * @param ... the values sorted in ascending order
 *
 * @return the median value, or NAN if no values are provided
 *
 * @note The values must be provided in ascending order.
 */
#define math_median_of(T, ...) MATH_DISPATCH_NUMBERS_POINTER(median, ((T[]) {__VA_ARGS__}), count_args(__VA_ARGS__))

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
 * @brief Calculates the number of ways to select r elements from n elements.
 *
 * @param n the total number of elements
 * @param r the number of elements to select
 *
 * @return the number of possible combinations
 *
 * @note The order of selected elements does not matter.
 */
#define math_combinations(n, r) MATH_DISPATCH_INTEGERS_TWO_ARGS(combinations, n, r)

/**
 * @brief Calculates the number of ways to arrange r elements selected from n elements.
 *
 * @param n the total number of elements
 * @param r the number of elements to arrange
 *
 * @return the number of possible permutations
 *
 * @note The order of selected elements matters.
 */
#define math_permutations(n, r) MATH_DISPATCH_INTEGERS_TWO_ARGS(permutations, n, r)

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
 * @brief Scales a floating-point number by a power of two, computing `n * 2^exponent`.
 *
 * @param n the number to scale
 * @param exponent the exponent of the power of two
 *
 * @return the result of scaling `n` by `2^exponent`
 */
#define math_scalb(n, exponent) _Generic((exponent),                                                            \
    int:  _Generic((n), float: math_scalbn_float, double: math_scalbn_double, ldouble: math_scalbn_ldouble),    \
    long: _Generic((n), float: math_scalbln_float, double: math_scalbln_double, ldouble: math_scalbln_ldouble)  \
)(n, exponent)

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
 * @brief Scales a floating-point number by a power of two, computing `n * 2^exponent`.
 *
 * @param n the number to scale
 * @param exponent the integer exponent of the power of two
 *
 * @return the result of scaling `n` by `2^exponent`
 */
#define math_ldexp(n, exponent) _Generic((n),   \
    float:    math_ldexp_float,                 \
    double:   math_ldexp_double,                \
    ldouble:  math_ldexp_ldouble                \
)(n, exponent)

/**
 * @brief Returns the next representable floating-point number after `from` in the direction of `to`.
 *
 * @param from the starting floating-point value
 * @param to the number toward which to advance
 *
 * @return the next representable floating-point number after `from` in the direction of `to`
 */
#define math_next_after(from, to) MATH_DISPATCH_FLOATS_TWO_ARGS(next_after, from, to)

/**
 * @brief Returns the next representable floating-point number after `from` in the direction of `to`.
 *
 * @param from the starting floating-point value
 * @param to the number toward which to advance
 *
 * @return the next representable floating-point number after `from` in the direction of `to`
 */
#define math_next_toward(from, to) _Generic((from),     \
    float:    math_next_toward_float,                   \
    double:   math_next_toward_double,                  \
    ldouble:  math_next_toward_ldouble                  \
)(from, to)

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
 * @brief Computes the positive difference between two floating-point numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return `a - b` if `a > b`; otherwise, `+0.0`
 */
#define math_fdim(a, b) MATH_DISPATCH_FLOATS_TWO_ARGS(fdim, a, b)

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
 * @param state the optional state used to generate the sequence (an uint64*)
 *
 * @return a pseudo-random number in the range [0.0, 1.0)
 *
 * @note If state is nullptr, an internal thread-local state is used.
 * @note This function is not suitable for cryptographic purposes.
 */
#define math_random(T, ...) math_random_(T, __VA_OPT__(__VA_ARGS__,) nullptr, __VA_ARGS__)

#define math_random_(T, state, ...) _Generic((T) 0,     \
    float:    math_random_float,                        \
    double:   math_random_double,                       \
    ldouble:  math_random_ldouble                       \
)(state)

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

/* Array pointers */

#define MATH_INTEGER_POINTER_TYPES(operation) \
    char*:    math_##operation##_char,        \
    schar*:   math_##operation##_schar,       \
    uchar*:   math_##operation##_uchar,       \
    short*:   math_##operation##_short,       \
    ushort*:  math_##operation##_ushort,      \
    int*:     math_##operation##_int,         \
    uint*:    math_##operation##_uint,        \
    long*:    math_##operation##_long,        \
    ulong*:   math_##operation##_ulong,       \
    llong*:   math_##operation##_llong,       \
    ullong*:  math_##operation##_ullong

#define MATH_FLOAT_POINTER_TYPES(operation)     \
    float*:   math_##operation##_float,         \
    double*:  math_##operation##_double,        \
    ldouble*: math_##operation##_ldouble

#define MATH_NUMBER_POINTER_TYPES(operation)    \
    MATH_INTEGER_POINTER_TYPES(operation),      \
    MATH_FLOAT_POINTER_TYPES(operation)

#define MATH_INTEGER_CONST_POINTER_TYPES(operation) \
    const char*:    math_##operation##_char,        \
    const schar*:   math_##operation##_schar,       \
    const uchar*:   math_##operation##_uchar,       \
    const short*:   math_##operation##_short,       \
    const ushort*:  math_##operation##_ushort,      \
    const int*:     math_##operation##_int,         \
    const uint*:    math_##operation##_uint,        \
    const long*:    math_##operation##_long,        \
    const ulong*:   math_##operation##_ulong,       \
    const llong*:   math_##operation##_llong,       \
    const ullong*:  math_##operation##_ullong

#define MATH_FLOAT_CONST_POINTER_TYPES(operation)   \
    const float*:   math_##operation##_float,       \
    const double*:  math_##operation##_double,      \
    const ldouble*: math_##operation##_ldouble

#define MATH_NUMBER_CONST_POINTER_TYPES(operation)      \
    MATH_INTEGER_CONST_POINTER_TYPES(operation),        \
    MATH_FLOAT_CONST_POINTER_TYPES(operation)

#define MATH_DISPATCH_NUMBERS_POINTER(operation, values, size)      \
    _Generic((values),                                              \
        MATH_NUMBER_POINTER_TYPES(operation),                       \
        MATH_NUMBER_CONST_POINTER_TYPES(operation)                  \
    )(values, size)

/* Declarations */

#define DECLARE_INTEGER_MATH(T, suffix)                     \
    T math_add_exact_##suffix(T a, T b);                    \
    T math_sub_exact_##suffix(T a, T b);                    \
    T math_mul_exact_##suffix(T a, T b);                    \
    T math_div_exact_##suffix(T a, T b);                    \
    double math_mean_##suffix(T* values, uint size);        \
    double math_median_##suffix(T* values, uint size);      \
    T math_gcd_##suffix(T a, T b);                          \
    T math_lcm_##suffix(T a, T b);                          \
    T math_factorial_##suffix(T n);                         \
    T math_combinations_##suffix(T n, T r);                 \
    T math_permutations_##suffix(T n, T r);                 \
    bool math_is_prime_##suffix(T n);                       \
    bool math_is_power_of_two_##suffix(T n);                \
    T math_abs_##suffix(T n);                               \
    T math_min_##suffix(T a, T b);                          \
    T math_max_##suffix(T a, T b);

#define DECLARE_FLOAT_MATH(T, suffix)                       \
    T math_pow_##suffix(T base, T exponent);                \
    T math_log_natural_##suffix(T n);                       \
    T math_log_base_##suffix(T n, T base);                  \
    T math_log2_##suffix(T n);                              \
    T math_log10_##suffix(T n);                             \
    T math_log1p_##suffix(T n);                             \
    T math_exp_##suffix(T n);                               \
    T math_exp2_##suffix(T n);                              \
    T math_expm1_##suffix(T n);                             \
    T math_sqrt_##suffix(T n);                              \
    T math_cbrt_##suffix(T n);                              \
    T math_hypot_##suffix(T x, T y);                        \
    T math_sin_##suffix(T n);                               \
    T math_cos_##suffix(T n);                               \
    T math_tan_##suffix(T n);                               \
    T math_asin_##suffix(T n);                              \
    T math_acos_##suffix(T n);                              \
    T math_atan_##suffix(T n);                              \
    T math_atan2_##suffix(T y, T x);                        \
    T math_sinh_##suffix(T n);                              \
    T math_cosh_##suffix(T n);                              \
    T math_tanh_##suffix(T n);                              \
    T math_asinh_##suffix(T n);                             \
    T math_acosh_##suffix(T n);                             \
    T math_atanh_##suffix(T n);                             \
    T math_to_radians_##suffix(T degrees);                  \
    T math_to_degrees_##suffix(T radians);                  \
    double math_mean_##suffix(T* values, uint size);        \
    double math_median_##suffix(T* values, uint size);      \
    T math_abs_##suffix(T n);                               \
    T math_min_##suffix(T a, T b);                          \
    T math_max_##suffix(T a, T b);                          \
    T math_sign_##suffix(T n);                              \
    T math_copysign_##suffix(T value, T sign);              \
    T math_scalbn_##suffix(T n, int exponent);              \
    T math_scalbln_##suffix(T n, long exponent);            \
    T math_frexp_##suffix(T n, int* exponent);              \
    T math_ldexp_##suffix(T n, int exponent);               \
    T math_next_after_##suffix(T from, T to);               \
    T math_next_toward_##suffix(T from, ldouble to);        \
    T math_remainder_##suffix(T a, T b);                    \
    T math_mod_##suffix(T a, T b);                          \
    T math_fdim_##suffix(T a, T b);                         \
    T math_fma_##suffix(T a, T b, T c);                     \
    T math_round_##suffix(T n);                             \
    T math_floor_##suffix(T n);                             \
    T math_ceil_##suffix(T n);                              \
    T math_trunc_##suffix(T n);                             \
    bool math_is_nan_##suffix(T n);                         \
    bool math_is_infinite_##suffix(T n);                    \
    bool math_is_finite_##suffix(T n);                      \
    bool math_is_normal_##suffix(T n);                      \
    bool math_is_close_##suffix(T a, T b, T tolerance);     \
    T math_lerp_##suffix(T start, T end, T amount);         \
    T math_percentage_##suffix(T value, T percentage);      \
    T math_random_##suffix(uint64* state);

DECLARE_INTEGER_MATH(char, char)
DECLARE_INTEGER_MATH(schar, schar)
DECLARE_INTEGER_MATH(uchar, uchar)

DECLARE_INTEGER_MATH(short, short)
DECLARE_INTEGER_MATH(ushort, ushort)

DECLARE_INTEGER_MATH(int, int)
DECLARE_INTEGER_MATH(uint, uint)

DECLARE_INTEGER_MATH(long, long)
DECLARE_INTEGER_MATH(ulong, ulong)

DECLARE_INTEGER_MATH(llong, llong)
DECLARE_INTEGER_MATH(ullong, ullong)

DECLARE_FLOAT_MATH(float, float)
DECLARE_FLOAT_MATH(double, double)
DECLARE_FLOAT_MATH(ldouble, ldouble)

#endif
