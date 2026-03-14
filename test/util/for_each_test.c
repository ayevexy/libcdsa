#include "util/for_each.h"

#include "../list/array_list_test.h"
#include "list/array_list.h"
#include "util/memory.h"

#include "../test_utilities.h"
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
    array_list_destroy(&array_list);
}

void test_iterate_through_data_structure() {
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

void test_iterate_through_empty_data_structure() {
    // given
    int count = 0;
    // when
    for_each (int* element, array_list) {
        count++;
    }
    // then
    TEST_ASSERT_EQUAL(0, count);
}

void test_iterate_through_collection_view() {
    // given
    int values[] = { 1, 2, 3, 4, 5 }, new_values[5], count = 0;
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    for_each (int* element, array_list_to_collection(array_list)) {
        new_values[count++] = *element;
    }
    // then
    TEST_ASSERT_ARRAY_EQUALS(new_values, (int**) &values);
    TEST_ASSERT_EQUAL(5, count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_iterate_through_data_structure);
    RUN_TEST(test_iterate_through_empty_data_structure);
    RUN_TEST(test_iterate_through_collection_view);
    return UNITY_END();
}