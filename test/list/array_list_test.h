#ifndef ARRAY_LIST_TEST_H
#define ARRAY_LIST_TEST_H

#include <stdio.h>
#include "fff.h"

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC_VARARG(int, fprintf, FILE*, const char*, ...);

#define INIT_MOCKS()        \
    RESET_FAKE(fprintf);    \
    FFF_RESET_HISTORY();    \


#define SIZE(array) (sizeof(array) / sizeof(array[0]))


#define POPULATE_ARRAY_LIST(array_list, array)          \
    for (int i = 0; i < SIZE(array); i++) {             \
        array_list_add_last(array_list, &array[i]);     \
}


#define TEST_ASSERT_ARRAY_EQUALS_TO_ARRAYLIST(array, array_list)                \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) array_list_get(array_list, i));     \
    }                                                                           \
    TEST_ASSERT_EQUAL(SIZE(array), array_list_size(array_list));


#define TEST_ASSERT_ARRAY_EQUALS(array_a, array_b)              \
    for (int i = 0; i < SIZE(array_a); i++) {                   \
        TEST_ASSERT_EQUAL(array_a[i], *(int*) array_b[i]);      \
    }


#define TEST_ASSERT_ERROR_MESSAGE(message)                      \
    TEST_ASSERT_EQUAL(stderr, fprintf_fake.arg0_val);           \
    TEST_ASSERT_EQUAL_STRING(message"\n", fprintf_fake.arg1_val)

#endif