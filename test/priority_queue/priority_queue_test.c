#include "priority_queue_test.h"

#include "priority_queue/priority_queue.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static PriorityQueue* priority_queue;

void setUp() {
    priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS());
}

void tearDown() {
    priority_queue_set_destructor(priority_queue, free);
    priority_queue_destroy(&priority_queue);
}

void test_create_priority_queue() {
    TEST_ASSERT_NOT_NULL(priority_queue);
}

void test_do_not_create_priority_queue_with_invalid_options() {
    // when
    PriorityQueue* new_priority_queue; Error error = attempt(new_priority_queue = priority_queue_new(&(PriorityQueueOptions) {}));
    // then
    TEST_ASSERT_NULL(new_priority_queue);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_priority_queue_set_it_to_null() {
    // given
    PriorityQueue* new_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS());
    // when
    priority_queue_destroy(&new_priority_queue);
    // then
    TEST_ASSERT_NULL(new_priority_queue);
}

void test_destroy_null_priority_queue_fails() {
    // given
    PriorityQueue* new_priority_queue = nullptr;
    // when
    Error error = attempt(priority_queue_destroy(&new_priority_queue));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_enqueue_element_to_priority_queue() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_PRIORITY_QUEUE(priority_queue, values);
    // when
    priority_queue_enqueue(priority_queue, new(int, 10));
    // then
    int new_values[] = { 10, 5, 4, 3, 2, 1 };
    TEST_ASSERT_ARRAY_EQUALS_TO_PRIORITY_QUEUE(new_values, priority_queue);
}

void test_dequeue_element_from_priority_queue() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_PRIORITY_QUEUE(priority_queue, values);
    // when
    int* value = priority_queue_dequeue(priority_queue);
    // then
    int new_values[] = { 4, 3, 2, 1 };
    TEST_ASSERT_EQUAL(5, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_PRIORITY_QUEUE(new_values, priority_queue);
}

void test_get_priority_queue_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_PRIORITY_QUEUE(priority_queue, values);
    // when
    int size = priority_queue_size(priority_queue);
    // then
    TEST_ASSERT_EQUAL(size, SIZE(values));
}

void test_get_priority_queue_capacity() {
    // when
    int capacity = priority_queue_capacity(priority_queue);
    // then
    TEST_ASSERT_EQUAL(10, capacity);
}

void test_priority_queue_is_empty() {
    // when
    bool empty = priority_queue_is_empty(priority_queue);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_priority_queue_is_not_empty() {
    // given
    priority_queue_enqueue(priority_queue, new(int, 10));
    // when
    bool empty = priority_queue_is_empty(priority_queue);
    // then
    TEST_ASSERT_FALSE(empty);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_priority_queue);
    RUN_TEST(test_do_not_create_priority_queue_with_invalid_options);

    RUN_TEST(test_destroy_priority_queue_set_it_to_null);
    RUN_TEST(test_destroy_null_priority_queue_fails);

    RUN_TEST(test_enqueue_element_to_priority_queue);
    RUN_TEST(test_dequeue_element_from_priority_queue);

    RUN_TEST(test_get_priority_queue_size);
    RUN_TEST(test_get_priority_queue_capacity);
    RUN_TEST(test_priority_queue_is_empty);
    RUN_TEST(test_priority_queue_is_not_empty);
    return UNITY_END();
}
