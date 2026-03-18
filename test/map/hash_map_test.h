#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include "../test_utilities.h"

#define CHAR_INT_HASH_MAP_OPTIONS() DEFAULT_HASH_MAP_OPTIONS(   \
    .hash = char_hash,                                          \
    .key_equals = char_pointer_value_equals,                    \
    .key_to_string = char_pointer_value_to_string,              \
    .value_equals = int_pointer_value_equals,                   \
    .value_to_string = int_pointer_value_to_string              \
)

#define POPULATE_HASH_MAP(hash_map, entry_array)                                                \
    for (int i = 0; i < SIZE(entry_array); i++) {                                               \
        hash_map_put(hash_map, new(char, entry_array[i].key), new(int, entry_array[i].value));  \
    }

#define TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(entry_array, hash_map)                                     \
    for (int i = 0; i < SIZE(entry_array); i++) {                                                       \
        TEST_ASSERT_EQUAL(entry_array[i].value, *(int*) hash_map_get(hash_map, &entry_array[i].key));   \
    }                                                                                                   \
    TEST_ASSERT_EQUAL(SIZE(entry_array), hash_map_size(hash_map));

#define TEST_ASSERT_EQUAL_ENTRY(_key, _value, entry)        \
    do {                                                    \
        MapEntry* _entry = (entry);                         \
        TEST_ASSERT_EQUAL(_key, *(char*) _entry->key);      \
        TEST_ASSERT_EQUAL(_value, *(int*) _entry->value);   \
    } while (false)

#endif