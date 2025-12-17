#include "list/array_list.h"

#include "util/iterator.h"

#include "unity.h"
#include "fff.h"
#include <stdlib.h>
#include <stdio.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(free, void*);
FAKE_VALUE_FUNC_VARARG(int, fprintf, FILE*, const char*, ...);
FAKE_VOID_FUNC(action, void*);

#define FFF_FAKES_LIST(FAKE)    \
    FAKE(free)                  \
    FAKE(fprintf)               \
    FAKE(action)

ArrayList* array_list;

void setUp() {
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    array_list = array_list_new(DEFAULT_OPTIONS);
}

void tearDown() {
    if (array_list) array_list_delete(array_list);
}

void test_create_array_list() {
    TEST_ASSERT_NOT_NULL(array_list);
}

void test_create_array_list_from_collection() {
    // given
    constexpr int SIZE = 5;
    int values[SIZE] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    ArrayList* new_array_list = array_list_from(array_list_to_collection(array_list), DEFAULT_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_array_list);
    for (int i = 0; i < SIZE; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) array_list_get(new_array_list, i));
    }
}

void test_delete_array_list() {
    // when
    array_list_delete(array_list);
    // then
    TEST_ASSERT_EQUAL(array_list, free_fake.arg0_val);
    // and (clean up)
    array_list = nullptr; // Setting pointer to null to prevent double delete call
}

void test_add_element_to_array_list() {
    // given
    int value = 10;
    // when
    array_list_add(array_list, &value);
    // then
    int expected_value = *(int*) array_list_get(array_list, 0);
    TEST_ASSERT_EQUAL(expected_value, value);
}

void test_add_multiple_elements_to_array_list() {
    // given
    int value_a = 10, value_b = 20;
    // when
    array_list_add(array_list, &value_a);
    array_list_add(array_list, &value_b);
    // then
    int expected_value_a = *(int*) array_list_get(array_list, 0);
    int expected_value_b = *(int*) array_list_get(array_list, 1);
    // and
    TEST_ASSERT_EQUAL(expected_value_a, value_a);
    TEST_ASSERT_EQUAL(expected_value_b, value_b);
}

void test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it() {
    // given
    constexpr int SIZE = 11;
    constexpr int NEW_CAPACITY = 20;
    int values[SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    // when
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // then
    TEST_ASSERT_EQUAL(SIZE, array_list_size(array_list));
    TEST_ASSERT_EQUAL(NEW_CAPACITY, array_list_capacity(array_list));
    for (int i = 0; i < SIZE; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) array_list_get(array_list, i));
    }
}

void test_add_element_at_specific_index_to_array_list() {
    // given
    int new_element = 10;
    int values[4] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_add_at(array_list, 2, &new_element);
    // then
    int expected_size = 5;
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_EQUAL(values[0], *(int*) array_list_get(array_list, 0));
    TEST_ASSERT_EQUAL(values[1], *(int*) array_list_get(array_list, 1));
    TEST_ASSERT_EQUAL(new_element, *(int*) array_list_get(array_list, 2));
    TEST_ASSERT_EQUAL(values[2], *(int*) array_list_get(array_list, 3));
    TEST_ASSERT_EQUAL(values[3], *(int*) array_list_get(array_list, 4));
}

void test_add_element_at_specific_index_to_array_list_exceeding_capacity_resize_it() {
    // given
    constexpr int SIZE = 11;
    constexpr int NEW_CAPACITY = 20;
    int values[SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int index = 7;
    // and
    for (int i = 0; i < SIZE - 1; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_add_at(array_list, index, &values[11]);
    // then
    TEST_ASSERT_EQUAL(SIZE, array_list_size(array_list));
    TEST_ASSERT_EQUAL(NEW_CAPACITY, array_list_capacity(array_list));
    for (int i = 0; i < index; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) array_list_get(array_list, i));
    }
    TEST_ASSERT_EQUAL(values[11], *(int*) array_list_get(array_list, index));
    for (int i = index; i < SIZE - 1; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) array_list_get(array_list, i + 1));
    }
}

