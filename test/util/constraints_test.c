#include "util/constraints.h"

#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_set_error_on_null() {
    // given
    void* pointer = nullptr;
    // when
    const Error error = attempt(set_error_on_null(pointer));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer' argument must not be null", plain_error_message());
}

void test_dont_set_error_on_not_null() {
    // given
    void* pointer = (void*) 0xDEADBEEF;
    // when
    const Error error = attempt(set_error_on_null(pointer));
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("", plain_error_message());
}

void test_set_error_on_null_two_pointers() {
    // given
    void* pointer_a = (void*) 0xDEADBEEF, * pointer_b = nullptr;
    // when
    const Error error = attempt(set_error_on_null(pointer_a, pointer_b));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer_b' argument must not be null", plain_error_message());
}

void test_set_error_on_null_four_pointers() {
    // given
    void* address = (void*) 0xDEADBEEF;
    void* pointer_a = address, * pointer_b = address, * pointer_c = address, * pointer_d = nullptr;
    // when
    const Error error = attempt(set_error_on_null(pointer_a, pointer_b, pointer_c, pointer_d));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'pointer_d' argument must not be null", plain_error_message());
}

void test_set_error_on_null_pointer_to_pointer() {
    // given
    void* inner_pointer = nullptr;
    void** pointer = &inner_pointer;
    // when
    const Error error = attempt(set_error_on_null(*pointer));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
    TEST_ASSERT_EQUAL_STRING("'*pointer' argument must not be null", plain_error_message());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_set_error_on_null);
    RUN_TEST(test_dont_set_error_on_not_null);
    RUN_TEST(test_set_error_on_null_two_pointers);
    RUN_TEST(test_set_error_on_null_four_pointers);
    RUN_TEST(test_set_error_on_null_pointer_to_pointer);
    return UNITY_END();
}