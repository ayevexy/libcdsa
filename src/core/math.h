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
#define math_add_exact(a, b) _Generic((a) + (b), \
    int:     math_add_exact_int,                 \
    uint:    math_add_exact_uint,                \
    long:    math_add_exact_long,                \
    ulong:   math_add_exact_ulong,               \
    llong:   math_add_exact_llong,               \
    ullong:  math_add_exact_ullong               \
)(a, b)

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
#define math_sub_exact(a, b) _Generic((a) + (b), \
    int:     math_sub_exact_int,                 \
    uint:    math_sub_exact_uint,                \
    long:    math_sub_exact_long,                \
    ulong:   math_sub_exact_ulong,               \
    llong:   math_sub_exact_llong,               \
    ullong:  math_sub_exact_ullong               \
)(a, b)

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
#define math_mul_exact(a, b) _Generic((a) + (b), \
    int:     math_mul_exact_int,                 \
    uint:    math_mul_exact_uint,                \
    long:    math_mul_exact_long,                \
    ulong:   math_mul_exact_ulong,               \
    llong:   math_mul_exact_llong,               \
    ullong:  math_mul_exact_ullong               \
)(a, b)

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
#define math_div_exact(a, b) _Generic((a) + (b), \
    int:     math_div_exact_int,                 \
    uint:    math_div_exact_uint,                \
    long:    math_div_exact_long,                \
    ulong:   math_div_exact_ulong,               \
    llong:   math_div_exact_llong,               \
    ullong:  math_div_exact_ullong               \
)(a, b)

/**
 * @brief Raises a number to a given power.
 *
 * @param base the base value
 * @param exponent the exponent value
 *
 * @return the result of raising the base to the exponent
 */
#define math_pow(base, exponent) _Generic((base) + (exponent), \
    float:    math_pow_float,                                  \
    double:   math_pow_double,                                 \
    ldouble:  math_pow_ldouble,                                \
    default:  math_pow_double                                  \
)(base, exponent)

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

#define math_log_natural(n) _Generic((n), \
    float:    math_log_natural_float,     \
    double:   math_log_natural_double,    \
    ldouble:  math_log_natural_ldouble,   \
    default:  math_log_natural_double     \
)(n)

#define math_log_base(n, base) _Generic((n) + (base), \
    float:    math_log_base_float,                    \
    double:   math_log_base_double,                   \
    ldouble:  math_log_base_ldouble,                  \
    default:  math_log_base_double                    \
)(n, base)

/**
 * @brief Computes the base-2 logarithm of a number.
 *
 * @param n the input value
 *
 * @return the base-2 logarithm of n
 */
#define math_log2(n) _Generic((n), \
    float:    math_log2_float,     \
    double:   math_log2_double,    \
    ldouble:  math_log2_ldouble,   \
    default:  math_log2_double     \
)(n)

/**
 * @brief Computes the base-10 logarithm of a number.
 *
 * @param n the input value
 *
 * @return the base-10 logarithm of n
 */
#define math_log10(n) _Generic((n), \
    float:    math_log10_float,     \
    double:   math_log10_double,    \
    ldouble:  math_log10_ldouble,   \
    default:  math_log10_double     \
)(n)

/**
 * @brief Computes the natural logarithm of one plus a number.
 *
 * @param n the input value
 *
 * @return the natural logarithm of one plus n
 */
#define math_log1p(n) _Generic((n), \
    float:    math_log1p_float,     \
    double:   math_log1p_double,    \
    ldouble:  math_log1p_ldouble,   \
    default:  math_log1p_double     \
)(n)

/**
 * @brief Computes the exponential of a number.
 *
 * @param n the exponent
 *
 * @return e raised to the power of n
 */
#define math_exp(n) _Generic((n), \
    float:    math_exp_float,     \
    double:   math_exp_double,    \
    ldouble:  math_exp_ldouble,   \
    default:  math_exp_double     \
)(n)

