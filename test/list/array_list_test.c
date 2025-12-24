#include "list/array_list.h"

#include "util/iterator.h"

#include "unity.h"
#include "fff.h"
#include <stdlib.h>
#include <stdio.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC_VARARG(int, fprintf, FILE*, const char*, ...);
FAKE_VOID_FUNC(action, void*);

#define FFF_FAKES_LIST(FAKE)    \
    FAKE(fprintf)               \
    FAKE(action)

#define SIZE(array) (sizeof(array) / sizeof(array[0]))

#define POPULATE_ARRAY_LIST(array_list, array)      \
    for (int i = 0; i < SIZE(array); i++) {         \
        array_list_add(array_list, &array[i]);      \
}

#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(array, array_list)                \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) array_list_get(array_list, i));     \
}

ArrayList* array_list;

bool int_equals(void* a, void* b) {
    return *(int*) a == *(int*) b;
}

void setUp() {
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    array_list = array_list_new((Options) {
        .initial_capacity = 10,
        .grow_factor = 2,
        .equals = int_equals,
        .to_string = default_to_string
    });
}

void tearDown() {
    array_list_delete(array_list);
}

void test_create_array_list() {
    TEST_ASSERT_NOT_NULL(array_list);
}

void test_create_array_list_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    ArrayList* new_array_list = array_list_from(array_list_to_collection(array_list), DEFAULT_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_array_list);
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, new_array_list);
    // clean up
    array_list_delete(new_array_list);
}

void test_add_element_to_array_list() {
    // given
    int value = 10;
    // when
    array_list_add(array_list, &value);
    // then
    TEST_ASSERT_EQUAL(value, *(int*) array_list_get(array_list, 0));
}

void test_add_multiple_elements_to_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5};
    // when
    POPULATE_ARRAY_LIST(array_list, values);
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, array_list);
}

void test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    // when
    POPULATE_ARRAY_LIST(array_list, values);
    // then
    TEST_ASSERT_EQUAL(SIZE(values), array_list_size(array_list));
    TEST_ASSERT_EQUAL(20, array_list_capacity(array_list));
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, array_list);
}

