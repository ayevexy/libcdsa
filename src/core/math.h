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
#define math_add(a, b) MATH_DISPATCH_INTEGERS(add, a, b)

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
#define math_sub(a, b) MATH_DISPATCH_INTEGERS(sub, a, b)

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
#define math_mul(a, b) MATH_DISPATCH_INTEGERS(mul, a, b)

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
#define math_div(a, b) MATH_DISPATCH_INTEGERS(div, a, b)

/**
 * @brief Raises a number to a given power.
 *
 * @param base the base value
 * @param exponent the exponent value
 *
 * @return the result of raising the base to the exponent
 */
#define math_pow(base, exponent) MATH_DISPATCH_FLOATS(pow, base, exponent)

/**
 * @brief Computes the natural logarithm of a number.
 *
 * @param number the input value
 *
 * @return the natural logarithm of the number
 */
#define math_log(number) MATH_DISPATCH_FLOATS(log, number)

/**
 * @brief Computes the base-2 logarithm of a number.
 *
 * @param number the input value
 *
 * @return the base-2 logarithm of the number
 */
#define math_log2(number) MATH_DISPATCH_FLOATS(log2, number)

/**
 * @brief Computes the base-10 logarithm of a number.
 *
 * @param number the input value
 *
 * @return the base-10 logarithm of the number
 */
#define math_log10(number) MATH_DISPATCH_FLOATS(log10, number)

/**
 * @brief Computes the natural logarithm of one plus a number.
 *
 * @param number the input value
 *
 * @return the natural logarithm of one plus the number
 */
#define math_log1p(number) MATH_DISPATCH_FLOATS(log1p, number)

/**
 * @brief Computes the exponential of a number.
 *
 * @param number the exponent
 *
 * @return e raised to the power of the number
 */
#define math_exp(number) MATH_DISPATCH_FLOATS(exp, number)

/**
 * @brief Computes two raised to the power of a number.
 *
 * @param number the exponent
 *
 * @return two raised to the power of the number
 */
#define math_exp2(number) MATH_DISPATCH_FLOATS(exp2, number)

/**
 * @brief Computes the exponential of a number minus one.
 *
 * @param number the exponent
 *
 * @return e raised to the power of the number, minus one
 */
#define math_expm1(number) MATH_DISPATCH_FLOATS(expm1, number)

/**
 * @brief Computes the square root of a number.
 *
 * @param number the input value
 *
 * @return the square root of the number
 */
#define math_sqrt(number) MATH_DISPATCH_FLOATS(sqrt, number)

/**
 * @brief Computes the cube root of a number.
 *
 * @param number the input value
 *
 * @return the cube root of the number
 */
#define math_cbrt(number) MATH_DISPATCH_FLOATS(cbrt, number)

/**
 * @brief Computes the hypotenuse of a right triangle.
 *
 * @param x the first side
 * @param y the second side
 *
 * @return the length of the hypotenuse
 */
#define math_hypot(x, y) MATH_DISPATCH_FLOATS(hypot, x, y)

/**
 * @brief Returns the sine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the sine of n
 */
#define math_sin(n) MATH_DISPATCH_FLOATS(sin, n)

/**
 * @brief Returns the cosine of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the cosine of n
 */
#define math_cos(n) MATH_DISPATCH_FLOATS(cos, n)

/**
 * @brief Returns the tangent of an angle in radians.
 *
 * @param n the angle in radians
 *
 * @return the tangent of n
 */
#define math_tan(n) MATH_DISPATCH_FLOATS(tan, n)

/**
 * @brief Returns the arc sine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc sine of n in radians
 */
#define math_asin(n) MATH_DISPATCH_FLOATS(asin, n)

/**
 * @brief Returns the arc cosine of a value.
 *
 * @param n the value, in the range -1..1
 *
 * @return the arc cosine of n in radians
 */
#define math_acos(n) MATH_DISPATCH_FLOATS(acos, n)

/**
 * @brief Returns the arc tangent of a value.
 *
 * @param n the value
 *
 * @return the arc tangent of n in radians
 */
#define math_atan(n) MATH_DISPATCH_FLOATS(atan, n)

/**
 * @brief Returns the arc tangent of y / x, using the signs of both arguments
 * to determine the quadrant.
 *
 * @param y the y-coordinate
 * @param x the x-coordinate
 *
 * @return the arc tangent of y / x in radians
 */
#define math_atan2(y, x) MATH_DISPATCH_FLOATS(atan2, y, x)