/**
 * @brief Computes two raised to the power of a number.
 *
 * @param n the exponent
 *
 * @return two raised to the power of n
 */
#define math_exp2(n) _Generic((n), \
    float:    math_exp2_float,     \
    double:   math_exp2_double,    \
    ldouble:  math_exp2_ldouble,   \
    default:  math_exp2_double     \
)(n)

/**
 * @brief Computes the exponential of a number minus one.
 *
 * @param n the exponent
 *
 * @return e raised to the power of n, minus one
 */
#define math_expm1(n) _Generic((n), \
    float:    math_expm1_float,     \
    double:   math_expm1_double,    \
    ldouble:  math_expm1_ldouble,   \
    default:  math_expm1_double     \
)(n)

/**
 * @brief Computes the square root of a number.
 *
 * @param n the input value
 *
 * @return the square root of n
 */
#define math_sqrt(n) _Generic((n), \
    float:    math_sqrt_float,     \
    double:   math_sqrt_double,    \
    ldouble:  math_sqrt_ldouble,   \
    default:  math_sqrt_double     \
)(n)

/**
 * @brief Computes the cube root of a number.
 *
 * @param n the input value
 *
 * @return the cube root of n
 */
#define math_cbrt(n) _Generic((n), \
    float:    math_cbrt_float,     \
    double:   math_cbrt_double,    \
    ldouble:  math_cbrt_ldouble,   \
    default:  math_cbrt_double     \
)(n)

/**
 * @brief Computes the hypotenuse of a right triangle.
 *
 * @param x the first side
 * @param y the second side
 *
 * @return the length of the hypotenuse
 */
#define math_hypot(x, y) _Generic((x) + (y), \
    float:    math_hypot_float,              \
    double:   math_hypot_double,             \
    ldouble:  math_hypot_ldouble,            \
    default:  math_hypot_double              \
)(x, y)

/**
 * @brief Returns the sine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the sine of n
 */
#define math_sin(n) _Generic((n), \
    float:    math_sin_float,     \
    double:   math_sin_double,    \
    ldouble:  math_sin_ldouble,   \
    default:  math_sin_double     \
)(n)

/**
 * @brief Returns the cosine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the cosine of n
 */
#define math_cos(n) _Generic((n), \
    float:    math_cos_float,     \
    double:   math_cos_double,    \
    ldouble:  math_cos_ldouble,   \
    default:  math_cos_double     \
)(n)

/**
 * @brief Returns the tangent of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the tangent of n
 */
#define math_tan(n) _Generic((n), \
    float:    math_tan_float,     \
    double:   math_tan_double,    \
    ldouble:  math_tan_ldouble,   \
    default:  math_tan_double     \
)(n)

/**
 * @brief Returns the arc sine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc sine of n in radians
 */
#define math_asin(n) _Generic((n), \
    float:    math_asin_float,     \
    double:   math_asin_double,    \
    ldouble:  math_asin_ldouble,   \
    default:  math_asin_double     \
)(n)

/**
 * @brief Returns the arc cosine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc cosine of n in radians
 */
#define math_acos(n) _Generic((n), \
    float:    math_acos_float,     \
    double:   math_acos_double,    \
    ldouble:  math_acos_ldouble,   \
    default:  math_acos_double     \
)(n)

/**
 * @brief Returns the arc tangent of a value.
 *
 * @param n the value
 *
 * @return the arc tangent of n in radians
 */
#define math_atan(n) _Generic((n), \
    float:    math_atan_float,     \
    double:   math_atan_double,    \
    ldouble:  math_atan_ldouble,   \
    default:  math_atan_double     \
)(n)

/**
 * @brief Returns the arc tangent of y / x, using the signs of both arguments
 * to determine the quadrant.
 *
 * @param y the y-coordinate
 * @param x the x-coordinate
 *
 * @return the arc tangent of y / x in radians
 */
