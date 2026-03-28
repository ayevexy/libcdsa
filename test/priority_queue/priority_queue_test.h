#ifndef PRIORITY_QUEUE_TEST_H
#define PRIORITY_QUEUE_TEST_H

#include "../test_utilities.h"

#define INT_PRIORITY_QUEUE_OPTIONS() DEFAULT_PRIORITY_QUEUE_OPTIONS(    \
    .compare = compare_int_pointers_value,                              \
    .equals = int_pointer_value_equals,                                 \
    .to_string = int_pointer_value_to_string                            \
)

#define POPULATE_PRIORITY_QUEUE(priority_queue, array)                  \
    for (int i = 0; i < SIZE(array); i++) {                             \
        priority_queue_enqueue(priority_queue, new(int, array[i]));     \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_PRIORITY_QUEUE(array, priority_queue)               \
    TEST_ASSERT_EQUAL(SIZE(array), priority_queue_size(priority_queue));                \
    for (int i = 0; i < SIZE(array); i++) {                                             \
        TEST_ASSERT_EQUAL(array[i], *(int*) priority_queue_dequeue(priority_queue));    \
    }                                                                                   \

#endif
