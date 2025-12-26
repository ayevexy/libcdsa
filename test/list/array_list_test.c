#include "list/array_list.h"

#include "util/iterator.h"

#include "unity.h"
#include "fff.h"
#include <stdlib.h>
#include <stdio.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC_VARARG(int, fprintf, FILE*, const char*, ...);

#define SIZE(array) (sizeof(array) / sizeof(array[0]))

#define POPULATE_ARRAY_LIST(array_list, array)          \
    for (int i = 0; i < SIZE(array); i++) {             \
        array_list_add_last(array_list, &array[i]);     \
}

#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(array, array_list)                \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) array_list_get(array_list, i));     \
}

#define TEST_ASSERT_ARRAY_EQUALS(array_a, array_b)              \
    for (int i = 0; i < SIZE(array_a); i++) {                   \
        TEST_ASSERT_EQUAL(array_a[i], *(int*) array_b[i]);      \
    }

ArrayList* array_list;

void setUp() {
    RESET_FAKE(fprintf);
    FFF_RESET_HISTORY();
    array_list = array_list_new((ArrayListOptions) {
        .initial_capacity = 10,
        .grow_factor = 2,
        .equals = DEFAULT_EQUALS(int),
        .to_string = DEFAULT_TO_STRING(int)
    });
}

void tearDown() {
    array_list_delete(array_list);
}

void test_create_array_list() {
    TEST_ASSERT_NOT_NULL(array_list);
}

void test_do_not_create_array_list_with_invalid_options() {
    // given
    ArrayListOptions invalid_options = {
        .initial_capacity = 0,
        .grow_factor = -1,
        .equals = nullptr,
        .to_string = nullptr
    };
    // when
    ArrayList* new_array_list = array_list_new(invalid_options);
    // then
    TEST_ASSERT_NULL(new_array_list);
}

void test_create_array_list_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    ArrayList* new_array_list = array_list_from(array_list_to_collection(array_list), DEFAULT_ARRAY_LIST_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_array_list);
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, new_array_list);
    // clean up
    array_list_delete(new_array_list);
}

void test_do_not_create_array_list_with_invalid_options_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    ArrayList* new_array_list = array_list_from(array_list_to_collection(array_list), (ArrayListOptions) {
        .initial_capacity = 0,
        .grow_factor = -1,
        .equals = nullptr,
        .to_string = nullptr
    });
    // then
    TEST_ASSERT_NULL(new_array_list);
}

void test_add_element_to_array_list() {
    // given
    int value = 10;
    // when
    array_list_add_last(array_list, &value);
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
    array_list_add(array_list, 2, &(int){10});
    // then
    TEST_ASSERT_EQUAL(SIZE(values) + 1, array_list_size(array_list));
    // and
    int new_values[] = { 1, 2, 10, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

// Edge case
void test_add_element_at_index_to_array_list_index_equals_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_add(array_list, 5, &(int){10});
    // then
    TEST_ASSERT_EQUAL(SIZE(values) + 1, array_list_size(array_list));
    // and
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

void test_add_element_at_index_to_array_list_exceeding_capacity_resize_it() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_add(array_list, 7, &(int){100});
    // then
    TEST_ASSERT_EQUAL(SIZE(values) + 1, array_list_size(array_list));
    TEST_ASSERT_EQUAL(20, array_list_capacity(array_list));
    // and
    int new_values[] = { 1, 2, 3, 4, 5, 6, 7, 100, 8, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void add_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_add index %d out of bounds\n";
    // when
    array_list_add(array_list, index, &(int){10});
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
    ArrayList* existing_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(existing_array_list, values);
    // when
    array_list_add_all_last(array_list, array_list_to_collection(existing_array_list));
    // then
    TEST_ASSERT_EQUAL(SIZE(values), array_list_size(array_list));
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, array_list);
    // clean up
    array_list_delete(existing_array_list);
}

void test_add_all_elements_from_collection_to_array_list_at_index() {
    // given
    ArrayList* existing_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_ARRAY_LIST(existing_array_list, other_values);

    // when
    array_list_add_all(array_list, 2, array_list_to_collection(existing_array_list));

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
    array_list_add_last(array_list, &value);
    // when
    int actual_value = *(int*) array_list_get(array_list, 0);
    // then
    TEST_ASSERT_EQUAL(value, actual_value);
}

static void get_index_out_of_bounds_test_helper(int index) {
    // given
    const char* message = "Warning: array_list_get index %d out of bounds\n";
    array_list_add_last(array_list, &(int){10});
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
    array_list_add_last(array_list, &value);
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

void test_swap_elements_of_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_swap(array_list, 1, 3);
    // then
    int swaped_values[] = { 1, 4, 3, 2, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(swaped_values, array_list);
}

static void swap_elements_of_array_list_index_out_of_bounds_test_helper(int index_a, int index_b) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_swap(array_list, index_a, index_b);
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, array_list);
}