#define math_atan2(y, x) _Generic((y) + (x), \
    float:    math_atan2_float,              \
    double:   math_atan2_double,             \
    ldouble:  math_atan2_ldouble,            \
    default:  math_atan2_double              \
)(y, x)

/**
 * @brief Returns the hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic sine of n
 */
#define math_sinh(n) _Generic((n), \
    float:    math_sinh_float,     \
    double:   math_sinh_double,    \
    ldouble:  math_sinh_ldouble,   \
    default:  math_sinh_double     \
)(n)

/**
 * @brief Returns the hyperbolic cosine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic cosine of n
 */
#define math_cosh(n) _Generic((n), \
    float:    math_cosh_float,     \
    double:   math_cosh_double,    \
    ldouble:  math_cosh_ldouble,   \
    default:  math_cosh_double     \
)(n)

/**
 * @brief Returns the hyperbolic tangent of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic tangent of n
 */
#define math_tanh(n) _Generic((n), \
    float:    math_tanh_float,     \
    double:   math_tanh_double,    \
    ldouble:  math_tanh_ldouble,   \
    default:  math_tanh_double     \
)(n)

/**
 * @brief Returns the inverse hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the inverse hyperbolic sine of n
 */
#define math_asinh(n) _Generic((n), \
    float:    math_asinh_float,     \
    double:   math_asinh_double,    \
    ldouble:  math_asinh_ldouble,   \
    default:  math_asinh_double     \
)(n)

/**
 * @brief Returns the inverse hyperbolic cosine of a value.
 *
 * @param n the value, greater than or equal to 1
 *
 * @return the inverse hyperbolic cosine of n
 */
#define math_acosh(n) _Generic((n), \
    float:    math_acosh_float,     \
    double:   math_acosh_double,    \
    ldouble:  math_acosh_ldouble,   \
    default:  math_acosh_double     \
)(n)

/**
 * @brief Returns the inverse hyperbolic tangent of a value.
 *
 * @param n the value, in the range -1..1 (exclusive)
 *
 * @return the inverse hyperbolic tangent of n
 */
#define math_atanh(n) _Generic((n), \
    float:    math_atanh_float,     \
    double:   math_atanh_double,    \
    ldouble:  math_atanh_ldouble,   \
    default:  math_atanh_double     \
)(n)

/**
 * @brief Converts degrees to radians.
 *
 * @param degrees the value in degrees
 *
 * @return the value in radians
 */
#define math_to_radians(degrees) _Generic((degrees), \
    float:    math_to_radians_float,                 \
    double:   math_to_radians_double,                \
    ldouble:  math_to_radians_ldouble,               \
    default:  math_to_radians_double                 \
)(degrees)

/**
 * @brief Converts radians to degrees.
 *
 * @param radians the value in radians
 *
 * @return the value in degrees
 */
#define math_to_degrees(radians) _Generic((radians), \
    float:    math_to_degrees_float,                 \
    double:   math_to_degrees_double,                \
    ldouble:  math_to_degrees_ldouble,               \
    default:  math_to_degrees_double                 \
)(radians)

/**
 * @brief Calculates the arithmetic mean of an array of values.
 *
 * @param values the array of values
 * @param size the number of values
 *
 * @return the arithmetic mean
 */
#define math_mean(values, size) _Generic(*(values), \
    int:      math_mean_int,                        \
    uint:     math_mean_uint,                       \
    long:     math_mean_long,                       \
    ulong:    math_mean_ulong,                      \
    llong:    math_mean_llong,                      \
    ullong:   math_mean_ullong,                     \
    float:    math_mean_float,                      \
    double:   math_mean_double,                     \
    ldouble:  math_mean_ldouble                     \
)(values, size)

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
#define math_mean_of(T, ...) math_mean(((T[]){__VA_ARGS__}), count_args(__VA_ARGS__))

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
#define math_median(values, size) _Generic(*(values), \
    int:      math_median_int,                        \
    uint:     math_median_uint,                       \
    long:     math_median_long,                       \
    ulong:    math_median_ulong,                      \
    llong:    math_median_llong,                      \
    ullong:   math_median_ullong,                     \
    float:    math_median_float,                      \
    double:   math_median_double,                     \
    ldouble:  math_median_ldouble                     \
)(values, size)

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
#define math_median_of(T, ...) math_median(((T[]){__VA_ARGS__}), count_args(__VA_ARGS__))

