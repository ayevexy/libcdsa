#include "util/collection.h"

#include "list/array_list.h"
#include "../test_utilities.h"

#include "unity.h"

#define INT_ARRAY_LIST_OPTIONS() &(ArrayListOptions) {  \
    .initial_capacity = 10,                             \
    .growth_factor = 2.0f,                              \
    .destruct = noop_destruct,                          \
    .equals = int_pointer_value_equals,                 \
    .to_string = int_pointer_value_to_string,           \
    .memory_alloc = memory_try_alloc,                   \
    .memory_realloc = memory_try_realloc,               \
    .memory_dealloc = memory_dealloc                    \
}

#define POPULATE_ARRAY_LIST(array_list, T, ...)                     \
    do {                                                            \
        auto array = (T[]){__VA_ARGS__};                            \
        int length = count_args(__VA_ARGS__);                       \
        for (int i = 0; i < length; i++) {                          \
            array_list_add_last(array_list, new(int, array[i]));    \
        }                                                           \
    } while (false)

#define TEST_ASSERT_ARRAY_LIST_ELEMENTS_EQUALS(array_list, T, ...)              \
    do {                                                                        \
        auto array = (T[]){__VA_ARGS__};;                                       \
        int length = count_args(__VA_ARGS__);                                   \
        TEST_ASSERT_EQUAL(length, array_list_size(array_list));                 \
        for (int i = 0; i < length; i++) {                                      \
            TEST_ASSERT_EQUAL(array[i], *(T*) array_list_get(array_list, i));   \
        }                                                                       \
    } while (false)

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
    POPULATE_ARRAY_LIST(array_list, int,  1, 2, 3, 4, 5);
    // and
    Collection collection = array_list_to_collection(array_list);
    // when
    collection_for_each(collection, action_add_one);
    // then
    TEST_ASSERT_ARRAY_LIST_ELEMENTS_EQUALS(array_list, int, 2, 3, 4, 5, 6);
}

void test_collection_contains_all_elements() {
    // given
    ArrayList* new_array_list = array_list_new(INT_ARRAY_LIST_OPTIONS());
    // and
    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
    POPULATE_ARRAY_LIST(new_array_list, int, 2, 3, 4);
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
    // and
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
    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
    POPULATE_ARRAY_LIST(new_array_list, int, 2, 10, 4);
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
    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) elements = collection_to_array(array_list_to_collection(array_list));
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&elements);
}

void test_collection_as() {
    // given
    Collection collection = array_list_to_collection(array_list);
    // then
    TEST_ASSERT_EQUAL(array_list, collection_as(collection, ARRAY_LIST));
    TEST_ASSERT_EQUAL(TYPE_MISMATCH_ERROR, attempt(collection_as(collection, LINKED_LIST)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_collection_for_each);
    RUN_TEST(test_collection_contains_all_elements);
    RUN_TEST(test_empty_collection_contains_all_elements_of_empty_collection);
    RUN_TEST(test_collection_does_not_contains_all_elements);
    RUN_TEST(test_collection_to_array);
    RUN_TEST(test_collection_as);
    return UNITY_END();
}
