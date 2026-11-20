#include "collections/list/array_list.h"

#include "collections/for_each.h" // should be included after all data structures
#include "collections/callbacks.h"
#include "core/memory.h"

#include "test_utilities.h"
#include "unity.h"

#define POPULATE_ARRAY_LIST(array_list, T, ...)                     \
    do {                                                            \
        auto array = (T[]){__VA_ARGS__};                            \
        int length = COUNT_ARGS(__VA_ARGS__);                       \
        for (int i = 0; i < length; i++) {                          \
            array_list_add_last(array_list, new(int, array[i]));    \
        }                                                           \
    } while (false)

static ArrayList* array_list;

void setUp() {
    array_list = array_list_new(ARRAY_LIST_OPTIONS(int));
    array_list_change_destructor(array_list, delete);
}

void tearDown() {
    array_list_destroy(&array_list);
}

void test_iterate_through_data_structure() {
    // given
    int* new_values[5], count = 0;
    // and
    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
    // when
    for_each (int* element, array_list) {
        new_values[count++] = element;
    }
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(new_values, int, 1, 2, 3, 4, 5);
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
    int* new_values[5], count = 0;
    // and
    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
    // when
    for_each (int* element, array_list_to_collection(array_list)) {
        new_values[count++] = element;
    }
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(new_values, int, 1, 2, 3, 4, 5);
    TEST_ASSERT_EQUAL(5, count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_iterate_through_data_structure);
    RUN_TEST(test_iterate_through_empty_data_structure);
    RUN_TEST(test_iterate_through_collection_view);
    return UNITY_END();
}