/**
 * @brief Returns the greatest common divisor (GCD) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the greatest common divisor
 */
#define math_gcd(a, b) _Generic((a) + (b), \
    int:     math_gcd_int,                 \
    uint:    math_gcd_uint,                \
    long:    math_gcd_long,                \
    ulong:   math_gcd_ulong,               \
    llong:   math_gcd_llong,               \
    ullong:  math_gcd_ullong               \
)(a, b)

/**
 * @brief Returns the least common multiple (LCM) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the least common multiple
 */
#define math_lcm(a, b) _Generic((a) + (b), \
    int:     math_lcm_int,                 \
    uint:    math_lcm_uint,                \
    long:    math_lcm_long,                \
    ulong:   math_lcm_ulong,               \
    llong:   math_lcm_llong,               \
    ullong:  math_lcm_ullong               \
)(a, b)

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
#define math_factorial(n) _Generic((n), \
    int:     math_factorial_int,        \
    uint:    math_factorial_uint,       \
    long:    math_factorial_long,       \
    ulong:   math_factorial_ulong,      \
    llong:   math_factorial_llong,      \
    ullong:  math_factorial_ullong      \
)(n)

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
#define math_combinations(n, r) _Generic((n) + (r), \
    int:     math_combinations_int,                 \
    uint:    math_combinations_uint,                \
    long:    math_combinations_long,                \
    ulong:   math_combinations_ulong,               \
    llong:   math_combinations_llong,               \
    ullong:  math_combinations_ullong               \
)(n, r)

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
#define math_permutations(n, r) _Generic((n) + (r), \
    int:     math_permutations_int,                 \
    uint:    math_permutations_uint,                \
    long:    math_permutations_long,                \
    ulong:   math_permutations_ulong,               \
    llong:   math_permutations_llong,               \
    ullong:  math_permutations_ullong               \
)(n, r)

/**
 * @brief Checks whether a number is even.
 *
 * @param n the number
 *
 * @return true if even, false otherwise
 */
#define math_is_even(n) ((n) % 2 == 0)

/**
 * @brief Checks whether a number is odd.
 *
 * @param n the number
 *
 * @return true if odd, false otherwise
 */
#define math_is_odd(n) ((n) % 2 != 0)

/**
 * @brief Checks whether a number is prime.
 *
 * @param n the number
 *
 * @return true if prime, false otherwise
 */
#define math_is_prime(n) _Generic((n), \
    int:     math_is_prime_int,        \
    uint:    math_is_prime_uint,       \
    long:    math_is_prime_long,       \
    ulong:   math_is_prime_ulong,      \
    llong:   math_is_prime_llong,      \
    ullong:  math_is_prime_ullong      \
)(n)

/**
 * @brief Checks whether a number is power of two.
 *
 * @param n the number
 *
 * @return true if n is power of two, false otherwise
 */
#define math_is_power_of_two(n) _Generic((n), \
    int:     math_is_power_of_two_int,        \
    uint:    math_is_power_of_two_uint,       \
    long:    math_is_power_of_two_long,       \
    ulong:   math_is_power_of_two_ulong,      \
    llong:   math_is_power_of_two_llong,      \
    ullong:  math_is_power_of_two_ullong      \
)(n)

/**
 * @brief Returns the absolute value of a number.
 *
 * @param n the number
 *
 * @return the absolute value of n
 */
