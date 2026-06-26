#include "util/array.h"

#include "../test/test_utilities.h"
#include "util/errors.h"
#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_array_new() {
    // given
    const Array(int) ints = array_new(5, int);
    // then
    TEST_ASSERT_EQUAL_INT(0, ints[0]);
    TEST_ASSERT_EQUAL_INT(0, ints[1]);
    TEST_ASSERT_EQUAL_INT(0, ints[2]);
    TEST_ASSERT_EQUAL_INT(0, ints[3]);
    TEST_ASSERT_EQUAL_INT(0, ints[4]);
    // clean up
    array_destroy(&ints);
    TEST_ASSERT_NULL(ints);
}

void test_array_of() {
    // when
    const Array(int) ints = array_of(int, 1, 2, 3, 4, 5);
    // then
    TEST_ASSERT_EQUAL_INT(1, ints[0]);
    TEST_ASSERT_EQUAL_INT(2, ints[1]);
    TEST_ASSERT_EQUAL_INT(3, ints[2]);
    TEST_ASSERT_EQUAL_INT(4, ints[3]);
    TEST_ASSERT_EQUAL_INT(5, ints[4]);
    // clean up
    array_destroy(&ints);
    TEST_ASSERT_NULL(ints);
}

void test_get_array_element() {
    // given
    const Array(int) ints = array_of(int, 1, 2, 3, 4, 5);
    // when
    int n = array_get(ints, 2);
    Error error1 = attempt(array_get(ints, -1));
    Error error2 = attempt(array_get(ints, 5));
    // then
    TEST_ASSERT_EQUAL_INT(3, n);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error1);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error2);
}

void test_set_array_element() {
    // given
    Array(int) ints = array_of(int, 1, 2, 3, 4, 5);
    // when
    array_set(ints, 2, 30);
    Error error1 = attempt(array_set(ints, -1, 30));
    Error error2 = attempt(array_set(ints, 5, 30));
    // then
    TEST_ASSERT_EQUAL_INT(1, ints[0]);
    TEST_ASSERT_EQUAL_INT(2, ints[1]);
    TEST_ASSERT_EQUAL_INT(30, ints[2]);
    TEST_ASSERT_EQUAL_INT(4, ints[3]);
    TEST_ASSERT_EQUAL_INT(5, ints[4]);
    // and
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error1);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error2);
}

void test_get_array_length() {
    // given
    const Array(int) ints = array_of(int, 1, 2, 3, 4, 5);
    // when
    int length = array_length(ints);
    // then
    TEST_ASSERT_EQUAL_INT(5, length);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_array_new);
    RUN_TEST(test_array_of);
    RUN_TEST(test_get_array_element);
    RUN_TEST(test_set_array_element);
    RUN_TEST(test_get_array_length);
    return UNITY_END();
}
