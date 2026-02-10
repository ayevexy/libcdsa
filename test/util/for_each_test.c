#include "util/for_each.h"

#include "../list/array_list_test.h"
#include "list/array_list.h"
#include "../test_functions.h"

#include "unity.h"

static ArrayList* array_list;

void setUp() {
    array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS(
        .destruct = free,
        .equals = int_pointer_value_equals,
        .to_string = int_pointer_value_to_string
    ));
}

void tearDown() {
    array_list_obliterate(&array_list);
}

void test_iterate_using_for_each_construct() {
    // given
    int values[] = { 1, 2, 3, 4, 5 }, new_values[5], count = 0;
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    for_each (int* element, array_list) {
        new_values[count++] = *element;
    }
    // then
    TEST_ASSERT_ARRAY_EQUALS(new_values, (int**) &values);
    TEST_ASSERT_EQUAL(5, count);
}

void test_iterate_using_for_each_construct_on_empty_collection() {
    // given
    int count = 0;
    // when
    for_each (int* element, array_list) {
        count++;
    }
    // then
    TEST_ASSERT_EQUAL(0, count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_iterate_using_for_each_construct);
    RUN_TEST(test_iterate_using_for_each_construct_on_empty_collection);
    return UNITY_END();
}