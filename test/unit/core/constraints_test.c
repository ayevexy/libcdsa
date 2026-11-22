#include "core/constraints.h"

#include "unity.h"

void setUp(void) {

}

void tearDown(void) {

}

static void test_require_non_null(void) {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_non_null(&(int){0})));
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, attempt(require_non_null(nullptr)));
}

static void test_require_positive(void) {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_positive(1)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_positive(0)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_positive(-1)));
}

static void test_require_non_negative(void) {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_non_negative(1)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_non_negative(0)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_non_negative(-1)));
}

static void test_require_in_range(void) {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(0, 0, 2)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(1, 0, 2)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(2, 0, 2)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_in_range(-1, 0, 2)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_in_range(3, 0, 2)));
}

static void test_require_in_bounds(void) {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_bounds(0, 1)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(require_in_bounds(-1, 1)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(require_in_bounds(1, 1)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_require_non_null);
    RUN_TEST(test_require_positive);
    RUN_TEST(test_require_non_negative);
    RUN_TEST(test_require_in_range);
    RUN_TEST(test_require_in_bounds);
    return UNITY_END();
}