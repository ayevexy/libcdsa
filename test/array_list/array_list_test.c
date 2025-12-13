#include "array_list/array_list.h"

#include "unity.h"
#include "fff.h"
#include <stdlib.h>
#include <stdio.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(free, void*);
FAKE_VALUE_FUNC(void*, realloc, void*, size_t);
FAKE_VALUE_FUNC_VARARG(int, fprintf, FILE*, const char*, ...);

#define FFF_FAKES_LIST(FAKE)    \
    FAKE(free)                  \
    FAKE(realloc)               \
    FAKE(fprintf)

ArrayList* array_list;

void setUp() {
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    array_list = array_list_new();
}

void tearDown() {
    if (array_list) array_list_delete(array_list);
}

void test_create_array_list() {
    TEST_ASSERT_NOT_NULL(array_list);
}

void test_delete_array_list() {
    // when
    array_list_delete(array_list);
    // then
    TEST_ASSERT_EQUAL(array_list, free_fake.arg0_val);
    // and (clean up)
    array_list = NULL; // Setting pointer to null to prevent double delete call
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
    // setup
    void** elements = malloc(sizeof(void*) * 10);
    realloc_fake.return_val = elements;
    // given
    int values[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (int i = 0; i < 10; i++) {
        array_list_add(array_list, &values[i]);
    }
    // when
    array_list_add(array_list, &values[11]);
    // then
    int expected_size = 11;
    int expected_capacity = 20;
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_EQUAL(expected_capacity, array_list_capacity(array_list));
    // and
    TEST_ASSERT_EQUAL(elements, realloc_fake.return_val);
    // TODO: add assertion for realloc.fake.arg0_val
    TEST_ASSERT_EQUAL(sizeof(void*) * 20, realloc_fake.arg1_val);
    // clean up
    free(elements);
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_array_list);
    RUN_TEST(test_delete_array_list);
    RUN_TEST(test_add_element_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it);
    RUN_TEST(test_get_element_from_array_list);
    RUN_TEST(test_get_element_from_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_set_element_of_array_list);
    RUN_TEST(test_set_element_of_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_remove_element_from_array_list);
    RUN_TEST(test_remove_element_from_array_list_shifts_its_remaining_elements);
    RUN_TEST(test_remove_element_from_array_list_index_out_of_bounds_warns_client);
    RUN_TEST(test_array_list_is_not_empty);
    RUN_TEST(test_array_list_is_empty);
    RUN_TEST(test_clear_array_list);
    RUN_TEST(test_array_list_contains_element);
    RUN_TEST(test_array_list_does_not_contains_element);
    return UNITY_END();
}