#ifndef LINKED_LIST_TEST_H
#define LINKED_LIST_TEST_H

#include "../test_utilities.h"

#define INT_LINKED_LIST_OPTIONS DEFAULT_LINKED_LIST_OPTIONS(    \
    .equals = int_pointer_value_equals,                         \
    .to_string = int_pointer_value_to_string                    \
)

#define POPULATE_LINKED_LIST(linked_list, array)                \
    for (int i = 0; i < SIZE(array); i++) {                     \
        linked_list_add_last(linked_list, new(int, array[i]));  \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(array, linked_list)             \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) linked_list_get(linked_list, i));   \
    }                                                                           \
    TEST_ASSERT_EQUAL(SIZE(array), linked_list_size(linked_list));

#endif