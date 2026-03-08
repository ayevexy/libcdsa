#ifndef ARRAY_LIST_TEST_H
#define ARRAY_LIST_TEST_H

#include "../test_utilities.h"

#define INT_ARRAY_LIST_OPTIONS DEFAULT_ARRAY_LIST_OPTIONS(  \
    .equals = int_pointer_value_equals,                     \
    .to_string = int_pointer_value_to_string                \
)

#define POPULATE_ARRAY_LIST(array_list, array)                  \
    for (int i = 0; i < SIZE(array); i++) {                     \
        array_list_add_last(array_list, int_new(array[i]));     \
}

#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAY_LIST(array, array_list)               \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) array_list_get(array_list, i));     \
    }                                                                           \
    TEST_ASSERT_EQUAL(SIZE(array), array_list_size(array_list));

#endif