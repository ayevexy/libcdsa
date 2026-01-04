#include "linked_list_test.h"

#include "list/linked_list.h"
#include "util/error.h"

#include "unity.h"
#include <stdlib.h>

static LinkedList* linked_list;

void setUp() {
    linked_list = linked_list_new(&(LinkedListOptions) {
        .equals = DEFAULT_EQUALS(int),
        .to_string = DEFAULT_TO_STRING(int),
        .memory_alloc = malloc,
        .memory_realloc = realloc,
        .memory_free = free
    });
}

void tearDown() {
    linked_list_delete(&linked_list);
}

void test_create_linked_list() {
    TEST_ASSERT_NOT_NULL(linked_list);
}

void test_do_not_create_linked_list_with_invalid_options() {
    // given
    LinkedListOptions invalid_options = {
        .equals = nullptr,
        .to_string = nullptr
    };
    // when
    LinkedList* new_linked_list; Error error = attempt(new_linked_list = linked_list_new(&invalid_options));
    // then
    TEST_ASSERT_NULL(new_linked_list);
    TEST_ASSERT_EQUAL(INVALID_ARGUMENTS_ERROR, error);
}

// TODO
void test_create_linked_list_from_collection() {
    TEST_FAIL();
}

// TODO
void test_do_not_create_linked_list_with_invalid_options_from_collection() {
    TEST_FAIL();
}

void test_delete_linked_list_set_it_to_null() {
    // given
    LinkedList* new_linked_list = linked_list_new(DEFAULT_LINKED_LIST_OPTIONS);
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

static void delete_data(void* element) {
    *(int*) element = 0; // in a real scenario, a free call is made here
}

void test_destroy_linked_list_deletes_its_data() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    linked_list_destroy(&linked_list, delete_data);
    // then
    int deleted_values[] = { 0, 0, 0, 0, 0 };
    TEST_ASSERT_NULL(linked_list);
    TEST_ASSERT_ARRAY_EQUALS(deleted_values, (void**) &values);
}

void test_destroy_null_linked_list_fails() {
    // given
    LinkedList* new_linked_list = nullptr;
    // when
    Error error = attempt(linked_list_destroy(&new_linked_list, delete_data));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_at_index_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool added = linked_list_add(linked_list, 2, &(int){10});
    // then
    int new_values[] = { 1, 2, 10, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_TRUE(added);
}

// Edge case
void test_add_element_at_index_0_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool added = linked_list_add(linked_list, 0, &(int){10});
    // then
    int new_values[] = { 10, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_TRUE(added);
}

// Edge case
void test_add_element_at_index_equal_size_to_linked_list() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool added = linked_list_add(linked_list, 5, &(int){10});
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(new_values, linked_list);
    TEST_ASSERT_TRUE(added);
}

static void add_index_out_of_bounds_test_helper(int index) {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_LINKED_LIST(linked_list, values);
    // when
    bool added; Error error = attempt(added = linked_list_add(linked_list, index, &(int){10}));
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(values, linked_list);
    TEST_ASSERT_FALSE(added);
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

// TODO
void test_add_all_elements_from_collection_at_index_in_linked_list() {
    TEST_FAIL();
}

// TODO
void test_add_all_elements_from_collection_at_beginning_of_linked_list() {
    TEST_FAIL();
}

// TODO
void test_add_all_elements_from_collection_at_end_of_linked_list() {
    TEST_FAIL();
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_linked_list);
    RUN_TEST(test_do_not_create_linked_list_with_invalid_options);

    RUN_TEST(test_create_linked_list_from_collection);
    RUN_TEST(test_do_not_create_linked_list_with_invalid_options_from_collection);

    RUN_TEST(test_delete_linked_list_set_it_to_null);
    RUN_TEST(test_delete_null_linked_list_fails);
    RUN_TEST(test_destroy_linked_list_deletes_its_data);
    RUN_TEST(test_destroy_null_linked_list_fails);

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

    UNITY_END();
}