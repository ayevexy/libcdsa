#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include "../test_macros.h"

#define POPULATE_HASH_MAP(hash_map, entry_array)                                                \
    for (int i = 0; i < SIZE(entry_array); i++) {                                               \
        hash_map_put(hash_map, char_new(entry_array[i].key), int_new(entry_array[i].value));    \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(entry_array, hash_map)                                     \
    for (int i = 0; i < SIZE(entry_array); i++) {                                                       \
        TEST_ASSERT_EQUAL(entry_array[i].value, *(int*) hash_map_get(hash_map, &entry_array[i].key));   \
    }                                                                                                   \
    TEST_ASSERT_EQUAL(SIZE(entry_array), hash_map_size(hash_map));

#define TEST_ASSERT_EQUAL_ENTRY(key, value, entry)              \
    do {                                                        \
        Entry* _entry = (entry);                                \
        TEST_ASSERT_EQUAL(key, *(char*) entry_key(_entry));     \
        TEST_ASSERT_EQUAL(value, *(int*) entry_value(_entry));  \
    } while (false)

#endif