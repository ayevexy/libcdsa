#ifndef LINKED_LIST_TEST_H
#define LINKED_LIST_TEST_H


#define SIZE(array) (sizeof(array) / sizeof(array[0]))


#define POPULATE_LINKED_LIST(linked_list, array)        \
    for (int i = 0; i < SIZE(array); i++) {             \
        linked_list_add_last(linked_list, &array[i]);   \
    }


#define TEST_ASSERT_ARRAY_EQUALS_TO_LINKED_LIST(array, linked_list)             \
    for (int i = 0; i < SIZE(array); i++) {                                     \
        TEST_ASSERT_EQUAL(array[i], *(int*) linked_list_get(linked_list, i));   \
    }                                                                           \
    TEST_ASSERT_EQUAL(SIZE(array), linked_list_size(linked_list));


#define TEST_ASSERT_ARRAY_EQUALS(array_a, array_b)              \
    for (int i = 0; i < SIZE(array_a); i++) {                   \
        TEST_ASSERT_EQUAL(array_a[i], *(int*) array_b[i]);      \
    }


#endif