#ifndef LIBCDSA_DEQUE_TEST_H
#define LIBCDSA_DEQUE_TEST_H

#include "../test_utilities.h"

#define INT_DEQUE_OPTIONS() DEFAULT_DEQUE_OPTIONS(  \
    .equals = int_pointer_value_equals,             \
    .to_string = int_pointer_value_to_string        \
)

#define POPULATE_DEQUE(deque, array)                \
    for (int i = 0; i < SIZE(array); i++) {         \
        deque_add_last(deque, new(int, array[i]));  \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(array, deque)                     \
    TEST_ASSERT_EQUAL(SIZE(array), deque_size(deque));                      \
    for (int i = 0; i < SIZE(array); i++) {                                 \
        TEST_ASSERT_EQUAL(array[i], *(int*) deque_remove_first(deque));     \
    }                                                                       \

#endif