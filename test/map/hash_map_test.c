#include "hash_map_test.h"

#include "map/hash_map.h"
#include "util/errors.h"
#include "../test_functions.h"

#include "unity.h"

#define CHAR_INT_HASH_MAP_OPTIONS DEFAULT_HASH_MAP_OPTIONS(     \
    .hash = char_hash,                                          \
    .key_destruct = free,                                       \
    .key_equals = char_pointer_value_equals,                    \
    .key_to_string = char_pointer_value_to_string,              \
    .value_destruct = free,                                     \
    .value_equals = int_pointer_value_equals,                   \
    .value_to_string = int_pointer_value_to_string              \
)

static HashMap* hash_map;

void setUp() {
    hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
}

void tearDown() {
    hash_map_obliterate(&hash_map);
}

void test_create_hash_map() {
    TEST_ASSERT_NOT_NULL(hash_map);
}

void test_do_not_create_hash_map_with_invalid_options() {
    // when
    HashMap* new_hash_map; Error error = attempt(new_hash_map = hash_map_new(&(HashMapOptions) {}));
    // then
    TEST_ASSERT_NULL(new_hash_map);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_hash_map_set_it_to_null() {
    // given
    HashMap* new_hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
    // when
    hash_map_destroy(&new_hash_map);
    // then
    TEST_ASSERT_NULL(new_hash_map);
}

void test_destroy_null_hash_map_fails() {
    // given
    HashMap* new_hash_map = nullptr;
    // when
    Error error = attempt(hash_map_destroy(&new_hash_map));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_entry_to_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    void* old_value = hash_map_put(hash_map, char_new('k'), int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'k', 10 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_update_entry_of_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    void* old_value = hash_map_put(hash_map, char_new('a'), int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *(int*) old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_get_value_from_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value = hash_map_get(hash_map, &(char){'a'});
    // then
    TEST_ASSERT_EQUAL(1, *value);
}

void test_get_value_from_hash_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value; Error error = attempt(value = hash_map_get(hash_map, &(char){'k'}));
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_entry_from_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value = hash_map_remove(hash_map, &(char){'c'});
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(3, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_remove_entry_from_hash_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value; Error error = attempt(value = hash_map_remove(hash_map, &(char){'k'}));
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_hash_map_size() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int size = hash_map_size(hash_map);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_map);
    RUN_TEST(test_do_not_create_hash_map_with_invalid_options);
    RUN_TEST(test_destroy_hash_map_set_it_to_null);
    RUN_TEST(test_destroy_null_hash_map_fails);
    RUN_TEST(test_add_entry_to_hash_map);
    RUN_TEST(test_update_entry_of_hash_map);
    RUN_TEST(test_get_value_from_hash_map);
    RUN_TEST(test_get_value_from_hash_map_no_mapping_fails);
    RUN_TEST(test_remove_entry_from_hash_map);
    RUN_TEST(test_remove_entry_from_hash_map_no_mapping_fails);
    RUN_TEST(test_get_hash_map_size);
    return UNITY_END();
}