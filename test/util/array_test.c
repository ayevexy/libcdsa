#include "util/array.h"

#include "../test/test_utilities.h"
#include "util/errors.h"
#include "unity.h"

void setUp() {

}

void tearDown() {

}

void test_create_and_destroy_array() {
    // when
    const Array(int) ints = array_new(5, int, 1, 2, 3, 4, 5);
    // then
    TEST_ASSERT_EQUAL_INT(1, ints[0]);
    TEST_ASSERT_EQUAL_INT(2, ints[1]);
    TEST_ASSERT_EQUAL_INT(3, ints[2]);
    TEST_ASSERT_EQUAL_INT(4, ints[3]);
    TEST_ASSERT_EQUAL_INT(5, ints[4]);
    // clean up
    array_destroy(ints);
    TEST_ASSERT_NULL(ints);
}

void test_get_array_length() {
    // given
    const Array(int) ints = array_new(5, int, 1, 2, 3, 4, 5);
    // when
    int length = array_length(ints);
    // then
    TEST_ASSERT_EQUAL_INT(5, length);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_and_destroy_array);
    RUN_TEST(test_get_array_length);
    return UNITY_END();
}