/**
 * @brief Returns the hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic sine of n
 */
#define math_sinh(n) MATH_DISPATCH_FLOATS(sinh, n)

/**
 * @brief Returns the hyperbolic cosine of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic cosine of n
 */
#define math_cosh(n) MATH_DISPATCH_FLOATS(cosh, n)

/**
 * @brief Returns the hyperbolic tangent of a value.
 *
 * @param n the value
 *
 * @return the hyperbolic tangent of n
 */
#define math_tanh(n) MATH_DISPATCH_FLOATS(tanh, n)

/**
 * @brief Returns the inverse hyperbolic sine of a value.
 *
 * @param n the value
 *
 * @return the inverse hyperbolic sine of n
 */
#define math_asinh(n) MATH_DISPATCH_FLOATS(asinh, n)

/**
 * @brief Returns the inverse hyperbolic cosine of a value.
 *
 * @param n the value, greater than or equal to 1
 *
 * @return the inverse hyperbolic cosine of n
 */
#define math_acosh(n) MATH_DISPATCH_FLOATS(acosh, n)

/**
 * @brief Returns the inverse hyperbolic tangent of a value.
 *
 * @param n the value, in the range -1..1 (exclusive)
 *
 * @return the inverse hyperbolic tangent of n
 */
#define math_atanh(n) MATH_DISPATCH_FLOATS(atanh, n)

/**
 * @brief Returns the greatest common divisor (GCD) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the greatest common divisor
 */
#define math_gcd(a, b) MATH_DISPATCH_INTEGERS(gcd, a, b)

/**
 * @brief Returns the least common multiple (LCM) of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the least common multiple
 */
#define math_lcm(a, b) MATH_DISPATCH_INTEGERS(lcm, a, b)

/**
 * @brief Calculates the factorial of a number.
 *
 * @param n the number
 *
 * @return the factorial of n
 *
 * @exception ARITHMETIC_ERROR if n is negative or the result overflows
 */
#define math_factorial(n) MATH_DISPATCH_INTEGERS(factorial, n)

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
#define math_is_prime(n) MATH_DISPATCH_INTEGERS(is_prime, n)

/**
 * @brief Returns the absolute value of a number.
 *
 * @param n the number
 *
 * @return the absolute value of n
 */
#define math_abs(n) MATH_DISPATCH_NUMBERS(abs, n)

/**
 * @brief Returns the minimum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the minimum number
 */
#define math_min(a, b) MATH_DISPATCH_NUMBERS(min, a, b)

/**
 * @brief Returns the maximum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the maximum number
 */
#define math_max(a, b) MATH_DISPATCH_NUMBERS(max, a, b)

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
#define math_sign(n) MATH_DISPATCH_FLOATS(sign, n)

/**
 * @brief Rounds a floating-point number to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_round(n) MATH_DISPATCH_FLOATS(round, n)

/**
 * @brief Rounds a floating-point number down to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_floor(n) MATH_DISPATCH_FLOATS(floor, n)

/**
 * @brief Rounds a floating-point number up to the nearest integer.
 *
 * @param n the number
 *
 * @return the rounded number
 */
#define math_ceil(n) MATH_DISPATCH_FLOATS(ceil, n)

/**
 * @brief Truncates the fractional part of a floating-point number.
 *
 * @param n the number
 *
 * @return the truncated number
 */
#define math_trunc(n) MATH_DISPATCH_FLOATS(trunc, n)

/**
 * @brief Converts degrees to radians.
 *
 * @param degrees the value in degrees
 *
 * @return the value in radians
 */
#define math_to_radians(degrees) ((degrees) * MATH_PI(typeof(degrees)) / 180.0)

/**
 * @brief Converts radians to degrees.
 *
 * @param radians the value in radians
 *
 * @return the value in degrees
 */
#define math_to_degrees(radians) ((radians) * 180.0 / MATH_PI(typeof(radians)))

/*---------------------------------------------------------------------------------*/

#define MATH_DISPATCH_INTEGERS(operation, a, ...) _Generic((a),     \
    char:     math_##operation##_char,                              \
    schar:    math_##operation##_schar,                             \
    uchar:    math_##operation##_uchar,                             \
    short:    math_##operation##_short,                             \
    ushort:   math_##operation##_ushort,                            \
    int:      math_##operation##_int,                               \
    uint:     math_##operation##_uint,                              \
    long:     math_##operation##_long,                              \
    ulong:    math_##operation##_ulong,                             \
    llong:    math_##operation##_llong,                             \
    ullong:   math_##operation##_ullong                             \
)(a __VA_OPT__(,) __VA_ARGS__)