#define math_abs(n) _Generic((n), \
    int:      math_abs_int,       \
    uint:     math_abs_uint,      \
    long:     math_abs_long,      \
    ulong:    math_abs_ulong,     \
    llong:    math_abs_llong,     \
    ullong:   math_abs_ullong,    \
    float:    math_abs_float,     \
    double:   math_abs_double,    \
    ldouble:  math_abs_ldouble    \
)(n)

/**
 * @brief Returns the minimum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the minimum number
 */
#define math_min(a, b) _Generic((a) + (b), \
    int:      math_min_int,                \
    uint:     math_min_uint,               \
    long:     math_min_long,               \
    ulong:    math_min_ulong,              \
    llong:    math_min_llong,              \
    ullong:   math_min_ullong,             \
    float:    math_min_float,              \
    double:   math_min_double,             \
    ldouble:  math_min_ldouble             \
)(a, b)

/**
 * @brief Returns the maximum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the maximum number
 */
#define math_max(a, b) _Generic((a) + (b), \
    int:      math_max_int,                \
    uint:     math_max_uint,               \
    long:     math_max_long,               \
    ulong:    math_max_ulong,              \
    llong:    math_max_llong,              \
    ullong:   math_max_ullong,             \
    float:    math_max_float,              \
    double:   math_max_double,             \
    ldouble:  math_max_ldouble             \
)(a, b)

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
#define math_sign(n) _Generic((n), \
    float:    math_sign_float,     \
    double:   math_sign_double,    \
    ldouble:  math_sign_ldouble,   \
    default:  math_sign_double     \
)(n)

/**
 * @brief Returns a value with the magnitude of one floating-point number and the sign of another.
 *
 * @param value the value whose magnitude is used
 * @param sign the value whose sign is copied
 *
 * @return the value with the magnitude of value and the sign of sign
 */
#define math_copysign(value, sign) _Generic((value) + (sign), \
    float:    math_copysign_float,                            \
    double:   math_copysign_double,                           \
    ldouble:  math_copysign_ldouble,                          \
    default:  math_copysign_double                            \
)(value, sign)

/**
 * @brief Scales a floating-point number by a power of two, computing `n * 2^exponent`.
 *
 * @param n the number to scale
 * @param exponent the exponent of the power of two
 *
 * @return the result of scaling `n` by `2^exponent`
 */
#define math_scalb(n, exponent) _Generic((n),                                             \
    float:    _Generic((exponent), int: math_scalbn_float,   long: math_scalbln_float),   \
    double:   _Generic((exponent), int: math_scalbn_double,  long: math_scalbln_double),  \
    ldouble:  _Generic((exponent), int: math_scalbn_ldouble, long: math_scalbln_ldouble), \
    default:  _Generic((exponent), int: math_scalbn_double,  long: math_scalbln_double)   \
)(n, exponent)

/**
 * @brief Decomposes a floating-point number into a normalized fraction and an integer exponent.
 *
 * @param n the number to decompose
 * @param exponent pointer to an integer where the exponent is stored
 *
 * @return the normalized fraction
 */
#define math_frexp(n, exponent) _Generic((n), \
    float:    math_frexp_float,               \
    double:   math_frexp_double,              \
    ldouble:  math_frexp_ldouble,             \
    default:  math_frexp_double               \
)(n, exponent)

/**
 * @brief Scales a floating-point number by a power of two, computing `n * 2^exponent`.
 *
 * @param n the number to scale
 * @param exponent the integer exponent of the power of two
 *
 * @return the result of scaling `n` by `2^exponent`
 */
#define math_ldexp(n, exponent) _Generic((n), \
    float:    math_ldexp_float,               \
    double:   math_ldexp_double,              \
    ldouble:  math_ldexp_ldouble,             \
    default:  math_ldexp_double               \
)(n, exponent)

/**
 * @brief Returns the next representable floating-point number after `from` in the direction of `to`.
 *
 * @param from the starting floating-point value
 * @param to the number toward which to advance
 *
 * @return the next representable floating-point number after `from` in the direction of `to`
 */
