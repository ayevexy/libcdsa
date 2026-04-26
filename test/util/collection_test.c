#include "util/collection.h"

#include "../list/array_list_test.h"
#include "list/array_list.h"
#include "util/memory.h"

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
    array_list_change_destructor(new_array_list, free);
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
    array_list_change_destructor(new_array_list, free);
    array_list_destroy(&new_array_list);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_collection_for_each);
    RUN_TEST(test_collection_contains_all_elements);
    RUN_TEST(test_empty_collection_contains_all_elements_of_empty_collection);
    RUN_TEST(test_collection_does_not_contains_all_elements);
    return UNITY_END();
}
