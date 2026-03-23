#include "util/errors.h"

#include <stdlib.h>
#include "unity.h"

void setUp() {

}

void tearDown() {

}

static void exit_handler(void) {
    TEST_ASSERT_TRUE(true); // Reached...
    TEST_ABORT();
}

void test_set_error_aborts_program() {
    // given
    atexit(exit_handler);
    // then
    set_error(UNKNOWN_ERROR, "No additional details available");
    //
    TEST_ASSERT_TRUE(false); // Should not be reached...
}

// Example function that sets an error
static int divide(int a, int b) {
    if (b == 0) {
        set_error(ARITHMETIC_ERROR, "Cannot divide by zero: %d / %d", a, b);
        return -1;
    }
    return a / b;
}

void test_attempt_on_fail_catch_error() {
    // given
    int result; Error error = attempt(result = divide(10, 0));
    // then
    TEST_ASSERT_EQUAL(result, -1);
    TEST_ASSERT_EQUAL(error, ARITHMETIC_ERROR);
    TEST_ASSERT_EQUAL_STRING("ARITHMETIC_ERROR: Cannot divide by zero: 10 / 0", error_message());
}

void test_attempt_on_success_catch_no_error() {
    // given
    int result; Error error = attempt(result = divide(10, 2));
    // then
    TEST_ASSERT_EQUAL(result, 5);
    TEST_ASSERT_EQUAL(error, NO_ERROR);
    TEST_ASSERT_EQUAL_STRING("", error_message());
}

// Example function that handles an error internally and wraps it
static int arithmetic_mean(int values[], int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    int result; Error error;
    if ((error = attempt(result = divide(sum, count)))) {
        // Additional context
        set_error(error, "Arithmetic Mean can't be calculated with zero values.\n\tDetails: %s", error_message());
    }
    return result;
}

void test_nested_attempt_on_fail_catch_error() {
    // given
    int result; Error error = attempt(result = arithmetic_mean((int[]){}, 0));
    // then
    TEST_ASSERT_EQUAL(result, -1);
    TEST_ASSERT_EQUAL(error, ARITHMETIC_ERROR);
    TEST_ASSERT_EQUAL_STRING("ARITHMETIC_ERROR: Arithmetic Mean can't be calculated with zero values.\n"
        "\tDetails: ARITHMETIC_ERROR: Cannot divide by zero: 0 / 0", error_message());
}

void test_nested_attempt_on_success_catch_no_error() {
    // given
    int result; Error error = attempt(result = arithmetic_mean((int[]){5, 5}, 2));
    // then
    TEST_ASSERT_EQUAL(result, 5);
    TEST_ASSERT_EQUAL(error, NO_ERROR);
    TEST_ASSERT_EQUAL_STRING("", error_message());
}

void test_get_plain_error_message() {
    // given
    attempt(divide(10, 0));
    // then
    TEST_ASSERT_EQUAL_STRING("Cannot divide by zero: 10 / 0", plain_error_message());
}

int main(void) {
    UNITY_BEGIN();
    if (TEST_PROTECT()) RUN_TEST(test_set_error_aborts_program);
    RUN_TEST(test_attempt_on_fail_catch_error);
    RUN_TEST(test_attempt_on_success_catch_no_error);
    RUN_TEST(test_nested_attempt_on_fail_catch_error);
    RUN_TEST(test_nested_attempt_on_success_catch_no_error);
    RUN_TEST(test_get_plain_error_message);
    return UNITY_END();
}