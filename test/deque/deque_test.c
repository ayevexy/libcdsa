#include "deque_test.h"

#include "deque/deque.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static Deque* deque;

void setUp() {
    deque = deque_new(INT_DEQUE_OPTIONS);
}

void tearDown() {
    deque_set_destructor(deque, free);
    deque_destroy(&deque);
}

void test_create_deque() {
    TEST_ASSERT_NOT_NULL(deque);
}

void test_do_not_create_deque_with_invalid_options() {
    // when
    Deque* new_deque; Error error = attempt(new_deque = deque_new(&(DequeOptions) {}));
    // then
    TEST_ASSERT_NULL(new_deque);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_deque_set_it_to_null() {
    // given
    Deque* new_deque = deque_new(INT_DEQUE_OPTIONS);
    // when
    deque_destroy(&new_deque);
    // then
    TEST_ASSERT_NULL(new_deque);
}

void test_destroy_null_deque_fails() {
    // given
    Deque* new_deque = nullptr;
    // when
    Error error = attempt(deque_destroy(&new_deque));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_at_beginning_of_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    deque_add_first(deque, new(int, 10));
    // then
    int new_values[] = { 10, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
}

void test_add_element_at_end_of_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    deque_add_last(deque, new(int, 10));
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
}

void test_get_deque_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int size = deque_size(deque);
    // then
    TEST_ASSERT_EQUAL(size, SIZE(values));
}

void test_deque_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    bool contains = deque_contains(deque, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_deque_does_not_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    bool contains = deque_contains(deque, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_deque);
    RUN_TEST(test_do_not_create_deque_with_invalid_options);
    RUN_TEST(test_destroy_deque_set_it_to_null);
    RUN_TEST(test_destroy_null_deque_fails);
    RUN_TEST(test_add_element_at_beginning_of_deque);
    RUN_TEST(test_add_element_at_end_of_deque);
    RUN_TEST(test_get_deque_size);
    RUN_TEST(test_deque_contains_element);
    RUN_TEST(test_deque_does_not_contains_element);
    return UNITY_END();
}