void test_add_element_at_index_to_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_add_at(array_list, 2, &(int){10});
    // then
    TEST_ASSERT_EQUAL(SIZE(values) + 1, array_list_size(array_list));
    // and
    int new_values[] = { 1, 2, 10, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

void test_add_element_at_index_to_array_list_exceeding_capacity_resize_it() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_add_at(array_list, 7, &(int){100});
    // then
    TEST_ASSERT_EQUAL(SIZE(values) + 1, array_list_size(array_list));
    TEST_ASSERT_EQUAL(20, array_list_capacity(array_list));
    // and
    int new_values[] = { 1, 2, 3, 4, 5, 6, 7, 100, 8, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void add_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_add_at index %d out of bounds\n";
    // when
    array_list_add_at(array_list, index, &(int){10});
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_add_element_at_index_to_array_list_index_above_bounds_warns_client() {
    add_index_out_of_bounds_test_helper(10);
}

void test_add_element_at_index_to_array_list_negative_index_warns_client() {
    add_index_out_of_bounds_test_helper(-1);
}

void test_add_all_elements_from_collection_to_array_list() {
    // given
    ArrayList* existing_array_list = array_list_new(DEFAULT_OPTIONS);
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(existing_array_list, values);
    // when
    array_list_add_all(array_list, array_list_to_collection(existing_array_list));
    // then
    TEST_ASSERT_EQUAL(SIZE(values), array_list_size(array_list));
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, array_list);
    // clean up
    array_list_delete(existing_array_list);
}

void test_add_all_elements_from_collection_to_array_list_at_index() {
    // given
    ArrayList* existing_array_list = array_list_new(DEFAULT_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_ARRAY_LIST(existing_array_list, other_values);

    // when
    array_list_add_all_at(array_list, 2, array_list_to_collection(existing_array_list));

    // then
    TEST_ASSERT_EQUAL(SIZE(values) + SIZE(other_values), array_list_size(array_list));
    // and
    int new_values[] = { 1, 2, 10, 20, 30, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);

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

static void get_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_get index %d out of bounds\n";
    array_list_add(array_list, &(int){10});
    // when
    void* element = array_list_get(array_list, index);
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_get_element_from_array_list_index_above_bounds_warns_client() {
    get_index_out_of_bounds_test_helper(10);
}

void test_get_element_from_array_list_negative_index_warns_client() {
    get_index_out_of_bounds_test_helper(-1);
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

static void set_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_set index %d out of bounds\n";
    // when
    array_list_set(array_list, index, &(int){10});
    // then
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_set_element_of_array_list_index_above_bounds_warns_client() {
    set_index_out_of_bounds_test_helper(10);
}

void test_set_element_of_array_list_negative_index_warns_client() {
    set_index_out_of_bounds_test_helper(-1);
}

void test_remove_element_from_array_list() {
    // given
    array_list_add(array_list, &(int){10});
    // when
    array_list_remove(array_list, 0);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_NULL(array_list_get(array_list, 0));
}

void test_remove_element_from_array_list_shifts_its_remaining_elements() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_remove(array_list, 2);
    // then
    int new_values[] = { 1, 2, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

void remove_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_remove index %d out of bounds\n";
    // when
    array_list_remove(array_list, index);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_remove_element_from_array_list_index_above_bounds_warns_client() {
    remove_index_out_of_bounds_test_helper(10);
}

void test_remove_element_from_array_list_negative_index_warns_client() {
    remove_index_out_of_bounds_test_helper(-1);
}

void test_remove_element_from_array_list_by_reference() {
    // given
    int value = 10;
    array_list_add(array_list, &value);
    // when
    array_list_remove_element(array_list, &value);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_NULL(array_list_get(array_list, 0));
}

void test_remove_all_elements_from_array_list_matching_collection() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int sub_values[] = { 2, 3, 4 };
    POPULATE_ARRAY_LIST(new_array_list, sub_values);

    // when
    array_list_remove_all(array_list, array_list_to_collection(new_array_list));

    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    // and
    int new_values[] = { 1, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);

    // clean up
    array_list_delete(new_array_list);
}

void test_remove_elements_in_range_from_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_remove_range(array_list, 1, 3);
    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    // and
    int new_values[] = { 1, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

// Edge case
void test_remove_elements_in_range_from_array_list_end_index_equals_last_index() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_remove_range(array_list, 2, 4);
    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    // and
    int new_values[] = { 1, 2 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void remove_elements_in_range_index_out_of_bounds_test_helper(int start_index, int end_index, char* message) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_remove_range(array_list, start_index, end_index);
    // then
    TEST_ASSERT_EQUAL(5, array_list_size(array_list));
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_NOT_NULL(fprintf_fake.arg1_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_remove_elements_in_range_from_array_list_end_index_above_bounds_warns_client() {
    char message[] = "Warning: array_list_remove_range end_index %d out of bounds\n";
    remove_elements_in_range_index_out_of_bounds_test_helper(0, 10, message);
}

void test_remove_elements_in_range_from_array_list_negative_start_index_warns_client() {
    char message[] = "Warning: array_list_remove_range start_index %d out of bounds\n";
    remove_elements_in_range_index_out_of_bounds_test_helper(-1, 3, message);
}

void test_remove_elements_in_range_from_array_list_start_index_greater_than_end_index_warns_client() {
    char message[] = "Warning: array_list_remove_range start_index %d greater than end_index %d\n";
    remove_elements_in_range_index_out_of_bounds_test_helper(4, 3, message);
}

static bool odd_predicate(void* element) {
    return *(int *) element % 2 != 0;
}

void test_remove_elements_from_array_list_matching_predicate() {
    // given
    int values[] = { 1, 2, 3, 3, 4, 5 }; // the duplicated 3 ensure it will not be skipped
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_remove_if(array_list, odd_predicate);
    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    // and
    int new_values[] = { 2, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void* replace_by_2_times_original(void* element) {
    int* value = malloc(sizeof(int));
    *value = *(int*) element * 2;
    // free(element); free original element, since that pointer will be lost now.
    // since element is in the stack, it's not necessary
    // if the element was in the heap, a free call MUST be made here
    return value;
}

void test_replace_all_elements_from_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_replace_all(array_list, replace_by_2_times_original);
    // then
    int new_values[] = { 2, 4, 6, 8, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

void test_retain_all_elements_from_collection_in_array_list() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int new_values[] = { 2, 4 }; // empty value between `2` and `4` to ensure `3` will not be skipped
    POPULATE_ARRAY_LIST(new_array_list, new_values);

    // when
    array_list_retain_all(array_list, array_list_to_collection(new_array_list));

    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);

    // clean up
    array_list_delete(new_array_list);
}

void test_array_list_trim_to_size() {
    // given
    int values[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (int i = 0; i < 11; i++) {
        array_list_add(array_list, &values[i]);
    }
    array_list_remove_range(array_list, 0, 6);
    // when
    array_list_trim_to_size(array_list);
    // then
    TEST_ASSERT_EQUAL(4, array_list_capacity(array_list));
    // and (check if the array list will grow correctly)
    for (int i = 0; i < 4; i++) {
        array_list_add(array_list, &values[i]);
    }
    TEST_ASSERT_EQUAL(8, array_list_capacity(array_list));
}

void test_array_list_ensure_capacity() {
    // when
    array_list_ensure_capacity(array_list, 25);
    // then
    TEST_ASSERT_EQUAL(40, array_list_capacity(array_list));
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

void sort_array_list(SortingAlgorithm sorting_algorithm) {
    // setup
    array_list_delete(array_list);
    array_list = array_list_new(DEFAULT_OPTIONS);
    // given
    constexpr int SIZE = 5;
    int values[SIZE] = { 3, 1, 0, 4, 2 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_sort(array_list, DEFAULT_COMPARATOR(int), sorting_algorithm);
    // then
    for (int i = 0; i < SIZE; i++) {
        TEST_ASSERT_EQUAL(i, *(int*) array_list_get(array_list, i));
    }
}

void test_sort_array_list() {
    sort_array_list(BUBBLE_SORT);
    sort_array_list(SELECTION_SORT);
    sort_array_list(INSERTION_SORT);
    sort_array_list(MERGE_SORT);
    sort_array_list(QUICK_SORT);
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

void test_array_list_contains_all_elements() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_OPTIONS);
    int values[5] = { 0, 1, 2, 3, 4 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    array_list_add(new_array_list, &values[1]);
    array_list_add(new_array_list, &values[2]);
    array_list_add(new_array_list, &values[3]);
    // when
    bool contains = array_list_contains_all(array_list, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_TRUE(contains);
    // clean up
    array_list_delete(new_array_list);
}

void test_array_list_does_not_contains_all_elements() {
    // given
    int new_value = 10;
    ArrayList* new_array_list = array_list_new(DEFAULT_OPTIONS);
    int values[5] = { 0, 1, 2, 3, 4 };
    for (int i = 0; i < 5; i++) {
        array_list_add(array_list, &values[i]);
    }
    array_list_add(new_array_list, &values[1]);
    array_list_add(new_array_list, &new_value);
    array_list_add(new_array_list, &values[3]);
    // when
    bool contains = array_list_contains_all(array_list, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_FALSE(contains);
    // clean up
    array_list_delete(new_array_list);
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

void test_array_list_last_index_of_element_returns_its_index() {
    // given
    int values[3] = { 0, 1, 2 };
    array_list_add(array_list, &values[0]);
    array_list_add(array_list, &values[1]);
    array_list_add(array_list, &values[1]);
    array_list_add(array_list, &values[1]);
    array_list_add(array_list, &values[2]);
    // when
    int index = array_list_last_index_of(array_list, &values[1]);
    // then
    TEST_ASSERT_EQUAL(3, index);
}

void test_array_list_last_index_of_nonexistent_element_returns_negative_one() {
    // given
    int nonexistent = 10;
    int values[3] = { 0, 1, 2 };
    for (int i = 0; i < 3; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    int last_index = array_list_last_index_of(array_list, &nonexistent);
    // then
    TEST_ASSERT_EQUAL(-1, last_index);
}

void test_array_list_binary_search() {
    // given
    int values[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < 10; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    int index = array_list_binary_search(array_list, &values[8], DEFAULT_COMPARATOR(int));
    // then
    TEST_ASSERT_EQUAL(8, index);
}

void test_array_list_clone() {
    // given
    constexpr int SIZE = 5;
    int values[SIZE] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    ArrayList* array_list_copy = array_list_clone(array_list);
    // then
    for (int i = 0; i < SIZE; i++) {
        void* original = array_list_get(array_list, i);
        void* copy = array_list_get(array_list_copy, i);
        TEST_ASSERT_EQUAL(original, copy);
        TEST_ASSERT_EQUAL(*(int*) original, *(int*) copy);
    }
    // clean up
    array_list_delete(array_list_copy);
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
    // clean up
    array_list_delete(new_array_list);
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

void test_array_list_to_array() {
    // given
    constexpr int SIZE = 5;
    int values[SIZE] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < SIZE; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    void** elements = array_list_to_array(array_list);
    // then
    for (int i = 0; i < SIZE; i++) {
        TEST_ASSERT_EQUAL(values[i], *(int*) elements[i]);
    }
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

void test_array_list_to_string_different_format() {
    // setup
    array_list = array_list_new((Options) {
        .initial_capacity = 10,
        .grow_factor = 2,
        .equals = default_equals,
        .to_string = DEFAULT_TO_STRING(int)
    });
    // given
    char expected_string[15];
    int values[4] = { 0, 1, 2, 3 };
    // and
    sprintf(expected_string, "[ %d, %d, %d, %d ]", values[0], values[1], values[2], values[3]);
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

    RUN_TEST(test_add_element_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it);

    RUN_TEST(test_add_element_at_index_to_array_list);
    RUN_TEST(test_add_element_at_index_to_array_list_exceeding_capacity_resize_it);
    RUN_TEST(test_add_element_at_index_to_array_list_index_above_bounds_warns_client);
    RUN_TEST(test_add_element_at_index_to_array_list_negative_index_warns_client);

    RUN_TEST(test_add_all_elements_from_collection_to_array_list);
    RUN_TEST(test_add_all_elements_from_collection_to_array_list_at_index);

    RUN_TEST(test_get_element_from_array_list);
    RUN_TEST(test_get_element_from_array_list_index_above_bounds_warns_client);
    RUN_TEST(test_get_element_from_array_list_negative_index_warns_client);

    RUN_TEST(test_set_element_of_array_list);
    RUN_TEST(test_set_element_of_array_list_index_above_bounds_warns_client);
    RUN_TEST(test_set_element_of_array_list_negative_index_warns_client);

    RUN_TEST(test_remove_element_from_array_list);
    RUN_TEST(test_remove_element_from_array_list_shifts_its_remaining_elements);
    RUN_TEST(test_remove_element_from_array_list_index_above_bounds_warns_client);
    RUN_TEST(test_remove_element_from_array_list_negative_index_warns_client);

    RUN_TEST(test_remove_element_from_array_list_by_reference);

    RUN_TEST(test_remove_all_elements_from_array_list_matching_collection);

    RUN_TEST(test_remove_elements_in_range_from_array_list);
    RUN_TEST(test_remove_elements_in_range_from_array_list_end_index_equals_last_index);
    RUN_TEST(test_remove_elements_in_range_from_array_list_end_index_above_bounds_warns_client);
    RUN_TEST(test_remove_elements_in_range_from_array_list_negative_start_index_warns_client);
    RUN_TEST(test_remove_elements_in_range_from_array_list_start_index_greater_than_end_index_warns_client);

    RUN_TEST(test_remove_elements_from_array_list_matching_predicate);

    RUN_TEST(test_replace_all_elements_from_array_list);
    RUN_TEST(test_retain_all_elements_from_collection_in_array_list);

    RUN_TEST(test_array_list_trim_to_size);
    RUN_TEST(test_array_list_ensure_capacity);

    RUN_TEST(test_array_list_is_not_empty);
    RUN_TEST(test_array_list_is_empty);

    RUN_TEST(test_array_list_iterator);

    RUN_TEST(test_perform_action_for_each_element_of_array_list);

    RUN_TEST(test_sort_array_list);

    RUN_TEST(test_clear_array_list);

    RUN_TEST(test_array_list_contains_element);
    RUN_TEST(test_array_list_does_not_contains_element);

    RUN_TEST(test_array_list_contains_all_elements);
    RUN_TEST(test_array_list_does_not_contains_all_elements);

    RUN_TEST(test_array_list_index_of_element_returns_its_index);
    RUN_TEST(test_array_list_index_of_nonexistent_element_returns_negative_one);

    RUN_TEST(test_array_list_last_index_of_element_returns_its_index);
    RUN_TEST(test_array_list_last_index_of_nonexistent_element_returns_negative_one);

    RUN_TEST(test_array_list_binary_search);

    RUN_TEST(test_array_list_clone);

    RUN_TEST(test_array_list_sub_list);
    RUN_TEST(test_array_list_sub_list_index_out_of_bounds_returns_null);
    RUN_TEST(test_array_list_sub_list_start_index_greater_than_end_index_returns_null);

    RUN_TEST(test_array_list_to_collection);
    RUN_TEST(test_array_list_to_array);
    RUN_TEST(test_array_list_to_string);
    RUN_TEST(test_array_list_to_string_different_format);
    return UNITY_END();
}