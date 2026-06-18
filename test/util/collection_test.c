#include "util/collection.h"
#include "list/array_list.h"
#include "util/memory.h"

#include "unity.h"
#include "../test_utilities.h"

#define INT_ARRAY_LIST_OPTIONS() &(ArrayListOptions) {  \
    .initial_capacity = 10,                             \
    .growth_factor = 2.0f,                              \
    .destruct = noop_destruct,                          \
    .equals = int_pointer_value_equals,                 \
    .to_string = int_pointer_value_to_string,           \
    .memory_alloc = malloc,                             \
    .memory_realloc = realloc,                          \
    .memory_dealloc = free                              \
}

#define POPULATE_ARRAY_LIST(array_list, array)                  \
    for (int i = 0; i < SIZE(array); i++) {                     \
        array_list_add_last(array_list, new(int, array[i]));    \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAY_LIST(array, array_list)               \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) array_list_get(array_list, i));     \
    }                                                                           \
    TEST_ASSERT_EQUAL(SIZE(array), array_list_size(array_list));

static ArrayList* array_list;

void setUp() {
    array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS(
        .destruct = delete,
        .equals = int_pointer_value_equals,
        .to_string = int_pointer_value_to_string
    ));
}

void tearDown() {
    array_list_destroy(&array_list);
}

static void action_add_one(void* element) {
    *(int*) element += 1;
}

void test_collection_for_each() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    Collection collection = array_list_to_collection(array_list);
    // when
    collection_for_each(collection, action_add_one);
    // then
    int new_values[] = { 2, 3, 4, 5, 6 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAY_LIST(new_values, array_list);
}

void test_collection_contains_all_elements() {
    // given
    ArrayList* new_array_list = array_list_new(INT_ARRAY_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 2, 3, 4};
    POPULATE_ARRAY_LIST(new_array_list, other_values);
    // and
    Collection collection = array_list_to_collection(array_list);
    // when
    bool contains_all = collection_contains_all(collection, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    array_list_change_destructor(new_array_list, delete);
    array_list_destroy(&new_array_list);
}

void test_empty_collection_contains_all_elements_of_empty_collection() {
    // given
    ArrayList* new_array_list = array_list_new(INT_ARRAY_LIST_OPTIONS());
    Collection collection = array_list_to_collection(array_list);
    // when
    bool contains_all = collection_contains_all(collection, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    array_list_destroy(&new_array_list);
}

void test_collection_does_not_contains_all_elements() {
    // given
    ArrayList* new_array_list = array_list_new(INT_ARRAY_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 2, 10, 4};
    POPULATE_ARRAY_LIST(new_array_list, other_values);
    // and
    Collection collection = array_list_to_collection(array_list);
    // when
    bool contains_all = collection_contains_all(collection, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    array_list_change_destructor(new_array_list, delete);
    array_list_destroy(&new_array_list);
}

void test_collection_to_array() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    Array(void*) elements = collection_to_array(array_list_to_collection(array_list));
    // then
    TEST_ASSERT_ARRAY_EQUALS(values, elements);
    // clean up
    array_destroy(&elements);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_collection_for_each);
    RUN_TEST(test_collection_contains_all_elements);
    RUN_TEST(test_empty_collection_contains_all_elements_of_empty_collection);
    RUN_TEST(test_collection_does_not_contains_all_elements);
    RUN_TEST(test_collection_to_array);
    return UNITY_END();
}