#define math_next_after(from, to) _Generic((from) + (to), \
    float:    math_next_after_float,                      \
    double:   math_next_after_double,                     \
    ldouble:  math_next_after_ldouble,                    \
    default:  math_next_after_double                      \
)(from, to)

/**
 * @brief Returns the next representable floating-point number after `from` in the direction of `to`.
 *
 * @param from the starting floating-point value
 * @param to the number toward which to advance
 *
 * @return the next representable floating-point number after `from` in the direction of `to`
 */
#define math_next_toward(from, to) _Generic((from) + (to), \
    float:    math_next_toward_float,                      \
    double:   math_next_toward_double,                     \
    ldouble:  math_next_toward_ldouble,                    \
    default:  math_next_toward_double                      \
)(from, to)

/**
 * @brief Calculates the IEEE floating-point remainder of two numbers.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the remainder of a divided by b
 */
#define math_remainder(a, b) _Generic((a) + (b), \
    float:    math_remainder_float,              \
    double:   math_remainder_double,             \
    ldouble:  math_remainder_ldouble,            \
    default:  math_remainder_double              \
)(a, b)

/**
 * @brief Computes the floating-point remainder of a division.
 *
 * @param a the dividend
 * @param b the divisor
 *
 * @return the remainder of dividing a by b
 */
#define math_mod(a, b) _Generic((a) + (b), \
    float:    math_mod_float,              \
    double:   math_mod_double,             \
    ldouble:  math_mod_ldouble,            \
    default:  math_mod_double              \
)(a, b)

/**
 * @brief Computes the positive difference between two floating-point numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return `a - b` if `a > b`; otherwise, `+0.0`
 */
#define math_fdim(a, b) _Generic((a) + (b), \
    float:    math_fdim_float,              \
    double:   math_fdim_double,             \
    ldouble:  math_fdim_ldouble,            \
    default:  math_fdim_double              \
)(a, b)

/**
 * @brief Computes the fused multiply-add operation: a * b + c.
 *
 * @param a the first multiplicand
 * @param b the second multiplicand
 * @param c the addend
 *
 * @return the result of a * b + c, computed with a single rounding
 */
#define math_fma(a, b, c) _Generic((a) + (b) + (c), \
    float:    math_fma_float,                       \
    double:   math_fma_double,                      \
    ldouble:  math_fma_ldouble,                     \
    default:  math_fma_double                       \
)(a, b, c)

/**
 * @brief Rounds a floating-point number to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_round(n) _Generic((n), \
    float:    math_round_float,     \
    double:   math_round_double,    \
    ldouble:  math_round_ldouble,   \
    default:  math_round_double     \
)(n)

/**
 * @brief Rounds a floating-point number down to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_floor(n) _Generic((n), \
    float:    math_floor_float,     \
    double:   math_floor_double,    \
    ldouble:  math_floor_ldouble,   \
    default:  math_floor_double     \
)(n)

/**
 * @brief Rounds a floating-point number up to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_ceil(n) _Generic((n), \
    float:    math_ceil_float,     \
    double:   math_ceil_double,    \
    ldouble:  math_ceil_ldouble,   \
    default:  math_ceil_double     \
)(n)

/**
 * @brief Truncates the fractional part of a floating-point number.
 *
 * @param n the number
 *
 * @return the truncated number
 */
#define math_trunc(n) _Generic((n), \
    float:    math_trunc_float,     \
    double:   math_trunc_double,    \
    ldouble:  math_trunc_ldouble,   \
    default:  math_trunc_double     \
)(n)

/**
 * @brief Checks whether a floating-point number is NaN.
 *
 * @param n the number
 *
 * @return true if n is NaN, false otherwise
 */
#define math_is_nan(n) _Generic((n), \
    float:    math_is_nan_float,     \
    double:   math_is_nan_double,    \
    ldouble:  math_is_nan_ldouble,   \
    default:  math_is_nan_double     \
)(n)

