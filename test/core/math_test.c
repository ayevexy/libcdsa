#include "core/math.h"

#include "core/errors.h"
#include "core/limits.h"

#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_math_add_exact() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 145, math_add_exact((uchar) 70, (uchar) 75));
    TEST_ASSERT_EQUAL((ushort) 1000, math_add_exact((ushort) 400, (ushort) 600));
    TEST_ASSERT_EQUAL((uint) 3000, math_add_exact((uint) 1000, (uint) 2000));
    TEST_ASSERT_EQUAL((ulong) 300000, math_add_exact((ulong) 100000, (ulong) 200000));
    TEST_ASSERT_EQUAL((ullong) 3000000, math_add_exact((ullong) 1000000, (ullong) 2000000));
    // then: signed integers
    TEST_ASSERT_EQUAL((schar) 15, math_add_exact((schar) 7, (schar) 8));
    TEST_ASSERT_EQUAL((short) 150, math_add_exact((short) 70, (short) 80));
    TEST_ASSERT_EQUAL(1500, math_add_exact(700, 800));
    TEST_ASSERT_EQUAL((long) 15000, math_add_exact((long) 7000, (long) 8000));
    TEST_ASSERT_EQUAL((llong) 150000, math_add_exact((llong) 70000, (llong) 80000));
    // and: unsigned integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(UCHAR_MAX_VALUE, (uchar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(USHORT_MAX_VALUE, (ushort) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(UINT_MAX_VALUE, 1u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(ULONG_MAX_VALUE, 1ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(ULLONG_MAX_VALUE, 1ull)));
    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(SCHAR_MAX_VALUE, (schar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(SHORT_MAX_VALUE, (short) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(INT_MAX_VALUE, 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(LONG_MAX_VALUE, 1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(LLONG_MAX_VALUE, 1LL)));
    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(SCHAR_MIN_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(SHORT_MIN_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(INT_MIN_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(LONG_MIN_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add_exact(LLONG_MIN_VALUE, -1LL)));
}

void test_math_sub_exact() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 5, math_sub_exact((uchar) 75, (uchar) 70));
    TEST_ASSERT_EQUAL((ushort) 200, math_sub_exact((ushort) 600, (ushort) 400));
    TEST_ASSERT_EQUAL((uint) 1000, math_sub_exact((uint) 3000, (uint) 2000));
    TEST_ASSERT_EQUAL((ulong) 100000, math_sub_exact((ulong) 200000, (ulong) 100000));
    TEST_ASSERT_EQUAL((ullong) 1000000, math_sub_exact((ullong) 3000000, (ullong) 2000000));
    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -1, math_sub_exact((schar) 7, (schar) 8));
    TEST_ASSERT_EQUAL((short) -10, math_sub_exact((short) 70, (short) 80));
    TEST_ASSERT_EQUAL(-100, math_sub_exact(700, 800));
    TEST_ASSERT_EQUAL((long) -1000, math_sub_exact((long) 7000, (long) 8000));
    TEST_ASSERT_EQUAL((llong) -10000, math_sub_exact((llong) 70000, (llong) 80000));
    // and: unsigned integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact((uchar) 0, (uchar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact((ushort) 0, (ushort) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(0u, 1u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(0ul, 1ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(0ull, 1ull)));
    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(SCHAR_MAX_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(SHORT_MAX_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(INT_MAX_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(LONG_MAX_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(LLONG_MAX_VALUE, -1LL)));
    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(SCHAR_MIN_VALUE, (schar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(SHORT_MIN_VALUE, (short) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(INT_MIN_VALUE, 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(LONG_MIN_VALUE, 1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub_exact(LLONG_MIN_VALUE, 1LL)));
}

void test_math_mul_exact() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 150, math_mul_exact((uchar) 15, (uchar) 10));
    TEST_ASSERT_EQUAL((ushort) 2000, math_mul_exact((ushort) 100, (ushort) 20));
    TEST_ASSERT_EQUAL((uint) 20000, math_mul_exact((uint) 100, (uint) 200));
    TEST_ASSERT_EQUAL((ulong) 200000, math_mul_exact((ulong) 1000, (ulong) 200));
    TEST_ASSERT_EQUAL((ullong) 2000000, math_mul_exact((ullong) 1000, (ullong) 2000));
    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -35, math_mul_exact((schar) -7, (schar) 5));
    TEST_ASSERT_EQUAL((short) -500, math_mul_exact((short) -25, (short) 20));
    TEST_ASSERT_EQUAL(-5000, math_mul_exact(-100, 50));
    TEST_ASSERT_EQUAL((long) -50000, math_mul_exact((long) -250, (long) 200));
    TEST_ASSERT_EQUAL((llong) -500000, math_mul_exact((llong) -250, (llong) 2000));
    // and: unsigned integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(UCHAR_MAX_VALUE, (uchar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(USHORT_MAX_VALUE, (ushort) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(UINT_MAX_VALUE, 2u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(ULONG_MAX_VALUE, 2ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(ULLONG_MAX_VALUE, 2ull)));
    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(SCHAR_MAX_VALUE, (schar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(SHORT_MAX_VALUE, (short) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(INT_MAX_VALUE, 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(LONG_MAX_VALUE, 2L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(LLONG_MAX_VALUE, 2LL)));
    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(SCHAR_MIN_VALUE, (schar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(SHORT_MIN_VALUE, (short) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(INT_MIN_VALUE, 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(LONG_MIN_VALUE, 2L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul_exact(LLONG_MIN_VALUE, 2LL)));
}

void test_math_div_exact() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 7, math_div_exact((uchar) 70, (uchar) 10));
    TEST_ASSERT_EQUAL((ushort) 20, math_div_exact((ushort) 400, (ushort) 20));
    TEST_ASSERT_EQUAL((uint) 10, math_div_exact((uint) 1000, (uint) 100));
    TEST_ASSERT_EQUAL((ulong) 100, math_div_exact((ulong) 100000, (ulong) 1000));
    TEST_ASSERT_EQUAL((ullong) 1000, math_div_exact((ullong) 1000000, (ullong) 1000));
    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -7, math_div_exact((schar) -70, (schar) 10));
    TEST_ASSERT_EQUAL((short) -20, math_div_exact((short) -400, (short) 20));
    TEST_ASSERT_EQUAL(-10, math_div_exact(-1000, 100));
    TEST_ASSERT_EQUAL((long) -100, math_div_exact((long) -100000, (long) 1000));
    TEST_ASSERT_EQUAL((llong) -1000, math_div_exact((llong) -1000000, (llong) 1000));
    // and: division by zero
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact((uchar) 1, (uchar) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact((ushort) 1, (ushort) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1u, 0u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1ul, 0ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1ull, 0ull)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact((schar) 1, (schar) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact((short) 1, (short) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1, 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1L, 0L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(1LL, 0LL)));
    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(SCHAR_MIN_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(SHORT_MIN_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(INT_MIN_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(LONG_MIN_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div_exact(LLONG_MIN_VALUE, -1LL)));
}

#define TEST_MATH_FUNCTIONS(T, suffix, tolerance, e)                        \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 8, math_pow_##suffix(2, 3));        \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_log_##suffix(e));           \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_log2_##suffix(2));          \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_log10_##suffix(10));        \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, e, math_exp_##suffix(1));           \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, e - 1, math_expm1_##suffix(1));     \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_log1p_##suffix(e - 1));     \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 2, math_sqrt_##suffix(4));          \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 2, math_cbrt_##suffix(8));          \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 5, math_hypot_##suffix(3, 4))

void test_math_functions() {
    // then
    TEST_MATH_FUNCTIONS(float, float, 1e-6f, MATH_E(float));
    TEST_MATH_FUNCTIONS(double, double, 1e-12, MATH_E(double));
    TEST_MATH_FUNCTIONS(ldouble, ldouble, 1e-12, MATH_E(ldouble));
}

#define TEST_TRIGONOMETRIC(T, suffix, tolerance, pi, half)                      \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_sin_##suffix(0));               \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_cos_##suffix(0));               \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_tan_##suffix(pi / 4));          \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, pi / 6, math_asin_##suffix(half));      \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, pi / 3, math_acos_##suffix(half));      \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, pi / 4, math_atan_##suffix(1));         \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, pi / 4, math_atan2_##suffix(1, 1));     \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_sinh_##suffix(0));              \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 1, math_cosh_##suffix(0));              \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_tanh_##suffix(0));              \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_asinh_##suffix(0));             \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_acosh_##suffix(1));             \
    TEST_ASSERT_FLOAT_WITHIN(tolerance, 0, math_atanh_##suffix(0))

void test_math_trigonometric() {
    // then
    TEST_TRIGONOMETRIC(float, float, 1e-6f, MATH_PI(float), 0.5f);
    TEST_TRIGONOMETRIC(double, double, 1e-12, MATH_PI(double), 0.5);
    TEST_TRIGONOMETRIC(ldouble, ldouble, 1e-15L, MATH_PI(ldouble), 0.5L);
}

void test_math_gcd() {
    // then
    TEST_ASSERT_EQUAL(6, math_gcd(-12, -18));
    TEST_ASSERT_EQUAL(6, math_gcd(12, 18));
    TEST_ASSERT_EQUAL(5, math_gcd(15, 20));
    TEST_ASSERT_EQUAL(1, math_gcd(7, 9));
    TEST_ASSERT_EQUAL(10, math_gcd(10, 0));
    TEST_ASSERT_EQUAL(10, math_gcd(0, 10));
    TEST_ASSERT_EQUAL(0, math_gcd(0, 0));
}

void test_math_lcm() {
    // then
    TEST_ASSERT_EQUAL(36, math_lcm(-12, -18));
    TEST_ASSERT_EQUAL(36, math_lcm(12, 18));
    TEST_ASSERT_EQUAL(60, math_lcm(15, 20));
    TEST_ASSERT_EQUAL(63, math_lcm(7, 9));
    TEST_ASSERT_EQUAL(0, math_lcm(10, 0));
    TEST_ASSERT_EQUAL(0,math_lcm(0, 10));
}

void test_math_factorial() {
    // then
    TEST_ASSERT_EQUAL(1, math_factorial(0));
    TEST_ASSERT_EQUAL(1, math_factorial(1));
    TEST_ASSERT_EQUAL(2, math_factorial(2));
    TEST_ASSERT_EQUAL(6, math_factorial(3));
    TEST_ASSERT_EQUAL(24, math_factorial(4));
    TEST_ASSERT_EQUAL(120, math_factorial(5));
    TEST_ASSERT_EQUAL(720, math_factorial(6));
    TEST_ASSERT_EQUAL(5040, math_factorial(7));
    TEST_ASSERT_EQUAL(3628800, math_factorial(10));
    TEST_ASSERT_EQUAL(479001600, math_factorial(12));
    // and
    TEST_ASSERT_EQUAL(INT_MIN_VALUE, math_factorial(-1));
    TEST_ASSERT_EQUAL(INT_MAX_VALUE, math_factorial(1'000'000));
}

void test_math_is_prime() {
    // then
    TEST_ASSERT_FALSE(math_is_prime(0));
    TEST_ASSERT_FALSE(math_is_prime(1));
    TEST_ASSERT_TRUE(math_is_prime(2));
    TEST_ASSERT_TRUE(math_is_prime(3));
    TEST_ASSERT_FALSE(math_is_prime(4));
    TEST_ASSERT_TRUE(math_is_prime(5));
    TEST_ASSERT_FALSE(math_is_prime(9));
    TEST_ASSERT_TRUE(math_is_prime(17));
    TEST_ASSERT_TRUE(math_is_prime(97));
    TEST_ASSERT_FALSE(math_is_prime(100));
}

void test_math_is_power_of_two() {
    // then
    TEST_ASSERT_TRUE(math_is_power_of_two(2));
    TEST_ASSERT_TRUE(math_is_power_of_two(8));
    TEST_ASSERT_TRUE(math_is_power_of_two(32));
    TEST_ASSERT_FALSE(math_is_power_of_two(-8));
    TEST_ASSERT_FALSE(math_is_power_of_two(-32));
    TEST_ASSERT_FALSE(math_is_power_of_two(-1));
    TEST_ASSERT_FALSE(math_is_power_of_two(237));
}

void test_math_abs() {
    // then
    TEST_ASSERT_EQUAL(10, math_abs(10));
    TEST_ASSERT_EQUAL(10, math_abs(-10));
    TEST_ASSERT_EQUAL(0, math_abs(0));
}

void test_math_min() {
    // then
    TEST_ASSERT_EQUAL(10, math_min(10, 20));
    TEST_ASSERT_EQUAL(10, math_min(20, 10));
    TEST_ASSERT_EQUAL(-10, math_min(-10, 10));
    TEST_ASSERT_EQUAL(-20, math_min(-10, -20));
    TEST_ASSERT_EQUAL(10, math_min(10, 10));
}

void test_math_max() {
    // then
    TEST_ASSERT_EQUAL(20, math_max(10, 20));
    TEST_ASSERT_EQUAL(20, math_max(20, 10));
    TEST_ASSERT_EQUAL(10, math_max(-10, 10));
    TEST_ASSERT_EQUAL(-10, math_max(-10, -20));
    TEST_ASSERT_EQUAL(10, math_max(10, 10));
}

void test_math_clamp() {
    // then
    TEST_ASSERT_EQUAL(0, math_clamp(-1, 0, 1));
    TEST_ASSERT_EQUAL(1, math_clamp(2, 0, 1));
    TEST_ASSERT_EQUAL(0, math_clamp(0, 0, 1));
}

void test_math_sign() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(-1.0, math_sign(-1.0));
    TEST_ASSERT_EQUAL_FLOAT(0.0, math_sign(0.0));
    TEST_ASSERT_EQUAL_FLOAT(1.0, math_sign(1.0));
}

void test_math_copysign() {
    // then: positive and negative signs
    TEST_ASSERT_EQUAL_FLOAT(-5.0, math_copysign(5.0, -1.0));
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_copysign(-5.0, 1.0));
    // then: zero signs
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_copysign(5.0, 0.0));
    TEST_ASSERT_EQUAL_FLOAT(-5.0, math_copysign(5.0, -0.0));
    // then: zero magnitude
    TEST_ASSERT_EQUAL_FLOAT(0.0, math_copysign(0.0, 1.0));
    TEST_ASSERT_EQUAL_FLOAT(-0.0, math_copysign(0.0, -1.0));
}

void test_math_frexp() {
    // when
    int exponent; float mantissa = math_frexp(10.0, &exponent);
    // then
    TEST_ASSERT_EQUAL_FLOAT(0.625f, mantissa);
    TEST_ASSERT_EQUAL(4, exponent);
}

void test_math_ldexp() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(0.437500, math_ldexp(7.0, -4));
}

void test_math_next_after() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(nextafterf(1.0f, 2.0f), math_next_after(1.0f, 2.0f));
    TEST_ASSERT_EQUAL_FLOAT(nextafter(1.0, 2.0), math_next_after(1.0, 2.0));
    TEST_ASSERT_EQUAL_FLOAT(nextafterl(1.0L, 2.0L), math_next_after(1.0L, 2.0L));
}

void test_math_next_toward() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(nexttowardf(1.0f, 2.0L), math_next_toward(1.0f, 2.0L));
    TEST_ASSERT_EQUAL_FLOAT(nexttoward(1.0, 2.0L), math_next_toward(1.0, 2.0L));
    TEST_ASSERT_EQUAL_FLOAT(nexttowardl(1.0L, 2.0L), math_next_toward(1.0L, 2.0L));
}

void test_math_remainder() {
    // then: positive values
    TEST_ASSERT_EQUAL_FLOAT(1.0, math_remainder(5.0, 2.0));
    TEST_ASSERT_EQUAL_FLOAT(-1.0, math_remainder(7.0, 2.0));
    TEST_ASSERT_EQUAL_FLOAT(-2.0, math_remainder(6.0, 4.0));
    // then: negative values
    TEST_ASSERT_EQUAL_FLOAT(-1.0, math_remainder(-5.0, 2.0));
    TEST_ASSERT_EQUAL_FLOAT(1.0, math_remainder(-7.0, 2.0));
    // then: exact division
    TEST_ASSERT_EQUAL_FLOAT(0.0, math_remainder(8.0, 2.0));
}

void test_math_mod() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(1.0, math_mod(10.0,  3.0));
    TEST_ASSERT_EQUAL_FLOAT(-1.0, math_mod(-10.0,  3.0));
    TEST_ASSERT_EQUAL_FLOAT(1.0, math_mod(10.0,  -3.0));
    TEST_ASSERT_EQUAL_FLOAT(-1.0, math_mod(-10.0, -3.0));
}

void test_math_fma() {
    // given
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_fma(2.0, 2.0, 1.0));
    TEST_ASSERT_EQUAL_FLOAT(-3.0, math_fma(2.0, -2.0, 1.0));
}

void test_math_round() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(4.0, math_round(4.1));
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_round(4.5));
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_round(4.9));
    TEST_ASSERT_EQUAL_FLOAT(-4, math_round(-4.1));
}

