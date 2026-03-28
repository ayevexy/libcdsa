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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_priority_queue);
    RUN_TEST(test_do_not_create_priority_queue_with_invalid_options);

    RUN_TEST(test_destroy_priority_queue_set_it_to_null);
    RUN_TEST(test_destroy_null_priority_queue_fails);
    return UNITY_END();
}