/**
 * @brief Checks whether a floating-point number is infinite.
 *
 * @param n the number
 *
 * @return true if n is infinite, false otherwise
 */
#define math_is_infinite(n) _Generic((n), \
    float:    math_is_infinite_float,     \
    double:   math_is_infinite_double,    \
    ldouble:  math_is_infinite_ldouble,   \
    default:  math_is_infinite_double     \
)(n)

/**
 * @brief Checks whether a floating-point number is finite.
 *
 * @param n the number
 *
 * @return true if n is finite, false otherwise
 */
#define math_is_finite(n) _Generic((n), \
    float:    math_is_finite_float,     \
    double:   math_is_finite_double,    \
    ldouble:  math_is_finite_ldouble,   \
    default:  math_is_finite_double     \
)(n)

/**
 * @brief Checks whether a floating-point number is normal.
 *
 * @param n the number
 *
 * @return true if n is normal, false otherwise
 */
#define math_is_normal(n) _Generic((n), \
    float:    math_is_normal_float,     \
    double:   math_is_normal_double,    \
    ldouble:  math_is_normal_ldouble,   \
    default:  math_is_normal_double     \
)(n)

/**
 * @brief Checks whether two floating-point numbers are approximately equal.
 *
 * @param a the first number
 * @param b the second number
 * @param tolerance the maximum allowed absolute difference
 *
 * @return true if the absolute difference between a and b is within tolerance, false otherwise
 */
#define math_is_close(a, b, tolerance) _Generic((a) + (b) + (tolerance), \
    float:    math_is_close_float,                                       \
    double:   math_is_close_double,                                      \
    ldouble:  math_is_close_ldouble,                                     \
    default:  math_is_close_double                                       \
)(a, b, tolerance)

/**
 * @brief Linearly interpolates between two floating-point numbers.
 *
 * @param start the starting value
 * @param end the ending value
 * @param amount the interpolation amount (0.0 to 1.0)
 *
 * @return the interpolated value
 */
#define math_lerp(start, end, amount) _Generic((start) + (end) + (amount), \
    float:    math_lerp_float,                                             \
    double:   math_lerp_double,                                            \
    ldouble:  math_lerp_ldouble,                                           \
    default:  math_lerp_double                                             \
)(start, end, amount)

/**
 * @brief Calculates a percentage of a floating-point number.
 *
 * @param value the value
 * @param percentage_ the percentage to calculate
 *
 * @return the calculated percentage of the value
 */
#define math_percentage(value, percentage_) _Generic((value) + (percentage_), \
    float:    math_percentage_float,                                          \
    double:   math_percentage_double,                                         \
    ldouble:  math_percentage_ldouble,                                        \
    default:  math_percentage_double                                         \
)(value, percentage_)

/**
 * @brief Generates a pseudo-random floating-point number.
 *
 * @param T the floating-point type of the generated number
 * @param state optional pointer to the PRNG state (uint64*)
 *
 * @return a pseudo-random number in the range [0.0, 1.0)
 *
 * @note If state is nullptr, an internal thread-local state is used.
 * @note The provided state is updated during generation.
 * @note The same initial state produces the same sequence of values.
 * @note Uses the SplitMix64 pseudo-random number generator (PRNG).
 * @note This function is not suitable for cryptographic purposes.
 */
#define math_random(T, ...) math_random_(T, __VA_OPT__(__VA_ARGS__,) nullptr, __VA_ARGS__)

#define math_random_(T, state, ...) _Generic((T) 0, \
    float:    math_random_float,                    \
    double:   math_random_double,                   \
    ldouble:  math_random_ldouble,                  \
    default:  math_random_double                    \
)(state)

/*-----------------------------------------------------------------------------------------*/