void test_math_floor() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(4.0, math_floor(4.1));
    TEST_ASSERT_EQUAL_FLOAT(4.0, math_floor(4.5));
    TEST_ASSERT_EQUAL_FLOAT(4.0, math_floor(4.9));
    TEST_ASSERT_EQUAL_FLOAT(-5, math_floor(-4.1));
}

void test_math_ceil() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_ceil(4.1));
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_ceil(4.5));
    TEST_ASSERT_EQUAL_FLOAT(5.0, math_ceil(4.9));
    TEST_ASSERT_EQUAL_FLOAT(-4, math_ceil(-4.1));
}

void test_math_trunc() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(4.000, math_trunc(4.125427));
    TEST_ASSERT_EQUAL_FLOAT(4.0000, math_trunc(4.523453));
    TEST_ASSERT_EQUAL_FLOAT(4.0000, math_trunc(4.985345));
    TEST_ASSERT_EQUAL_FLOAT(-4.0000, math_trunc(-4.111239));
}

void test_math_is() {
    // then
    TEST_ASSERT_TRUE(math_is_nan(0.0 / 0.0));
    TEST_ASSERT_TRUE(math_is_infinite(1.0 / 0.0));
    TEST_ASSERT_TRUE(math_is_infinite(-1.0 / 0.0));
    TEST_ASSERT_TRUE(math_is_finite(1.0 / 1.0));
    TEST_ASSERT_TRUE(math_is_normal(1.0 / 1.0));
}