#define MATH_DISPATCH_FLOATS(operation, a, ...) _Generic((a),       \
    float:    math_##operation##_float,                             \
    double:   math_##operation##_double,                            \
    ldouble:  math_##operation##_ldouble                            \
)(a __VA_OPT__(,) __VA_ARGS__)

#define MATH_DISPATCH_NUMBERS(operation, a, ...) _Generic((a),  \
    char:     math_##operation##_char,                          \
    schar:    math_##operation##_schar,                         \
    uchar:    math_##operation##_uchar,                         \
    short:    math_##operation##_short,                         \
    ushort:   math_##operation##_ushort,                        \
    int:      math_##operation##_int,                           \
    uint:     math_##operation##_uint,                          \
    long:     math_##operation##_long,                          \
    ulong:    math_##operation##_ulong,                         \
    llong:    math_##operation##_llong,                         \
    ullong:   math_##operation##_ullong,                        \
    float:    math_##operation##_float,                         \
    double:   math_##operation##_double,                        \
    ldouble:  math_##operation##_ldouble                        \
)(a __VA_OPT__(,) __VA_ARGS__)

/* char */

char math_add_char(char a, char b);
char math_sub_char(char a, char b);
char math_mul_char(char a, char b);
char math_div_char(char a, char b);
char math_gcd_char(char a, char b);
char math_lcm_char(char a, char b);
char math_factorial_char(char number);
bool math_is_prime_char(char number);
char math_abs_char(char n);
char math_min_char(char a, char b);
char math_max_char(char a, char b);

/* schar */

schar math_add_schar(schar a, schar b);
schar math_sub_schar(schar a, schar b);
schar math_mul_schar(schar a, schar b);
schar math_div_schar(schar a, schar b);
schar math_gcd_schar(schar a, schar b);
schar math_lcm_schar(schar a, schar b);
schar math_factorial_schar(schar number);
bool math_is_prime_schar(schar number);
schar math_abs_schar(schar n);
schar math_min_schar(schar a, schar b);
schar math_max_schar(schar a, schar b);

/* uchar */

uchar math_add_uchar(uchar a, uchar b);
uchar math_sub_uchar(uchar a, uchar b);
uchar math_mul_uchar(uchar a, uchar b);
uchar math_div_uchar(uchar a, uchar b);
uchar math_gcd_uchar(uchar a, uchar b);
uchar math_lcm_uchar(uchar a, uchar b);
uchar math_factorial_uchar(uchar number);
bool math_is_prime_uchar(uchar number);
uchar math_abs_uchar(uchar n);
uchar math_min_uchar(uchar a, uchar b);
uchar math_max_uchar(uchar a, uchar b);

/* short */

short math_add_short(short a, short b);
short math_sub_short(short a, short b);
short math_mul_short(short a, short b);
short math_div_short(short a, short b);
short math_gcd_short(short a, short b);
short math_lcm_short(short a, short b);
short math_factorial_short(short number);
bool math_is_prime_short(short number);
short math_abs_short(short n);
short math_min_short(short a, short b);
short math_max_short(short a, short b);

/* ushort */

ushort math_add_ushort(ushort a, ushort b);
ushort math_sub_ushort(ushort a, ushort b);
ushort math_mul_ushort(ushort a, ushort b);
ushort math_div_ushort(ushort a, ushort b);
ushort math_gcd_ushort(ushort a, ushort b);
ushort math_lcm_ushort(ushort a, ushort b);
ushort math_factorial_ushort(ushort number);
bool math_is_prime_ushort(ushort number);
ushort math_abs_ushort(ushort n);
ushort math_min_ushort(ushort a, ushort b);
ushort math_max_ushort(ushort a, ushort b);

/* int */

int math_add_int(int a, int b);
int math_sub_int(int a, int b);
int math_mul_int(int a, int b);
int math_div_int(int a, int b);
int math_gcd_int(int a, int b);
int math_lcm_int(int a, int b);
int math_factorial_int(int number);
bool math_is_prime_int(int number);
int math_abs_int(int n);
int math_min_int(int a, int b);
int math_max_int(int a, int b);

/* uint */