void test_swap_elements_of_array_list_index_a_above_bounds_warns_client() {
    swap_elements_of_array_list_index_out_of_bounds_test_helper(10, 3);
}

void test_swap_elements_of_array_list_negative_index_a_warns_client() {
    swap_elements_of_array_list_index_out_of_bounds_test_helper(-1, 3);
}

void test_swap_elements_of_array_list_index_b_above_bounds_warns_client() {
    swap_elements_of_array_list_index_out_of_bounds_test_helper(3, 10);
}

void test_swap_elements_of_array_list_negative_index_b_warns_client() {
    swap_elements_of_array_list_index_out_of_bounds_test_helper(3, -1);
}

void test_remove_element_from_array_list() {
    // given
    array_list_add_last(array_list, &(int){10});
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
    array_list_add_last(array_list, &value);
    // when
    array_list_remove_element(array_list, &value);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_NULL(array_list_get(array_list, 0));
}

void test_remove_nonexistent_element_from_array_list_by_reference_does_not_warns_client() {
    // when
    array_list_remove_element(array_list, &(int){10});
    // then
    TEST_ASSERT_EQUAL(0, fprintf_fake.call_count);
}

void test_remove_all_elements_from_array_list_matching_collection() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
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
    array_list_remove_range(array_list, 1, 4);
    // then
    TEST_ASSERT_EQUAL(2, array_list_size(array_list));
    // and
    int new_values[] = { 1, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void remove_elements_in_range_index_out_of_bounds_test_helper(int start_index, int end_index) {
    // given
    char message[] = "Warning: array_list_remove_range invalid range: %d to %d\n";
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
    remove_elements_in_range_index_out_of_bounds_test_helper(0, 10);
}

void test_remove_elements_in_range_from_array_list_negative_start_index_warns_client() {
    remove_elements_in_range_index_out_of_bounds_test_helper(-1, 3);
}

void test_remove_elements_in_range_from_array_list_start_index_greater_than_end_index_warns_client() {
    remove_elements_in_range_index_out_of_bounds_test_helper(4, 3);
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
    ArrayList* new_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
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

void test_trim_array_list_capacity_to_match_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    array_list_remove_range(array_list, 0, 5);

    // when
    array_list_trim_to_size(array_list);

    // then
    TEST_ASSERT_EQUAL(10, array_list_capacity(array_list));
    // and
    POPULATE_ARRAY_LIST(array_list, values); // (check if the array list will grow correctly)
    TEST_ASSERT_EQUAL(20, array_list_capacity(array_list));
}

void test_trim_array_list_capacity_to_minimum_capacity_for_size_less_than_2() {
    // when
    array_list_trim_to_size(array_list);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(array_list));
    TEST_ASSERT_EQUAL(10, array_list_capacity(array_list));
}

void test_ensure_capacity_of_array_list() {
    // when
    array_list_ensure_capacity(array_list, 25);
    // then
    TEST_ASSERT_EQUAL(40, array_list_capacity(array_list));
}