void test_math_is_close() {
    // then
    TEST_ASSERT_TRUE(math_is_close(3.0010, 3.0015, 0.1));
    TEST_ASSERT_FALSE(math_is_close(3.0010, 3.0015, 0.0001));
}

void test_math_lerp() {
    // then: interpolation
    TEST_ASSERT_EQUAL_FLOAT(0, math_lerp(0.0, 10.0, 0.0));
    TEST_ASSERT_EQUAL_FLOAT(5, math_lerp(0.0, 10.0, 0.5));
    TEST_ASSERT_EQUAL_FLOAT(10, math_lerp(0.0, 10.0, 1.0));
    // then: reverse interpolation
    TEST_ASSERT_EQUAL_FLOAT(5, math_lerp(10.0, 0.0, 0.5));
    // then: extrapolation
    TEST_ASSERT_EQUAL_FLOAT(15, math_lerp(0.0, 10.0, 1.5));
    TEST_ASSERT_EQUAL_FLOAT(-5, math_lerp(0.0, 10.0, -0.5));
    // then: negative values
    TEST_ASSERT_EQUAL_FLOAT(-5, math_lerp(-10.0, 0.0, 0.5));
    TEST_ASSERT_EQUAL_FLOAT(-10, math_lerp(-10.0, -20.0, 0.0));
    TEST_ASSERT_EQUAL_FLOAT(-20, math_lerp(-10.0, -20.0, 1.0));
}

