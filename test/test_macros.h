#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#define SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TEST_ASSERT_ARRAY_EQUALS(array_a, array_b)              \
    for (int i = 0; i < SIZE(array_a); i++) {                   \
        TEST_ASSERT_EQUAL(array_a[i], *(int*) array_b[i]);      \
    }

#endif