void add_out_of_bounds(int index) {
    // given
    const char* message = "Warning: array_list_add_at index %d out of bounds\n";
    int value = 10;
    // when
    array_list_add_at(array_list, index, &value);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_add_element_at_specific_index_to_array_list_out_of_bounds_warns_client() {
    add_out_of_bounds(10);
    add_out_of_bounds(-1);
}

void test_add_all_elements_from_collection_to_array_list() {
    // given
    ArrayList* existing_array_list = array_list_new(DEFAULT_OPTIONS);
    constexpr int SIZE = 5;
    int values[SIZE] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(existing_array_list, &values[i]);
    }
    // when
    array_list_add_all(array_list, array_list_to_collection(existing_array_list));
    // then
    TEST_ASSERT_EQUAL(SIZE, array_list_size(array_list));
    for (int i = 0; i < SIZE; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) array_list_get(array_list, i));
    }
}

void test_add_all_elements_from_collection_to_array_list_at_specific_index() {
    // given
    constexpr int SIZE = 5;
    int values[SIZE] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // and
    ArrayList* existing_array_list = array_list_new(DEFAULT_OPTIONS);
    int new_values[] = { 10, 20 , 30};
    array_list_add(existing_array_list, &new_values[0]);
    array_list_add(existing_array_list, &new_values[1]);
    array_list_add(existing_array_list, &new_values[2]);
    // when
    array_list_add_all_at(array_list, 2, array_list_to_collection(existing_array_list));
    // then
    TEST_ASSERT_EQUAL(SIZE + 3, array_list_size(array_list));
    TEST_ASSERT_EQUAL(values[0], *(int*) array_list_get(array_list, 0));
    TEST_ASSERT_EQUAL(values[1], *(int*) array_list_get(array_list, 1));
    // and
    TEST_ASSERT_EQUAL(new_values[0], *(int*) array_list_get(array_list, 2));
    TEST_ASSERT_EQUAL(new_values[1], *(int*) array_list_get(array_list, 3));
    TEST_ASSERT_EQUAL(new_values[2], *(int*) array_list_get(array_list, 4));
    // and
    TEST_ASSERT_EQUAL(values[2], *(int*) array_list_get(array_list, 5));
    TEST_ASSERT_EQUAL(values[3], *(int*) array_list_get(array_list, 6));
    TEST_ASSERT_EQUAL(values[4], *(int*) array_list_get(array_list, 7));
    // clean up
    array_list_delete(existing_array_list);
}

void test_get_element_from_array_list() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    int actual_value = *(int*) array_list_get(array_list, 0);
    // then
    TEST_ASSERT_EQUAL(value, actual_value);
}

void get_out_of_bounds(int index) {
    // given
    const char* message = "Warning: array_list_get index %d out of bounds\n";
    int value = 10;
    array_list_add(array_list, &value);
    // when
    void* element = array_list_get(array_list, index);
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_get_element_from_array_list_index_out_of_bounds_warns_client() {
    get_out_of_bounds(10);
    get_out_of_bounds(-1);
}

void test_set_element_of_array_list() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    int new_value = 20;
    array_list_set(array_list, 0, &new_value);
    // then
    int actual_value = *(int*) array_list_get(array_list, 0);
    TEST_ASSERT_EQUAL(new_value, actual_value);
}

void set_out_of_bounds(int index) {
    // given
    const char* message = "Warning: array_list_set index %d out of bounds\n";
    int value = 20;
    // when
    array_list_set(array_list, index, &value);
    // then
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_set_element_of_array_list_index_out_of_bounds_warns_client() {
    set_out_of_bounds(10);
    set_out_of_bounds(-1);
}

void test_remove_element_from_array_list() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    array_list_remove(array_list, 0);
    // then
    int expected_size = 0;
    void* actual_value = array_list_get(array_list, 0);
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_NULL(actual_value);
}

