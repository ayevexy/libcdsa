#include "array_list/array_list.h"

#include "unity.h"
#include "fff.h"
#include <stdlib.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(free, void*);
FAKE_VALUE_FUNC(void*, realloc, void*, size_t);

#define FFF_FAKES_LIST(FAKE)    \
    FAKE(free)                  \
    FAKE(realloc)

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
    size_t expected_size = 11;
    size_t expected_capacity = 20;
    TEST_ASSERT_EQUAL(expected_size, array_list_size(array_list));
    TEST_ASSERT_EQUAL(expected_capacity, array_list_capacity(array_list));
    // and
    TEST_ASSERT_EQUAL(elements, realloc_fake.return_val);
    // TODO: add assertion for realloc.fake.arg0_val
    TEST_ASSERT_EQUAL(sizeof(void*) * 20, realloc_fake.arg1_val);
    // clean up
    free(elements);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_array_list);
    RUN_TEST(test_delete_array_list);
    RUN_TEST(test_add_element_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list);
    RUN_TEST(test_add_multiple_elements_to_array_list_exceeding_capacity_resize_it);
    return UNITY_END();
}