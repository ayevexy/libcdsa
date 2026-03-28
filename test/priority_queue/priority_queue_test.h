#ifndef PRIORITY_QUEUE_TEST_H
#define PRIORITY_QUEUE_TEST_H

#include "../test_utilities.h"

#define INT_PRIORITY_QUEUE_OPTIONS() DEFAULT_PRIORITY_QUEUE_OPTIONS(    \
    .compare = compare_int_pointers_value,                              \
    .equals = int_pointer_value_equals,                                 \
    .to_string = int_pointer_value_to_string                            \
)
/*
#define POPULATE_ARRAY_LIST(priority_queue, array)                      \
    for (int i = 0; i < SIZE(array); i++) {                             \
        priority_queue_add_last(priority_queue, new(int, array[i]));    \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAY_LIST(array, priority_queue)                   \
    for (int i = 0; i < SIZE(array); i++) {                                             \
        TEST_ASSERT_EQUAL(array[i], *(int*) priority_queue_get(priority_queue, i));     \
    }                                                                                   \
    TEST_ASSERT_EQUAL(SIZE(array), priority_queue_size(priority_queue));
*/
#endif