#define DECLARE_INTEGER_MATH(T, suffix)                      \
    T math_add_exact_##suffix(T a, T b);                     \
    T math_sub_exact_##suffix(T a, T b);                     \
    T math_mul_exact_##suffix(T a, T b);                     \
    T math_div_exact_##suffix(T a, T b);                     \
    double math_mean_##suffix(const T* values, uint size);   \
    double math_median_##suffix(const T* values, uint size); \
    T math_gcd_##suffix(T a, T b);                           \
    T math_lcm_##suffix(T a, T b);                           \
    T math_factorial_##suffix(T n);                          \
    T math_combinations_##suffix(T n, T r);                  \
    T math_permutations_##suffix(T n, T r);                  \
    bool math_is_prime_##suffix(T n);                        \
    bool math_is_power_of_two_##suffix(T n);                 \
    T math_abs_##suffix(T n);                                \
    T math_min_##suffix(T a, T b);                           \
    T math_max_##suffix(T a, T b);

#define DECLARE_FLOAT_MATH(T, suffix)                        \
    T math_pow_##suffix(T base, T exponent);                 \
    T math_log_natural_##suffix(T n);                        \
    T math_log_base_##suffix(T n, T base);                   \
    T math_log2_##suffix(T n);                               \
    T math_log10_##suffix(T n);                              \
    T math_log1p_##suffix(T n);                              \
    T math_exp_##suffix(T n);                                \
    T math_exp2_##suffix(T n);                               \
    T math_expm1_##suffix(T n);                              \
    T math_sqrt_##suffix(T n);                               \
    T math_cbrt_##suffix(T n);                               \
    T math_hypot_##suffix(T x, T y);                         \
    T math_sin_##suffix(T n);                                \
    T math_cos_##suffix(T n);                                \
    T math_tan_##suffix(T n);                                \
    T math_asin_##suffix(T n);                               \
    T math_acos_##suffix(T n);                               \
    T math_atan_##suffix(T n);                               \
    T math_atan2_##suffix(T y, T x);                         \
    T math_sinh_##suffix(T n);                               \
    T math_cosh_##suffix(T n);                               \
    T math_tanh_##suffix(T n);                               \
    T math_asinh_##suffix(T n);                              \
    T math_acosh_##suffix(T n);                              \
    T math_atanh_##suffix(T n);                              \
    T math_to_radians_##suffix(T degrees);                   \
    T math_to_degrees_##suffix(T radians);                   \
    double math_mean_##suffix(const T* values, uint size);   \
    double math_median_##suffix(const T* values, uint size); \
    T math_abs_##suffix(T n);                                \
    T math_min_##suffix(T a, T b);                           \
    T math_max_##suffix(T a, T b);                           \
    T math_sign_##suffix(T n);                               \
    T math_copysign_##suffix(T value, T sign);               \
    T math_scalbn_##suffix(T n, int exponent);               \
    T math_scalbln_##suffix(T n, long exponent);             \
    T math_frexp_##suffix(T n, int* exponent);               \
    T math_ldexp_##suffix(T n, int exponent);                \
    T math_next_after_##suffix(T from, T to);                \
    T math_next_toward_##suffix(T from, ldouble to);         \
    T math_remainder_##suffix(T a, T b);                     \
    T math_mod_##suffix(T a, T b);                           \
    T math_fdim_##suffix(T a, T b);                          \
    T math_fma_##suffix(T a, T b, T c);                      \
    T math_round_##suffix(T n);                              \
    T math_floor_##suffix(T n);                              \
    T math_ceil_##suffix(T n);                               \
    T math_trunc_##suffix(T n);                              \
    bool math_is_nan_##suffix(T n);                          \
    bool math_is_infinite_##suffix(T n);                     \
    bool math_is_finite_##suffix(T n);                       \
    bool math_is_normal_##suffix(T n);                       \
    bool math_is_close_##suffix(T a, T b, T tolerance);      \
    T math_lerp_##suffix(T start, T end, T amount);          \
    T math_percentage_##suffix(T value, T percentage);       \
    T math_random_##suffix(uint64* state);

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
