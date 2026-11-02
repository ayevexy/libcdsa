#include "core/math.h"

#include "core/errors.h"
#include "core/limits.h"

#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_math_add() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 145, math_add((uchar) 70, (uchar) 75));
    TEST_ASSERT_EQUAL((ushort) 1000, math_add((ushort) 400, (ushort) 600));
    TEST_ASSERT_EQUAL((uint) 3000, math_add((uint) 1000, (uint) 2000));
    TEST_ASSERT_EQUAL((ulong) 300000, math_add((ulong) 100000, (ulong) 200000));
    TEST_ASSERT_EQUAL((ullong) 3000000, math_add((ullong) 1000000, (ullong) 2000000));

    // then: signed integers
    TEST_ASSERT_EQUAL((schar) 15, math_add((schar) 7, (schar) 8));
    TEST_ASSERT_EQUAL((short) 150, math_add((short) 70, (short) 80));
    TEST_ASSERT_EQUAL(1500, math_add(700, 800));
    TEST_ASSERT_EQUAL((long) 15000, math_add((long) 7000, (long) 8000));
    TEST_ASSERT_EQUAL((llong) 150000, math_add((llong) 70000, (llong) 80000));

    // and: unsigned integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(UCHAR_MAX_VALUE, (uchar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(USHORT_MAX_VALUE, (ushort) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(UINT_MAX_VALUE, 1u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(ULONG_MAX_VALUE, 1ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(ULLONG_MAX_VALUE, 1ull)));

    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(SCHAR_MAX_VALUE, (schar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(SHORT_MAX_VALUE, (short) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(INT_MAX_VALUE, 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(LONG_MAX_VALUE, 1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(LLONG_MAX_VALUE, 1LL)));

    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(SCHAR_MIN_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(SHORT_MIN_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(INT_MIN_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(LONG_MIN_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_add(LLONG_MIN_VALUE, -1LL)));
}

void test_math_sub() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 5, math_sub((uchar) 75, (uchar) 70));
    TEST_ASSERT_EQUAL((ushort) 200, math_sub((ushort) 600, (ushort) 400));
    TEST_ASSERT_EQUAL((uint) 1000, math_sub((uint) 3000, (uint) 2000));
    TEST_ASSERT_EQUAL((ulong) 100000, math_sub((ulong) 200000, (ulong) 100000));
    TEST_ASSERT_EQUAL((ullong) 1000000, math_sub((ullong) 3000000, (ullong) 2000000));

    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -1, math_sub((schar) 7, (schar) 8));
    TEST_ASSERT_EQUAL((short) -10, math_sub((short) 70, (short) 80));
    TEST_ASSERT_EQUAL(-100, math_sub(700, 800));
    TEST_ASSERT_EQUAL((long) -1000, math_sub((long) 7000, (long) 8000));
    TEST_ASSERT_EQUAL((llong) -10000, math_sub((llong) 70000, (llong) 80000));

    // and: unsigned integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub((uchar) 0, (uchar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub((ushort) 0, (ushort) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(0u, 1u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(0ul, 1ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(0ull, 1ull)));

    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(SCHAR_MAX_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(SHORT_MAX_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(INT_MAX_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(LONG_MAX_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(LLONG_MAX_VALUE, -1LL)));

    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(SCHAR_MIN_VALUE, (schar) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(SHORT_MIN_VALUE, (short) 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(INT_MIN_VALUE, 1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(LONG_MIN_VALUE, 1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_sub(LLONG_MIN_VALUE, 1LL)));
}

void test_math_mul() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 150, math_mul((uchar) 15, (uchar) 10));
    TEST_ASSERT_EQUAL((ushort) 2000, math_mul((ushort) 100, (ushort) 20));
    TEST_ASSERT_EQUAL((uint) 20000, math_mul((uint) 100, (uint) 200));
    TEST_ASSERT_EQUAL((ulong) 200000, math_mul((ulong) 1000, (ulong) 200));
    TEST_ASSERT_EQUAL((ullong) 2000000, math_mul((ullong) 1000, (ullong) 2000));

    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -35, math_mul((schar) -7, (schar) 5));
    TEST_ASSERT_EQUAL((short) -500, math_mul((short) -25, (short) 20));
    TEST_ASSERT_EQUAL(-5000, math_mul(-100, 50));
    TEST_ASSERT_EQUAL((long) -50000, math_mul((long) -250, (long) 200));
    TEST_ASSERT_EQUAL((llong) -500000, math_mul((llong) -250, (llong) 2000));

    // and: unsigned integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(UCHAR_MAX_VALUE, (uchar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(USHORT_MAX_VALUE, (ushort) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(UINT_MAX_VALUE, 2u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(ULONG_MAX_VALUE, 2ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(ULLONG_MAX_VALUE, 2ull)));

    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(SCHAR_MAX_VALUE, (schar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(SHORT_MAX_VALUE, (short) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(INT_MAX_VALUE, 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(LONG_MAX_VALUE, 2L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(LLONG_MAX_VALUE, 2LL)));

    // and: signed integer underflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(SCHAR_MIN_VALUE, (schar) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(SHORT_MIN_VALUE, (short) 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(INT_MIN_VALUE, 2)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(LONG_MIN_VALUE, 2L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_mul(LLONG_MIN_VALUE, 2LL)));
}

void test_math_div() {
    // then: unsigned integers
    TEST_ASSERT_EQUAL((uchar) 7, math_div((uchar) 70, (uchar) 10));
    TEST_ASSERT_EQUAL((ushort) 20, math_div((ushort) 400, (ushort) 20));
    TEST_ASSERT_EQUAL((uint) 10, math_div((uint) 1000, (uint) 100));
    TEST_ASSERT_EQUAL((ulong) 100, math_div((ulong) 100000, (ulong) 1000));
    TEST_ASSERT_EQUAL((ullong) 1000, math_div((ullong) 1000000, (ullong) 1000));

    // then: signed integers
    TEST_ASSERT_EQUAL((schar) -7, math_div((schar) -70, (schar) 10));
    TEST_ASSERT_EQUAL((short) -20, math_div((short) -400, (short) 20));
    TEST_ASSERT_EQUAL(-10, math_div(-1000, 100));
    TEST_ASSERT_EQUAL((long) -100, math_div((long) -100000, (long) 1000));
    TEST_ASSERT_EQUAL((llong) -1000, math_div((llong) -1000000, (llong) 1000));

    // and: division by zero
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div((uchar) 1, (uchar) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div((ushort) 1, (ushort) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1u, 0u)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1ul, 0ul)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1ull, 0ull)));

    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div((schar) 1, (schar) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div((short) 1, (short) 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1, 0)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1L, 0L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(1LL, 0LL)));

    // and: signed integer overflow
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(SCHAR_MIN_VALUE, (schar) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(SHORT_MIN_VALUE, (short) -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(INT_MIN_VALUE, -1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(LONG_MIN_VALUE, -1L)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_div(LLONG_MIN_VALUE, -1LL)));
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
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_factorial(-1)));
    TEST_ASSERT_EQUAL(ARITHMETIC_ERROR, attempt(math_factorial(1'000'000)));
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
    RUN_TEST(test_math_add);
    RUN_TEST(test_math_sub);
    RUN_TEST(test_math_mul);
    RUN_TEST(test_math_div);
    RUN_TEST(test_math_gcd);
    RUN_TEST(test_math_lcm);
    RUN_TEST(test_math_factorial);
    RUN_TEST(test_math_is_prime);
    RUN_TEST(test_math_abs);
    RUN_TEST(test_math_min);
    RUN_TEST(test_math_max);
    RUN_TEST(test_math_clamp);
    RUN_TEST(test_math_sign);
    RUN_TEST(test_math_round);
    RUN_TEST(test_math_floor);
    RUN_TEST(test_math_ceil);
    RUN_TEST(test_math_trunc);
    RUN_TEST(test_math_to_radians);
    RUN_TEST(test_math_to_degrees);
    return UNITY_END();
}
