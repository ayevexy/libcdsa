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

void test_get_default_value_from_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value_a = hash_map_get_or_default(hash_map, &(char){'a'}, nullptr);
    int* value_b = hash_map_get_or_default(hash_map, &(char){'k'}, &(int){10});
    // then
    TEST_ASSERT_EQUAL(1, *value_a);
    TEST_ASSERT_EQUAL(10, *value_b);
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

void test_remove_entry_from_hash_map_matching_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool removed = hash_map_remove_if_equals(hash_map, &(char){'c'}, &(int){3});
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_remove_entry_from_hash_map_no_matching_value_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool removed = hash_map_remove_if_equals(hash_map, &(char){'c'}, &(int){10});
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(entries, hash_map);
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

void test_hash_map_is_empty() {
    // when
    bool empty = hash_map_is_empty(hash_map);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_hash_map_is_not_empty() {
    // given
    hash_map_put(hash_map, char_new('k'), int_new(10));
    // when
    bool empty = hash_map_is_empty(hash_map);
    // then
    TEST_ASSERT_FALSE(empty);
}

static void action_add_one_every_two_keys(void* key, void* value) {
    switch (*(char*) key) {
        case 'a':
        case 'c':
        case 'e': { *(int*) value += 1; break; }
        default: ;
    }
}

void test_perform_action_for_each_entry_of_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    hash_map_for_each(hash_map, action_add_one_every_two_keys);
    // then
    CharIntEntry new_entries[] = { { 'a', 2 }, { 'b', 2 }, { 'c', 4 }, { 'd', 4}, { 'e', 6 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_clear_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    hash_map_clear(hash_map);
    // then
    TEST_ASSERT_EQUAL(0, hash_map_size(hash_map));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(hash_map_get(hash_map, &(char){'a'})));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(hash_map_get(hash_map, &(char){'b'})));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(hash_map_get(hash_map, &(char){'c'})));
}

void test_hash_map_contains_entry() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains(hash_map, &(char){'c'}, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_hash_map_does_not_contains_entry() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains(hash_map, &(char){'c'}, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_hash_map_contains_key() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains_key(hash_map, &(char){'c'});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_hash_map_does_not_contains_key() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains_key(hash_map, &(char){'k'});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_hash_map_contains_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains_value(hash_map, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_hash_map_does_not_contains_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool contains = hash_map_contains_value(hash_map, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
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
    RUN_TEST(test_get_default_value_from_hash_map);

    RUN_TEST(test_remove_entry_from_hash_map);
    RUN_TEST(test_remove_entry_from_hash_map_no_mapping_fails);
    RUN_TEST(test_remove_entry_from_hash_map_matching_value);
    RUN_TEST(test_remove_entry_from_hash_map_no_matching_value_fails);

    RUN_TEST(test_get_hash_map_size);
    RUN_TEST(test_hash_map_is_empty);
    RUN_TEST(test_hash_map_is_not_empty);
    RUN_TEST(test_perform_action_for_each_entry_of_hash_map);
    RUN_TEST(test_clear_hash_map);

    RUN_TEST(test_hash_map_contains_entry);
    RUN_TEST(test_hash_map_does_not_contains_entry);
    RUN_TEST(test_hash_map_contains_key);
    RUN_TEST(test_hash_map_does_not_contains_key);
    RUN_TEST(test_hash_map_contains_value);
    RUN_TEST(test_hash_map_does_not_contains_value);
    return UNITY_END();
}