void test_math_percentage() {
    // then
    TEST_ASSERT_EQUAL_FLOAT(10.0, math_percentage(100.0, 10.0));
    TEST_ASSERT_EQUAL_FLOAT(2.0, math_percentage(200.0, 1.0));
    TEST_ASSERT_EQUAL_FLOAT(200.0, math_percentage(100.0, 200.0));
}

void test_math_random() {
    // then: float
    float random_float = math_random(float);
    TEST_ASSERT_TRUE(random_float >= 0.0f);
    TEST_ASSERT_TRUE(random_float < 1.0f);
    // then: double
    double random_double = math_random(double);
    TEST_ASSERT_TRUE(random_double >= 0.0);
    TEST_ASSERT_TRUE(random_double < 1.0);
    // then: ldouble
    ldouble random_ldouble = math_random(ldouble);
    TEST_ASSERT_TRUE(random_ldouble >= 0.0L);
    TEST_ASSERT_TRUE(random_ldouble < 1.0L);
}

void test_math_to_radians() {
    // then
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 0.0, math_to_radians(0.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 0.7853981633974483, math_to_radians(45.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 1.5707963267948966, math_to_radians(90.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 3.1415926535897932, math_to_radians(180.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 6.2831853071795864, math_to_radians(360.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, -1.5707963267948966, math_to_radians(-90.0));
}

void test_math_to_degrees() {
    // then
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 0.0, math_to_degrees(0.0));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 45.0, math_to_degrees(0.7853981633974483));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 90.0, math_to_degrees(1.5707963267948966));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 180.0, math_to_degrees(3.1415926535897932));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 360.0, math_to_degrees(6.2831853071795864));
    TEST_ASSERT_FLOAT_WITHIN(0.000001, -90.0, math_to_degrees(-1.5707963267948966));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_math_add_exact);
    RUN_TEST(test_math_sub_exact);
    RUN_TEST(test_math_mul_exact);
    RUN_TEST(test_math_div_exact);
    RUN_TEST(test_math_functions);
    RUN_TEST(test_math_trigonometric);
    RUN_TEST(test_math_gcd);
    RUN_TEST(test_math_lcm);
    RUN_TEST(test_math_factorial);
    RUN_TEST(test_math_is_prime);
    RUN_TEST(test_math_is_power_of_two);
    RUN_TEST(test_math_abs);
    RUN_TEST(test_math_min);
    RUN_TEST(test_math_max);
    RUN_TEST(test_math_clamp);
    RUN_TEST(test_math_sign);
    RUN_TEST(test_math_copysign);
    RUN_TEST(test_math_frexp);
    RUN_TEST(test_math_ldexp);
    RUN_TEST(test_math_next_after);
    RUN_TEST(test_math_next_toward);
    RUN_TEST(test_math_remainder);
    RUN_TEST(test_math_mod);
    RUN_TEST(test_math_fma);
    RUN_TEST(test_math_round);
    RUN_TEST(test_math_floor);
    RUN_TEST(test_math_ceil);
    RUN_TEST(test_math_trunc);
    RUN_TEST(test_math_is);
    RUN_TEST(test_math_is_close);
    RUN_TEST(test_math_lerp);
    RUN_TEST(test_math_percentage);
    RUN_TEST(test_math_random);
    RUN_TEST(test_math_to_radians);
    RUN_TEST(test_math_to_degrees);
    return UNITY_END();
}
