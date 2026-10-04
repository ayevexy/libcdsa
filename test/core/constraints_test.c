#include "core/constraints.h"

#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_require_non_null_set_error_on_null() {
    // given
    void* pointer = nullptr;
    // when
    const Error error = attempt(require_non_null(pointer));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer' must not be null", error_description());
}

void test_require_non_null_dont_set_error_on_not_null() {
    // given
    void* pointer = (void*) 0xDEADBEEF;
    // when
    const Error error = attempt(require_non_null(pointer));
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("", error_description());
}

void test_require_non_null_two_pointers() {
    // given
    void* pointer_a = (void*) 0xDEADBEEF, * pointer_b = nullptr;
    // when
    const Error error = attempt(require_non_null(pointer_a, pointer_b));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer_b' must not be null", error_description());
}

void test_require_non_null_four_pointers() {
    // given
    void* address = (void*) 0xDEADBEEF;
    void* pointer_a = address, * pointer_b = address, * pointer_c = address, * pointer_d = nullptr;
    // when
    const Error error = attempt(require_non_null(pointer_a, pointer_b, pointer_c, pointer_d));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer_d' must not be null", error_description());
}

void test_require_non_null_pointer_to_pointer() {
    // given
    void* inner_pointer = nullptr;
    void** pointer = &inner_pointer;
    // when
    const Error error = attempt(require_non_null(*pointer));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'*pointer' must not be null", error_description());
}

void test_require_positive() {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_positive(1)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_positive(0)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_positive(-1)));
}

void test_require_non_negative() {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_non_negative(1)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_non_negative(0)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_non_negative(-1)));
}

void test_require_in_range() {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(0, 0, 2)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(1, 0, 2)));
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_range(2, 0, 2)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_in_range(-1, 0, 2)));
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(require_in_range(3, 0, 2)));
}

void test_require_in_bounds() {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(require_in_bounds(0, 1)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(require_in_bounds(-1, 1)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(require_in_bounds(1, 1)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_require_non_null_set_error_on_null);
    RUN_TEST(test_require_non_null_dont_set_error_on_not_null);
    RUN_TEST(test_require_non_null_two_pointers);
    RUN_TEST(test_require_non_null_four_pointers);
    RUN_TEST(test_require_non_null_pointer_to_pointer);
    RUN_TEST(test_require_positive);
    RUN_TEST(test_require_non_negative);
    RUN_TEST(test_require_in_range);
    RUN_TEST(test_require_in_bounds);
    return UNITY_END();
}