void test_array_list_is_empty() {
    // when
    bool empty = array_list_is_empty(array_list);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_array_list_is_not_empty() {
    // given
    array_list_add_last(array_list, &(int){10});
    // when
    bool empty = array_list_is_empty(array_list);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_array_list_iterator() {
    // given
    int values[] = { 1, 2, 3 };
    POPULATE_ARRAY_LIST(array_list, values);
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

static void action_add_one(void* element) {
    *(int*) element += 1;
}

void test_perform_action_for_each_element_of_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_for_each(array_list, action_add_one);
    // then
    int new_values[] = { 2, 3, 4, 5, 6 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(new_values, array_list);
}

static void sort_array_list_test_helper(SortingAlgorithm sorting_algorithm) {
    // given
    int values[] = { 3, 1, 4, 2, 6, 7, 8, 10, 9, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_sort(array_list, DEFAULT_COMPARATOR(int), sorting_algorithm);
    // then
    int sorted_values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(sorted_values, array_list);
}

void test_bubble_sort_array_list() {
    sort_array_list_test_helper(BUBBLE_SORT);
}

void test_selection_sort_array_list() {
    sort_array_list_test_helper(SELECTION_SORT);
}

void test_insertion_sort_array_list() {
    sort_array_list_test_helper(INSERTION_SORT);
}

void test_merge_sort_array_list() {
    sort_array_list_test_helper(MERGE_SORT);
}

void test_quick_sort_array_list() {
    sort_array_list_test_helper(QUICK_SORT);
}

void test_reverse_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    array_list_reverse(array_list);
    // then
    int reversed_values[] = { 5, 4, 3, 2, 1 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(reversed_values, array_list);
}

void test_clear_array_list() {
    // given
    int values[] = { 1, 2, 3 };
    POPULATE_ARRAY_LIST(array_list, values);
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
    array_list_add_last(array_list, &value);
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
    ArrayList* new_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 2, 3, 4};
    POPULATE_ARRAY_LIST(new_array_list, other_values);

    // when
    bool contains_all = array_list_contains_all(array_list, array_list_to_collection(new_array_list));

    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    array_list_delete(new_array_list);
}

void test_empty_array_list_contains_all_elements_of_empty_collection() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
    // when
    bool contains_all = array_list_contains_all(array_list, array_list_to_collection(new_array_list));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    array_list_delete(new_array_list);
}

void test_array_list_does_not_contains_all_elements() {
    // given
    ArrayList* new_array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // and
    int other_values[] = { 2, 10, 4};
    POPULATE_ARRAY_LIST(new_array_list, other_values);

    // when
    bool contains_all = array_list_contains_all(array_list, array_list_to_collection(new_array_list));

    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    array_list_delete(new_array_list);
}

void test_get_index_of_element_from_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    int index = array_list_index_of(array_list, &(int){3});
    // then
    TEST_ASSERT_EQUAL(2, index);
}

void test_get_index_of_nonexistent_element_from_array_list_returns_negative_one() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    int index = array_list_index_of(array_list, &(int){10});
    // then
    TEST_ASSERT_EQUAL(-1, index);
}

void test_get_last_index_of_element_from_array_list() {
    // given
    int values[] = { 1, 2, 3, 3, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    int last_index = array_list_last_index_of(array_list, &(int){3});
    // then
    TEST_ASSERT_EQUAL(4, last_index);
}

void test_get_last_index_of_nonexistent_element_from_array_list_returns_negative_one() {
    // given
    int values[] = { 1, 2, 3, 3, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    int last_index = array_list_last_index_of(array_list, &(int){10});
    // then
    TEST_ASSERT_EQUAL(-1, last_index);
}

void test_binary_search_element_of_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    int index = array_list_binary_search(array_list, &(int){9}, DEFAULT_COMPARATOR(int));
    // then
    TEST_ASSERT_EQUAL(8, index);
}

void test_clone_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    ArrayList* copy_array_list = array_list_clone(array_list);
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(values, copy_array_list);
    // clean up
    array_list_delete(copy_array_list);
}

void test_create_sub_list_of_array_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);

    // when
    ArrayList* new_array_list = array_list_sub_list(array_list, 1, 4);

    // then
    TEST_ASSERT_EQUAL(3, array_list_size(new_array_list));
    // and
    int sub_list_values[] = { 2, 3, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(sub_list_values, new_array_list);

    // clean up
    array_list_delete(new_array_list);
}

void test_create_empty_sub_list_of_array_list() {
    // when
    ArrayList* new_array_list = array_list_sub_list(array_list, 0, 0);
    // then
    TEST_ASSERT_EQUAL(0, array_list_size(new_array_list));
}

static void sub_list_index_out_of_bounds_test_helper(int start_index, int end_index) {
    // given
    char message[] = "Warning: array_list_sub_list invalid range: %d to %d\n";
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    ArrayList* sub_list = array_list_sub_list(array_list, start_index, end_index);
    // then
    TEST_ASSERT_NULL(sub_list);
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);
    TEST_ASSERT_EQUAL_STRING(message, fprintf_fake.arg1_val);
}

void test_create_sub_list_end_index_above_bounds_returns_null_and_warns_client() {
    sub_list_index_out_of_bounds_test_helper(0, 10);
}

void test_create_sub_list_negative_start_index_returns_null_and_warns_client() {
    sub_list_index_out_of_bounds_test_helper(-1, 4);
}

void test_create_sub_list_start_index_greater_than_end_index_returns_null_and_warns_client() {
    sub_list_index_out_of_bounds_test_helper(4, 3);
}

void test_convert_array_list_to_collection() {
    // when
    Collection collection = array_list_to_collection(array_list);
    // then
    TEST_ASSERT_EQUAL(array_list, collection.data_structure);
    TEST_ASSERT_EQUAL(array_list_size, collection.size);
    TEST_ASSERT_EQUAL(array_list_iterator, collection.iterator);
}

void test_convert_array_list_to_array() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    void** elements = array_list_to_array(array_list);
    // then
    TEST_ASSERT_ARRAY_EQUALS(values, elements);
}

void test_get_array_list_string_representation() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_ARRAY_LIST(array_list, values);
    // when
    char* string = array_list_to_string(array_list);
    // then
    TEST_ASSERT_EQUAL_STRING("[ 1, 2, 3, 4, 5 ]", string);
}

