#ifndef LIBCDSA_TREE_SET_TEST_H
#define LIBCDSA_TREE_SET_TEST_H

#include "../test_utilities.h"

#define INT_TREE_SET_OPTIONS() DEFAULT_TREE_SET_OPTIONS(    \
    .compare = int_pointer_value_compare,                   \
    .to_string = int_pointer_value_to_string                \
)

#define POPULATE_TREE_SET(tree_set, array)          \
    for (int i = 0; i < SIZE(array); i++) {         \
        tree_set_add(tree_set, new(int, array[i])); \
    }

#define TEST_ASSERT_TREE_SET_CONTAINS(tree_set, array)              \
    for (int i = 0; i < SIZE(array); i++) {                         \
        TEST_ASSERT_TRUE(tree_set_contains(tree_set, &array[i]));   \
    }                                                               \
    TEST_ASSERT_EQUAL(SIZE(array), tree_set_size(tree_set));

#define TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS(tree_set, array)       \
    for (int i = 0; i < SIZE(array); i++) {                         \
        TEST_ASSERT_FALSE(tree_set_contains(tree_set, &array[i]));  \
    }                                                               \

#endif