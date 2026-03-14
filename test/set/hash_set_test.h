#ifndef HASH_SET_TEST_H
#define HASH_SET_TEST_H

#include "../test_utilities.h"

#define INT_HASH_SET_OPTIONS DEFAULT_HASH_SET_OPTIONS(  \
    .hash = char_hash,                                  \
    .equals = int_pointer_value_equals,                 \
    .to_string = int_pointer_value_to_string            \
)

#define POPULATE_HASH_SET(hash_set, array)          \
    for (int i = 0; i < SIZE(array); i++) {         \
        hash_set_add(hash_set, new(int, array[i])); \
    }

#define TEST_ASSERT_HASH_SET_CONTAINS(hash_set, array)              \
    for (int i = 0; i < SIZE(array); i++) {                         \
        TEST_ASSERT_TRUE(hash_set_contains(hash_set, &array[i]));   \
    }                                                               \
    TEST_ASSERT_EQUAL(SIZE(array), hash_set_size(hash_set));

#define TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS(hash_set, array)       \
    for (int i = 0; i < SIZE(array); i++) {                         \
        TEST_ASSERT_FALSE(hash_set_contains(hash_set, &array[i]));  \
    }                                                               \

#endif