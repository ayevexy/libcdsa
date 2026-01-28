#include "linked_list_test.h"

#include "list/linked_list.h"
#include "util/errors.h"

#include "unity.h"
#include <stdlib.h>

static LinkedList* linked_list;

void setUp() {
    linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS(
        .equals = (bool (*)(const void*, const void*)) int_equals,
        .to_string = (int (*)(const void*, char*, size_t)) int_to_string,
    ));
}

void tearDown() {
    linked_list_delete(&linked_list);
}

void test_create_linked_list() {
    TEST_ASSERT_NOT_NULL(linked_list);
}

void test_do_not_create_linked_list_with_invalid_options() {
    // when
    LinkedList* new_linked_list; Error error = attempt(new_linked_list = linked_list_new(&(LinkedListOptions) {
        .equals = nullptr,
        .to_string = nullptr
    }));
    // then
    TEST_ASSERT_NULL(new_linked_list);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_linked_list_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    LinkedList* new_linked_list = linked_list_from(linked_list_to_collection(linked_list), DEFAULT_LINKED_LIST_OPTIONS());
    // then
    TEST_ASSERT_NOT_NULL(new_linked_list);
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, new_linked_list);
    // clean up
    linked_list_delete(&new_linked_list);
}

void test_do_not_create_linked_list_with_invalid_options_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    LinkedList* new_linked_list; Error error = attempt(new_linked_list = linked_list_from(linked_list_to_collection(linked_list), &(LinkedListOptions) {
        .equals = nullptr,
        .to_string = nullptr
    }));
    // then
    TEST_ASSERT_NULL(new_linked_list);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_delete_linked_list_set_it_to_null() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // when
    linked_list_delete(&new_linked_list);
    // then
    TEST_ASSERT_NULL(new_linked_list);
}