void test_get_empty_array_list_string_representation() {
    // when
    char* string = array_list_to_string(array_list);
    // then
    TEST_ASSERT_EQUAL_STRING("[]", string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_array_list);
    RUN_TEST(test_do_not_create_array_list_with_invalid_options);
    RUN_TEST(test_create_array_list_from_collection);
    RUN_TEST(test_do_not_create_array_list_with_invalid_options_from_collection);

    RUN_TEST(test_add_element_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it);

    RUN_TEST(test_add_element_at_index_to_array_list);
    RUN_TEST(test_add_element_at_index_to_array_list_index_equals_size);
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

    RUN_TEST(test_swap_elements_of_array_list);
    RUN_TEST(test_swap_elements_of_array_list_index_a_above_bounds_warns_client);
    RUN_TEST(test_swap_elements_of_array_list_negative_index_a_warns_client);
    RUN_TEST(test_swap_elements_of_array_list_index_b_above_bounds_warns_client);
    RUN_TEST(test_swap_elements_of_array_list_negative_index_b_warns_client);

    RUN_TEST(test_remove_element_from_array_list);
    RUN_TEST(test_remove_element_from_array_list_shifts_its_remaining_elements);
    RUN_TEST(test_remove_element_from_array_list_index_above_bounds_warns_client);
    RUN_TEST(test_remove_element_from_array_list_negative_index_warns_client);

    RUN_TEST(test_remove_element_from_array_list_by_reference);
    RUN_TEST(test_remove_nonexistent_element_from_array_list_by_reference_does_not_warns_client);
    RUN_TEST(test_remove_all_elements_from_array_list_matching_collection);

    RUN_TEST(test_remove_elements_in_range_from_array_list);
    RUN_TEST(test_remove_elements_in_range_from_array_list_end_index_above_bounds_warns_client);
    RUN_TEST(test_remove_elements_in_range_from_array_list_negative_start_index_warns_client);
    RUN_TEST(test_remove_elements_in_range_from_array_list_start_index_greater_than_end_index_warns_client);

    RUN_TEST(test_remove_elements_from_array_list_matching_predicate);
    RUN_TEST(test_replace_all_elements_from_array_list);
    RUN_TEST(test_retain_all_elements_from_collection_in_array_list);

    RUN_TEST(test_trim_array_list_capacity_to_match_size);
    RUN_TEST(test_trim_array_list_capacity_to_minimum_capacity_for_size_less_than_2);
    RUN_TEST(test_ensure_capacity_of_array_list);
    RUN_TEST(test_array_list_is_empty);
    RUN_TEST(test_array_list_is_not_empty);

    RUN_TEST(test_array_list_iterator);
    RUN_TEST(test_perform_action_for_each_element_of_array_list);

    RUN_TEST(test_bubble_sort_array_list);
    RUN_TEST(test_selection_sort_array_list);
    RUN_TEST(test_insertion_sort_array_list);
    RUN_TEST(test_merge_sort_array_list);
    RUN_TEST(test_quick_sort_array_list);

    RUN_TEST(test_reverse_array_list);
    RUN_TEST(test_clear_array_list);

    RUN_TEST(test_array_list_contains_element);
    RUN_TEST(test_array_list_does_not_contains_element);
    RUN_TEST(test_array_list_contains_all_elements);
    RUN_TEST(test_empty_array_list_contains_all_elements_of_empty_collection);
    RUN_TEST(test_array_list_does_not_contains_all_elements);

    RUN_TEST(test_get_index_of_element_from_array_list);
    RUN_TEST(test_get_index_of_nonexistent_element_from_array_list_returns_negative_one);
    RUN_TEST(test_get_last_index_of_element_from_array_list);
    RUN_TEST(test_get_last_index_of_nonexistent_element_from_array_list_returns_negative_one);

    RUN_TEST(test_binary_search_element_of_array_list);

    RUN_TEST(test_clone_array_list);

    RUN_TEST(test_create_sub_list_of_array_list);
    RUN_TEST(test_create_empty_sub_list_of_array_list);
    RUN_TEST(test_create_sub_list_end_index_above_bounds_returns_null_and_warns_client);
    RUN_TEST(test_create_sub_list_negative_start_index_returns_null_and_warns_client);
    RUN_TEST(test_create_sub_list_start_index_greater_than_end_index_returns_null_and_warns_client);

    RUN_TEST(test_convert_array_list_to_collection);
    RUN_TEST(test_convert_array_list_to_array);
    RUN_TEST(test_get_array_list_string_representation);
    RUN_TEST(test_get_empty_array_list_string_representation);
    return UNITY_END();
}