void test_remove_element_from_array_list_shifts_its_remaining_elements() {
    // given
    int values[4] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_remove(array_list, 2);
    // then
    TEST_ASSERT_EQUAL(values[0], *(int*) array_list_get(array_list, 0));
    TEST_ASSERT_EQUAL(values[1], *(int*) array_list_get(array_list, 1));
    TEST_ASSERT_EQUAL(values[3], *(int*) array_list_get(array_list, 2));
}

void remove_out_of_bounds(int index) {
    // given
    const char* message = "Warning: array_list_remove index %d out of bounds\n";
    // when
    array_list_remove(array_list, index);
    // then
    int expected_size = 0;
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_remove_element_from_array_list_index_out_of_bounds_warns_client() {
    remove_out_of_bounds(10);
    remove_out_of_bounds(-1);
}

void test_remove_element_from_array_list_by_reference() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    array_list_remove_element(array_list, &value);
    // then
    int expected_size = 0;
    void* actual_value = array_list_get(array_list, 0);
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_NULL(actual_value);
}

void test_array_list_is_not_empty() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    bool empty = array_list_is_empty(array_list);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_array_list_is_empty() {
    // when
    bool empty = array_list_is_empty(array_list);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_array_list_iterator() {
    // given
    int values[3] = { 0, 1, 2 };
    for (int i = 0; i < 3; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    Iterator* iterator = array_list_iterator(array_list);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(values[0], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(values[1], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(values[2], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_NULL(iterator_next(iterator));
}

void test_perform_action_for_each_element_of_array_list() {
    // given
    int values[5] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_for_each(array_list, action);
    // then
    TEST_ASSERT_EQUAL(5, action_fake.call_count);
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) action_fake.arg0_history[i]);
    }
}

void test_sort_array_list() {
    // given
    int values[5] = { 3, 1, 0, 4, 2 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_sort(array_list, DEFAULT_COMPARATOR, BUBBLE_SORT);
    // then
    TEST_ASSERT_EQUAL(values[0], *(int*) array_list_get(array_list, 0));
    TEST_ASSERT_EQUAL(values[1], *(int*) array_list_get(array_list, 1));
    TEST_ASSERT_EQUAL(values[2], *(int*) array_list_get(array_list, 2));
    TEST_ASSERT_EQUAL(values[3], *(int*) array_list_get(array_list, 3));
    TEST_ASSERT_EQUAL(values[4], *(int*) array_list_get(array_list, 4));
}

void test_clear_array_list() {
    // given
    int values[3] = { 0, 1, 2 };
    for (int i = 0; i < 3; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_clear(array_list);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_NULL(array_list_get(array_list, 0));
    TEST_ASSERT_NULL(array_list_get(array_list, 1));
    TEST_ASSERT_NULL(array_list_get(array_list, 2));
}

void test_array_list_contains_element() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    bool contains = array_list_contains(array_list, &value);
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_array_list_does_not_contains_element() {
    // given
    int value = 10;
    // when
    bool contains = array_list_contains(array_list, &value);
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_array_list_index_of_element_returns_its_index() {
    // given
    int values[3] = { 0, 1, 2 };
    for (int i = 0; i < 3; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    int index = array_list_index_of(array_list, &values[1]);
    // then
    TEST_ASSERT_EQUAL(1, index);
}

void test_array_list_index_of_nonexistent_element_returns_negative_one() {
    // given
    int nonexistent = 10;
    int values[3] = { 0, 1, 2 };
    for (int i = 0; i < 3; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    int index = array_list_index_of(array_list, &nonexistent);
    // then
    TEST_ASSERT_EQUAL(-1, index);
}

void test_array_list_sub_list() {
    // given
    int values[5] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    ArrayList* new_array_list = array_list_sub_list(array_list, 1, 3);
    // then
    TEST_ASSERT_NOT_NULL(new_array_list);
    TEST_ASSERT_EQUAL(values[1], *(int*) array_list_get(new_array_list, 0));
    TEST_ASSERT_EQUAL(values[2], *(int*) array_list_get(new_array_list, 1));
    TEST_ASSERT_EQUAL(values[3], *(int*) array_list_get(new_array_list, 2));
    TEST_ASSERT_NULL(array_list_get(new_array_list, 3));
}

void test_array_list_sub_list_index_out_of_bounds_returns_null() {
    // given
    int values[5] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    // then
    TEST_ASSERT_NULL(array_list_sub_list(array_list, -1, 4));
    TEST_ASSERT_NULL(array_list_sub_list(array_list, 0, 6));
    // and
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_NOT_NULL(fprintf_fake.arg1_val);
    TEST_ASSERT_EQUAL_STRING("Warning: array_list_sub_list start_index %d out of bounds\n", fprintf_fake.arg1_history[0]);
    TEST_ASSERT_EQUAL_STRING("Warning: array_list_sub_list end_index %d out of bounds\n", fprintf_fake.arg1_history[1]);
}

void test_array_list_sub_list_start_index_greater_than_end_index_returns_null() {
    // given
    const char* message = "Warning: array_list_sub_list start_index %d greater than end_index %d\n";
    int values[5] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    ArrayList* new_array_list = array_list_sub_list(array_list, 4, 3);
    // then
    TEST_ASSERT_NULL(new_array_list);
    // and
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_array_list_to_collection() {
    // when
    Collection collection = array_list_to_collection(array_list);
    // then
    TEST_ASSERT_EQUAL(array_list, collection.data_structure);
    TEST_ASSERT_EQUAL(array_list_size, collection.size);
    TEST_ASSERT_EQUAL(array_list_iterator, collection.iterator);
}

void test_array_list_to_string() {
    // given
    char expected_string[68];
    int values[4] = { 0, 1, 2, 3 };
    // and
    sprintf(expected_string, "[ %p, %p, %p, %p ]", &values[0], &values[1], &values[2], &values[3]);
    for (int i = 0; i < 4; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    char* string = array_list_to_string(array_list);
    // then
    TEST_ASSERT_EQUAL_STRING(expected_string, string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_array_list);
    RUN_TEST(test_create_array_list_from_collection);
    RUN_TEST(test_delete_array_list);
    RUN_TEST(test_add_element_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it);
    RUN_TEST(test_add_element_at_specific_index_to_array_list);
    RUN_TEST(test_add_element_at_specific_index_to_array_list_exceeding_capacity_resize_it);
    RUN_TEST(test_add_element_at_specific_index_to_array_list_out_of_bounds_warns_client);
    RUN_TEST(test_add_all_elements_from_collection_to_array_list);
    RUN_TEST(test_add_all_elements_from_collection_to_array_list_at_specific_index);
    RUN_TEST(test_get_element_from_array_list);
    RUN_TEST(test_get_element_from_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_set_element_of_array_list);
    RUN_TEST(test_set_element_of_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_remove_element_from_array_list);
    RUN_TEST(test_remove_element_from_array_list_shifts_its_remaining_elements);
    RUN_TEST(test_remove_element_from_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_remove_element_from_array_list_by_reference);
    RUN_TEST(test_array_list_is_not_empty);
    RUN_TEST(test_array_list_is_empty);
    RUN_TEST(test_array_list_iterator);
    RUN_TEST(test_perform_action_for_each_element_of_array_list);
    RUN_TEST(test_sort_array_list);
    RUN_TEST(test_clear_array_list);
    RUN_TEST(test_array_list_contains_element);
    RUN_TEST(test_array_list_does_not_contains_element);
    RUN_TEST(test_array_list_index_of_element_returns_its_index);
    RUN_TEST(test_array_list_index_of_nonexistent_element_returns_negative_one);
    RUN_TEST(test_array_list_sub_list);
    RUN_TEST(test_array_list_sub_list_index_out_of_bounds_returns_null);
    RUN_TEST(test_array_list_sub_list_start_index_greater_than_end_index_returns_null);
    RUN_TEST(test_array_list_to_collection);
    RUN_TEST(test_array_list_to_string);
    return UNITY_END();
}