void test_delete_null_linked_list_fails() {
    // given
    LinkedList* new_linked_list = nullptr;
    // when
    Error error = attempt(linked_list_delete(&new_linked_list));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_at_index_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_add(linked_list, 2, &(int){10});
    // then
    int new_values[] = { 1, 2, 10, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

// Edge case
void test_add_element_at_index_0_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_add(linked_list, 0, &(int){10});
    // then
    int new_values[] = { 10, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

// Edge case
void test_add_element_at_index_equal_size_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_add(linked_list, 5, &(int){10});
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

static void add_index_out_of_bounds_test_helper(int index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Error error = attempt(linked_list_add(linked_list, index, &(int){10}));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_add_element_at_index_to_linked_list_index_above_bounds_fails() {
    add_index_out_of_bounds_test_helper(10);
}

void test_add_element_at_index_to_linked_list_negative_index_fails() {
    add_index_out_of_bounds_test_helper(-1);
}

void test_add_element_at_beginning_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_add_first(linked_list, &(int){10});
    // then
    int new_values[] = { 10, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

void test_add_element_at_end_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_add_last(linked_list, &(int){10});
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

void test_add_all_elements_from_collection_at_index_in_linked_list() {
    // given
    LinkedList* existing_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_LINKED_LIST(existing_linked_list, other_values);

    // when
    linked_list_add_all(linked_list, 2, linked_list_to_collection(existing_linked_list));

    // then
    int new_values[] = { 1, 2, 10, 20, 30, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);

    // clean up
    linked_list_delete(&existing_linked_list);
}

void test_add_all_elements_from_collection_at_beginning_of_linked_list() {
    // given
    LinkedList* existing_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_LINKED_LIST(existing_linked_list, other_values);

    // when
    linked_list_add_all_first(linked_list, linked_list_to_collection(existing_linked_list));

    // then
    int new_values[] = { 10, 20, 30, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);

    // clean up
    linked_list_delete(&existing_linked_list);
}

void test_add_all_elements_from_collection_at_end_of_linked_list() {
    // given
    LinkedList* existing_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_LINKED_LIST(existing_linked_list, other_values);

    // when
    linked_list_add_all_last(linked_list, linked_list_to_collection(existing_linked_list));

    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10, 20, 30 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);

    // clean up
    linked_list_delete(&existing_linked_list);
}

void test_get_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int actual_value = *(int*) linked_list_get(linked_list, 2);
    // then
    TEST_ASSERT_EQUAL(3, actual_value);
}

static void get_index_out_of_bounds_test_helper(int index) {
    // given
    linked_list_add_last(linked_list, &(int){10});
    // when
    void* element; Error error = attempt(element = linked_list_get(linked_list, index));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_get_element_from_linked_list_index_above_bounds_fails() {
    get_index_out_of_bounds_test_helper(10);
}

void test_get_element_from_linked_list_negative_index_fails() {
    get_index_out_of_bounds_test_helper(-1);
}

void test_get_first_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int actual_value = *(int*) linked_list_get_first(linked_list);
    // then
    TEST_ASSERT_EQUAL(1, actual_value);
}

void test_get_first_element_from_empty_linked_list_fails() {
    // when
    void* element; Error error = attempt(element = linked_list_get_first(linked_list));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_last_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int actual_value = *(int*) linked_list_get_last(linked_list);
    // then
    TEST_ASSERT_EQUAL(5, actual_value);
}

void test_get_last_element_from_empty_linked_list_fails() {
    // when
    void* element; Error error = attempt(element = linked_list_get_last(linked_list));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_set_element_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* old_value = linked_list_set(linked_list, 2, &(int){10});
    // then
    int new_values[] = { 1, 2, 10, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(3, *old_value);
}

static void set_index_out_of_bounds_test_helper(int index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* old_value; Error error = attempt(old_value = linked_list_set(linked_list, index, &(int){10}));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_set_element_of_linked_list_index_above_bounds_fails() {
    set_index_out_of_bounds_test_helper(10);
}

void test_set_element_of_linked_list_negative_index_fails() {
    set_index_out_of_bounds_test_helper(-1);
}

void test_swap_elements_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_swap(linked_list, 1, 3);
    // then
    int swaped_values[] = { 1, 4, 3, 2, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(swaped_values, linked_list);
}

static void swap_elements_of_linked_list_index_out_of_bounds_test_helper(int index_a, int index_b) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Error error = attempt(linked_list_swap(linked_list, index_a, index_b));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_swap_elements_of_linked_list_index_a_above_bounds_fails() {
    swap_elements_of_linked_list_index_out_of_bounds_test_helper(10, 3);
}

void test_swap_elements_of_linked_list_negative_index_a_fails() {
    swap_elements_of_linked_list_index_out_of_bounds_test_helper(-1, 3);
}

void test_swap_elements_of_linked_list_index_b_above_bounds_fails() {
    swap_elements_of_linked_list_index_out_of_bounds_test_helper(3, 10);
}

void test_swap_elements_of_linked_list_negative_index_b_fails() {
    swap_elements_of_linked_list_index_out_of_bounds_test_helper(3, -1);
}

void test_remove_element_by_index_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* element = linked_list_remove(linked_list, 2);
    // then
    int new_values[] = { 1, 2, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(3, *element);
}

static void remove_index_out_of_bounds_test_helper(int index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* element; Error error = attempt(element = linked_list_remove(linked_list, index));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_remove_element_by_index_from_linked_list_index_above_bounds_fails() {
    remove_index_out_of_bounds_test_helper(10);
}

void test_remove_element_by_index_from_linked_list_negative_index_fails() {
    remove_index_out_of_bounds_test_helper(-1);
}

void test_remove_first_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* element = linked_list_remove_first(linked_list);
    // then
    int new_values[] = { 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(1, *element);
}

void test_remove_first_element_from_empty_linked_list_fails() {
    // when
    void* element; Error error = attempt(element = linked_list_remove_first(linked_list));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_last_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int* element = linked_list_remove_last(linked_list);
    // then
    int new_values[] = { 1, 2, 3, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(5, *element);
}

void test_remove_last_element_from_empty_linked_list_fails() {
    // when
    void* element; Error error = attempt(element = linked_list_remove_last(linked_list));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_element_by_memory_address_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool removed = linked_list_remove_element(linked_list, &values[2]);
    // then
    int new_values[] = { 1, 2, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_TRUE(removed);
}

void test_remove_element_by_memory_address_from_linked_list_nonexistent_element_fails() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool removed = linked_list_remove_element(linked_list, &(int){10});
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_FALSE(removed);
}

void test_remove_all_elements_from_linked_list_matching_collection() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int sub_values[] = { 2, 3, 4 };
    POPULATE_LINKED_LIST(new_linked_list, sub_values);

    // when
    int count = linked_list_remove_all(linked_list, linked_list_to_collection(new_linked_list));

    // then
    int new_values[] = { 1, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(3, count);

    // clean up
    linked_list_delete(&new_linked_list);
}

void test_remove_elements_in_range_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int count = linked_list_remove_range(linked_list, 1, 4);
    // then
    int new_values[] = { 1, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(3, count);
}

static void remove_elements_in_range_index_out_of_bounds_test_helper(int start_index, int end_index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int count; Error error = attempt(count = linked_list_remove_range(linked_list, start_index, end_index));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_EQUAL(0, count);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_remove_elements_in_range_from_linked_list_end_index_above_bounds_fails() {
    remove_elements_in_range_index_out_of_bounds_test_helper(0, 10);
}

void test_remove_elements_in_range_from_linked_list_negative_start_index_fails() {
    remove_elements_in_range_index_out_of_bounds_test_helper(-1, 3);
}

void test_remove_elements_in_range_from_linked_list_start_index_greater_than_end_index_fails() {
    remove_elements_in_range_index_out_of_bounds_test_helper(4, 3);
}

static bool is_odd(const void* element) {
    return *(int *) element % 2 != 0;
}

void test_remove_elements_from_linked_list_matching_predicate() {
    // given
    int values[] = { 1, 2, 3, 3, 4, 5 }; // the duplicated 3 ensure it will not be skipped
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int count = linked_list_remove_if(linked_list, is_odd);
    // then
    int new_values[] = { 2, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(4, count);
}

static void* replace_by_2_times_original(void* element) {
    int* value = malloc(sizeof(int));
    *value = *(int*) element * 2;
    // free(element); free original element, since that pointer will be lost now.
    // since element is in the stack, it's not necessary
    // if the element was in the heap, a free call MUST be made here
    return value;
}

void test_replace_all_elements_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_replace_all(linked_list, replace_by_2_times_original);
    // then
    int new_values[] = { 2, 4, 6, 8, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

void test_retain_all_elements_from_collection_in_linked_list() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int new_values[] = { 2, 4 }; // empty value between `2` and `4` to ensure `3` will not be skipped
    POPULATE_LINKED_LIST(new_linked_list, new_values);

    // when
    int count = linked_list_retain_all(linked_list, linked_list_to_collection(new_linked_list));

    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_EQUAL(3, count);
    // clean up
    linked_list_delete(&new_linked_list);
}

void test_get_linked_list_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int size = linked_list_size(linked_list);
    // then
    TEST_ASSERT_EQUAL(size, SIZE(values));
}

void test_linked_list_is_empty() {
    // when
    bool empty = linked_list_is_empty(linked_list);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_linked_list_is_not_empty() {
    // given
    linked_list_add_last(linked_list, &(int){10});
    // when
    bool empty = linked_list_is_empty(linked_list);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_linked_list_iterator() {
    // given
    int values[] = { 1, 2, 3 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Iterator* iterator = linked_list_iterator(linked_list);
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
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_delete(&iterator);
}

void test_linked_list_is_equal_to_it_self() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool equals = linked_list_equals(linked_list, linked_list);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_linked_list_is_equal_to_another_linked_list() {
    // given
    LinkedList* other_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    POPULATE_LINKED_LIST(other_linked_list, values);
    // when
    bool equals = linked_list_equals(linked_list, other_linked_list);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_linked_list_is_not_equal_to_another_linked_list_with_different_size() {
    // given
    LinkedList* other_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    POPULATE_LINKED_LIST(other_linked_list, values);
    linked_list_add_last(other_linked_list, &(int){10});

    // when
    bool equals = linked_list_equals(linked_list, other_linked_list);

    // then
    TEST_ASSERT_FALSE(equals);
}

void test_linked_list_is_not_equal_to_another_linked_list_with_different_elements() {
    // given
    LinkedList* other_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 2, 3, 4, 5, 6 };
    POPULATE_LINKED_LIST(other_linked_list, other_values);

    // when
    bool equals = linked_list_equals(linked_list, other_linked_list);

    // then
    TEST_ASSERT_FALSE(equals);
}

static void action_add_one(void* element) {
    *(int*) element += 1;
}

void test_perform_action_for_each_element_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_for_each(linked_list, action_add_one);
    // then
    int new_values[] = { 2, 3, 4, 5, 6 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
}

static void sort_linked_list_test_helper(SortingAlgorithm sorting_algorithm) {
    // given
    int values[] = { 3, 1, 4, 2, 6, 7, 8, 10, 9, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_sort(linked_list, (Comparator) &compare_ints, sorting_algorithm);
    // then
    int sorted_values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(sorted_values, linked_list);
}

void test_bubble_sort_linked_list() {
    sort_linked_list_test_helper(BUBBLE_SORT);
}

void test_selection_sort_linked_list() {
    sort_linked_list_test_helper(SELECTION_SORT);
}

void test_insertion_sort_linked_list() {
    sort_linked_list_test_helper(INSERTION_SORT);
}

void test_merge_sort_linked_list() {
    sort_linked_list_test_helper(MERGE_SORT);
}

void test_quick_sort_linked_list() {
    sort_linked_list_test_helper(QUICK_SORT);
}

static int random_number(void) {
    static int count = 0; // use a true random number generator here
    return count++;
}

void test_durstenfeld_shuffle_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_shuffle(linked_list, random_number, DURSTENFELD_SHUFFLE);
    // then
    int shuffled_values[] = { 7, 6, 9, 8, 10, 5, 4, 3, 2, 1 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(shuffled_values, linked_list);
}

void test_sattolo_shuffle_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_shuffle(linked_list, random_number, SATTOLO_SHUFFLE);
    // then
    int shuffled_values[] = { 2, 8, 6, 7, 9, 4, 10, 5, 3, 1 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(shuffled_values, linked_list);
}

void test_naive_shuffle_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_shuffle(linked_list, random_number, NAIVE_SHUFFLE);
    // then
    int shuffled_values[] = { 3, 4, 5, 6, 7, 8, 1, 2, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(shuffled_values, linked_list);
}

void test_reverse_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_reverse(linked_list);
    // then
    int reversed_values[] = { 5, 4, 3, 2, 1 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(reversed_values, linked_list);
}

void test_rotate_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_rotate(linked_list, 2);
    // then
    int rotated_values[] = { 4, 5, 1, 2, 3 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(rotated_values, linked_list);
}

void test_rotate_linked_list_backward() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_rotate(linked_list, -2);
    // then
    int rotated_values[] = { 3, 4, 5, 1, 2 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(rotated_values, linked_list);
}

void test_clear_linked_list() {
    // given
    int values[] = { 1, 2, 3 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_clear(linked_list);
    // then
    TEST_ASSERT_EQUAL(0, linked_list_size(linked_list));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(linked_list_get(linked_list, 0)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(linked_list_get(linked_list, 1)));
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(linked_list_get(linked_list, 2)));
}

void test_find_element_matching_predicate_in_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Optional result = linked_list_find(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(1, *(int*) result.value);
    TEST_ASSERT_TRUE(result.present);
}

void test_find_element_matching_predicate_in_linked_list_nonexistent_element_returns_null() {
    // given
    int values[] = { 2, 4, 6, 8, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Optional result = linked_list_find(linked_list, is_odd);
    // then
    TEST_ASSERT_FALSE(result.present);
}

void test_find_last_element_matching_predicate_in_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Optional result = linked_list_find_last(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(5, *(int*) result.value);
    TEST_ASSERT_TRUE(result.present);
}

void test_find_last_element_matching_predicate_in_linked_list_nonexistent_element_returns_null() {
    // given
    int values[] = { 2, 4, 6, 8, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    Optional result = linked_list_find_last(linked_list, is_odd);
    // then
    TEST_ASSERT_FALSE(result.present);
}

void test_get_index_matching_predicate_in_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_index_where(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(0, index);
}

void test_get_index_matching_predicate_in_linked_list_no_matching_returns_negative_one() {
    // given
    int values[] = { 2, 4, 6, 8, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_index_where(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(-1, index);
}

void test_get_last_index_matching_predicate_in_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_last_index_where(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(4, index);
}

void test_get_last_index_matching_predicate_in_linked_list_no_matching_returns_negative_one() {
    // given
    int values[] = { 2, 4, 6, 8, 10 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_last_index_where(linked_list, is_odd);
    // then
    TEST_ASSERT_EQUAL(-1, index);
}

void test_linked_list_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool contains = linked_list_contains(linked_list, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_linked_list_does_not_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool contains = linked_list_contains(linked_list, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_linked_list_contains_all_elements() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 2, 3, 4};
    POPULATE_LINKED_LIST(new_linked_list, other_values);

    // when
    bool contains_all = linked_list_contains_all(linked_list, linked_list_to_collection(new_linked_list));

    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    linked_list_delete(&new_linked_list);
}

void test_empty_linked_list_contains_all_elements_of_empty_collection() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // when
    bool contains_all = linked_list_contains_all(linked_list, linked_list_to_collection(new_linked_list));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    linked_list_delete(&new_linked_list);
}

void test_linked_list_does_not_contains_all_elements() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS());
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // and
    int other_values[] = { 2, 10, 4};
    POPULATE_LINKED_LIST(new_linked_list, other_values);

    // when
    bool contains_all = linked_list_contains_all(linked_list, linked_list_to_collection(new_linked_list));

    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    linked_list_delete(&new_linked_list);
}

void test_get_occurrences_of_element_in_linked_list() {
    // given
    int values[] = { 1, 2, 3, 3, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int count = linked_list_occurrences_of(linked_list, &(int){3});
    // then
    TEST_ASSERT_EQUAL(3, count);
}

void test_get_index_of_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_index_of(linked_list, &(int){3});
    // then
    TEST_ASSERT_EQUAL(2, index);
}

void test_get_index_of_nonexistent_element_from_linked_list_returns_negative_one() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int index = linked_list_index_of(linked_list, &(int){10});
    // then
    TEST_ASSERT_EQUAL(-1, index);
}

void test_get_last_index_of_element_from_linked_list() {
    // given
    int values[] = { 1, 2, 3, 3, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int last_index = linked_list_last_index_of(linked_list, &(int){3});
    // then
    TEST_ASSERT_EQUAL(4, last_index);
}

void test_get_last_index_of_nonexistent_element_from_linked_list_returns_negative_one() {
    // given
    int values[] = { 1, 2, 3, 3, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    int last_index = linked_list_last_index_of(linked_list, &(int){10});
    // then
    TEST_ASSERT_EQUAL(-1, last_index);
}

void test_clone_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    LinkedList* copy_linked_list = linked_list_clone(linked_list);
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, copy_linked_list);
    // clean up
    linked_list_delete(&copy_linked_list);
}

void test_create_sub_list_of_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);

    // when
    LinkedList* new_linked_list = linked_list_sub_list(linked_list, 1, 4);

    // then
    int sub_list_values[] = { 2, 3, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(sub_list_values, new_linked_list);

    // clean up
    linked_list_delete(&new_linked_list);
}

void test_create_empty_sub_list_of_linked_list() {
    // when
    LinkedList* new_linked_list = linked_list_sub_list(linked_list, 0, 0);
    // then
    TEST_ASSERT_EQUAL(0, linked_list_size(new_linked_list));
    // clean up
    linked_list_delete(&new_linked_list);
}

static void sub_list_index_out_of_bounds_test_helper(int start_index, int end_index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    LinkedList* sub_list; Error error = attempt(sub_list = linked_list_sub_list(linked_list, start_index, end_index));
    // then
    TEST_ASSERT_NULL(sub_list);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_create_sub_list_end_index_above_bounds_fails() {
    sub_list_index_out_of_bounds_test_helper(0, 10);
}

void test_create_sub_list_negative_start_index_fails() {
    sub_list_index_out_of_bounds_test_helper(-1, 4);
}

void test_create_sub_list_start_index_greater_than_end_index_fails() {
    sub_list_index_out_of_bounds_test_helper(4, 3);
}

void test_convert_linked_list_to_collection() {
    // when
    Collection collection = linked_list_to_collection(linked_list);
    // then
    TEST_ASSERT_EQUAL(linked_list, collection.data_structure);
    TEST_ASSERT_EQUAL(linked_list_size, collection.size);
    TEST_ASSERT_EQUAL(linked_list_iterator, collection.iterator);
}

void test_convert_linked_list_to_array() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    void** elements = linked_list_to_array(linked_list);
    // then
    TEST_ASSERT_ARRAY_EQUALS(values, elements);
    // clean up
    free(elements);
}

void test_get_linked_list_string_representation() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    char* string = linked_list_to_string(linked_list);
    // then
    TEST_ASSERT_EQUAL_STRING("{ 1 -> 2 -> 3 -> 4 -> 5 }", string);
    // clean up
    free(string);
}

void test_get_empty_linked_list_string_representation() {
    // when
    char* string = linked_list_to_string(linked_list);
    // then
    TEST_ASSERT_EQUAL_STRING("{}", string);
    // clean up
    free(string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_linked_list);
    RUN_TEST(test_do_not_create_linked_list_with_invalid_options);

    RUN_TEST(test_create_linked_list_from_collection);
    RUN_TEST(test_do_not_create_linked_list_with_invalid_options_from_collection);

    RUN_TEST(test_delete_linked_list_set_it_to_null);
    RUN_TEST(test_delete_null_linked_list_fails);

    RUN_TEST(test_add_element_at_index_to_linked_list);
    RUN_TEST(test_add_element_at_index_0_to_linked_list);
    RUN_TEST(test_add_element_at_index_equal_size_to_linked_list);
    RUN_TEST(test_add_element_at_index_to_linked_list_index_above_bounds_fails);
    RUN_TEST(test_add_element_at_index_to_linked_list_negative_index_fails);
    RUN_TEST(test_add_element_at_beginning_of_linked_list);
    RUN_TEST(test_add_element_at_end_of_linked_list);

    RUN_TEST(test_add_all_elements_from_collection_at_index_in_linked_list);
    RUN_TEST(test_add_all_elements_from_collection_at_beginning_of_linked_list);
    RUN_TEST(test_add_all_elements_from_collection_at_end_of_linked_list);

    RUN_TEST(test_get_element_from_linked_list);
    RUN_TEST(test_get_element_from_linked_list_index_above_bounds_fails);
    RUN_TEST(test_get_element_from_linked_list_negative_index_fails);
    RUN_TEST(test_get_first_element_from_linked_list);
    RUN_TEST(test_get_first_element_from_empty_linked_list_fails);
    RUN_TEST(test_get_last_element_from_linked_list);
    RUN_TEST(test_get_last_element_from_empty_linked_list_fails);

    RUN_TEST(test_set_element_of_linked_list);
    RUN_TEST(test_set_element_of_linked_list_index_above_bounds_fails);
    RUN_TEST(test_set_element_of_linked_list_negative_index_fails);

    RUN_TEST(test_swap_elements_of_linked_list);
    RUN_TEST(test_swap_elements_of_linked_list_index_a_above_bounds_fails);
    RUN_TEST(test_swap_elements_of_linked_list_negative_index_a_fails);
    RUN_TEST(test_swap_elements_of_linked_list_index_b_above_bounds_fails);
    RUN_TEST(test_swap_elements_of_linked_list_negative_index_b_fails);

    RUN_TEST(test_remove_element_by_index_from_linked_list);
    RUN_TEST(test_remove_element_by_index_from_linked_list_index_above_bounds_fails);
    RUN_TEST(test_remove_element_by_index_from_linked_list_negative_index_fails);

    RUN_TEST(test_remove_first_element_from_linked_list);
    RUN_TEST(test_remove_first_element_from_empty_linked_list_fails);
    RUN_TEST(test_remove_last_element_from_linked_list);
    RUN_TEST(test_remove_last_element_from_empty_linked_list_fails);

    RUN_TEST(test_remove_element_by_memory_address_from_linked_list);
    RUN_TEST(test_remove_element_by_memory_address_from_linked_list_nonexistent_element_fails);
    RUN_TEST(test_remove_all_elements_from_linked_list_matching_collection);

    RUN_TEST(test_remove_elements_in_range_from_linked_list);
    RUN_TEST(test_remove_elements_in_range_from_linked_list_end_index_above_bounds_fails);
    RUN_TEST(test_remove_elements_in_range_from_linked_list_negative_start_index_fails);
    RUN_TEST(test_remove_elements_in_range_from_linked_list_start_index_greater_than_end_index_fails);

    RUN_TEST(test_remove_elements_from_linked_list_matching_predicate);
    RUN_TEST(test_replace_all_elements_from_linked_list);
    RUN_TEST(test_retain_all_elements_from_collection_in_linked_list);

    RUN_TEST(test_get_linked_list_size);
    RUN_TEST(test_linked_list_is_empty);
    RUN_TEST(test_linked_list_is_not_empty);

    RUN_TEST(test_linked_list_iterator);
    RUN_TEST(test_linked_list_is_equal_to_it_self);
    RUN_TEST(test_linked_list_is_equal_to_another_linked_list);
    RUN_TEST(test_linked_list_is_not_equal_to_another_linked_list_with_different_size);
    RUN_TEST(test_linked_list_is_not_equal_to_another_linked_list_with_different_elements);
    RUN_TEST(test_perform_action_for_each_element_of_linked_list);

    RUN_TEST(test_bubble_sort_linked_list);
    RUN_TEST(test_selection_sort_linked_list);
    RUN_TEST(test_insertion_sort_linked_list);
    RUN_TEST(test_merge_sort_linked_list);
    RUN_TEST(test_quick_sort_linked_list);

    RUN_TEST(test_durstenfeld_shuffle_linked_list);
    RUN_TEST(test_sattolo_shuffle_linked_list);
    RUN_TEST(test_naive_shuffle_linked_list);

    RUN_TEST(test_reverse_linked_list);
    RUN_TEST(test_rotate_linked_list);
    RUN_TEST(test_rotate_linked_list_backward);

    RUN_TEST(test_clear_linked_list);

    RUN_TEST(test_find_element_matching_predicate_in_linked_list);
    RUN_TEST(test_find_element_matching_predicate_in_linked_list_nonexistent_element_returns_null);
    RUN_TEST(test_find_last_element_matching_predicate_in_linked_list);
    RUN_TEST(test_find_last_element_matching_predicate_in_linked_list_nonexistent_element_returns_null);

    RUN_TEST(test_get_index_matching_predicate_in_linked_list);
    RUN_TEST(test_get_index_matching_predicate_in_linked_list_no_matching_returns_negative_one);
    RUN_TEST(test_get_last_index_matching_predicate_in_linked_list);
    RUN_TEST(test_get_last_index_matching_predicate_in_linked_list_no_matching_returns_negative_one);

    RUN_TEST(test_linked_list_contains_element);
    RUN_TEST(test_linked_list_does_not_contains_element);
    RUN_TEST(test_linked_list_contains_all_elements);
    RUN_TEST(test_empty_linked_list_contains_all_elements_of_empty_collection);
    RUN_TEST(test_linked_list_does_not_contains_all_elements);

    RUN_TEST(test_get_occurrences_of_element_in_linked_list);
    RUN_TEST(test_get_index_of_element_from_linked_list);
    RUN_TEST(test_get_index_of_nonexistent_element_from_linked_list_returns_negative_one);
    RUN_TEST(test_get_last_index_of_element_from_linked_list);
    RUN_TEST(test_get_last_index_of_nonexistent_element_from_linked_list_returns_negative_one);

    RUN_TEST(test_clone_linked_list);

    RUN_TEST(test_create_sub_list_of_linked_list);
    RUN_TEST(test_create_empty_sub_list_of_linked_list);
    RUN_TEST(test_create_sub_list_end_index_above_bounds_fails);
    RUN_TEST(test_create_sub_list_negative_start_index_fails);
    RUN_TEST(test_create_sub_list_start_index_greater_than_end_index_fails);

    RUN_TEST(test_convert_linked_list_to_collection);
    RUN_TEST(test_convert_linked_list_to_array);
    RUN_TEST(test_get_linked_list_string_representation);
    RUN_TEST(test_get_empty_linked_list_string_representation);
    UNITY_END();
}