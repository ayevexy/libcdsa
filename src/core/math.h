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
#define math_abs(n) MATH_DISPATCH_FLOATS(abs, n)

/**
 * @brief Returns the minimum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the minimum number
 */
#define math_min(a, b) MATH_DISPATCH_FLOATS(min, a, b)

/**
 * @brief Returns the maximum of two numbers.
 *
 * @param a the first number
 * @param b the second number
 *
 * @return the maximum number
 */
#define math_max(a, b) MATH_DISPATCH_FLOATS(max, a, b)

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

#define MATH_DISPATCH_FLOATS(operation, a, ...) _Generic((a),   \
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

float math_abs_float(float n);
float math_min_float(float a, float b);
float math_max_float(float a, float b);

/* double */

double math_abs_double(double n);
double math_min_double(double a, double b);
double math_max_double(double a, double b);

/* ldouble */

ldouble math_abs_ldouble(ldouble n);
ldouble math_min_ldouble(ldouble a, ldouble b);
ldouble math_max_ldouble(ldouble a, ldouble b);

#endif