uint math_add_uint(uint a, uint b);
uint math_sub_uint(uint a, uint b);
uint math_mul_uint(uint a, uint b);
uint math_div_uint(uint a, uint b);
uint math_gcd_uint(uint a, uint b);
uint math_lcm_uint(uint a, uint b);
uint math_factorial_uint(uint number);
bool math_is_prime_uint(uint number);
uint math_abs_uint(uint n);
uint math_min_uint(uint a, uint b);
uint math_max_uint(uint a, uint b);

/* long */

long math_add_long(long a, long b);
long math_sub_long(long a, long b);
long math_mul_long(long a, long b);
long math_div_long(long a, long b);
long math_gcd_long(long a, long b);
long math_lcm_long(long a, long b);
long math_factorial_long(long number);
bool math_is_prime_long(long number);
long math_abs_long(long n);
long math_min_long(long a, long b);
long math_max_long(long a, long b);

/* ulong */

ulong math_add_ulong(ulong a, ulong b);
ulong math_sub_ulong(ulong a, ulong b);
ulong math_mul_ulong(ulong a, ulong b);
ulong math_div_ulong(ulong a, ulong b);
ulong math_gcd_ulong(ulong a, ulong b);
ulong math_lcm_ulong(ulong a, ulong b);
ulong math_factorial_ulong(ulong number);
bool math_is_prime_ulong(ulong number);
ulong math_abs_ulong(ulong n);
ulong math_min_ulong(ulong a, ulong b);
ulong math_max_ulong(ulong a, ulong b);

/* llong */

llong math_add_llong(llong a, llong b);
llong math_sub_llong(llong a, llong b);
llong math_mul_llong(llong a, llong b);
llong math_div_llong(llong a, llong b);
llong math_gcd_llong(llong a, llong b);
llong math_lcm_llong(llong a, llong b);
llong math_factorial_llong(llong number);
bool math_is_prime_llong(llong number);
llong math_abs_llong(llong n);
llong math_min_llong(llong a, llong b);
llong math_max_llong(llong a, llong b);

/* ullong */

ullong math_add_ullong(ullong a, ullong b);
ullong math_sub_ullong(ullong a, ullong b);
ullong math_mul_ullong(ullong a, ullong b);
ullong math_div_ullong(ullong a, ullong b);
ullong math_gcd_ullong(ullong a, ullong b);
ullong math_lcm_ullong(ullong a, ullong b);
ullong math_factorial_ullong(ullong number);
bool math_is_prime_ullong(ullong number);
ullong math_abs_ullong(ullong n);
ullong math_min_ullong(ullong a, ullong b);
ullong math_max_ullong(ullong a, ullong b);

/* float */

float math_pow_float(float base, float exponent);
float math_log_float(float number);
float math_log2_float(float number);
float math_log10_float(float number);
float math_log1p_float(float number);

float math_exp_float(float number);
float math_exp2_float(float number);
float math_expm1_float(float number);
float math_sqrt_float(float number);
float math_cbrt_float(float number);
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
float math_sign_float(float number);

float math_round_float(float number);
float math_floor_float(float number);
float math_ceil_float(float number);
float math_trunc_float(float number);

/* double */

double math_pow_double(double base, double exponent);
double math_log_double(double number);
double math_log2_double(double number);
double math_log10_double(double number);
double math_log1p_double(double number);

double math_exp_double(double number);
double math_exp2_double(double number);
double math_expm1_double(double number);
double math_sqrt_double(double number);
double math_cbrt_double(double number);
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
double math_sign_double(double number);

double math_round_double(double number);
double math_floor_double(double number);
double math_ceil_double(double number);
double math_trunc_double(double number);

/* ldouble */

ldouble math_pow_ldouble(ldouble base, ldouble exponent);
ldouble math_log_ldouble(ldouble number);
ldouble math_log2_ldouble(ldouble number);
ldouble math_log10_ldouble(ldouble number);
ldouble math_log1p_ldouble(ldouble number);

ldouble math_exp_ldouble(ldouble number);
ldouble math_exp2_ldouble(ldouble number);
ldouble math_expm1_ldouble(ldouble number);
ldouble math_sqrt_ldouble(ldouble number);
ldouble math_cbrt_ldouble(ldouble number);
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
ldouble math_sign_ldouble(ldouble number);

ldouble math_round_ldouble(ldouble number);
ldouble math_floor_ldouble(ldouble number);
ldouble math_ceil_ldouble(ldouble number);
ldouble math_trunc_ldouble(ldouble number);

#endif
