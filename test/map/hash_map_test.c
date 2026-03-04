#include "hash_map_test.h"

#include "map/hash_map.h"
#include "util/errors.h"
#include "../test_functions.h"

#include "unity.h"
#include "list/array_list.h"

#define CHAR_INT_HASH_MAP_OPTIONS DEFAULT_HASH_MAP_OPTIONS(     \
    .hash = char_hash,                                          \
    .key_equals = char_pointer_value_equals,                    \
    .key_to_string = char_pointer_value_to_string,              \
    .value_equals = int_pointer_value_equals,                   \
    .value_to_string = int_pointer_value_to_string              \
)

static HashMap* hash_map;

void setUp() {
    hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
}

void tearDown() {
    hash_map_set_destructors(hash_map, free, free);
    hash_map_destroy(&hash_map);
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

static void* remapper_return_null(void* key, void* value) {
    return nullptr;
}

static void* remapper_return_new_value(void* key, void* value) {
    return int_new(10);
}

void test_compute_mapping_of_hash_map_if_remapper_return_value_is_null_remove_mapping() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value = hash_map_compute(hash_map, &(char){'a'}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_compute_mapping_of_hash_map_if_remapper_return_value_is_not_null_put_mapping() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    hash_map_put(hash_map, char_new('a'), nullptr);
    // when
    int* value = hash_map_compute(hash_map, &(char){'a'}, remapper_return_new_value);
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_compute_mapping_of_hash_map_if_key_is_absent_and_remapper_return_null_do_nothing() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* value = hash_map_compute(hash_map, &(char){'a'}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
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

void test_add_entry_to_hash_map_if_absent() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    hash_map_put(hash_map, char_new('a'), nullptr);
    // when
    void* old_value = hash_map_put_if_absent(hash_map, char_new('a'), int_new(1));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_do_not_add_entry_to_hash_map_if_not_absent() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    void* old_value = hash_map_put_if_absent(hash_map, char_new('a'), int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
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
    int* value = hash_map_get(hash_map, &(char){'k'});
    // then
    TEST_ASSERT_NULL(value);
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

void test_replace_value_from_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* old_value = hash_map_replace(hash_map, &(char){'a'}, int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_replace_value_from_hash_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    int* old_value = hash_map_replace(hash_map, &(char){'k'}, int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_replace_entry_from_hash_map_matching_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool replaced = hash_map_replace_if_equals(hash_map, &(char){'c'}, &(int){3}, int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 10 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_TRUE(replaced);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
}

void test_replace_entry_from_hash_map_no_matching_value_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool replaced = hash_map_replace_if_equals(hash_map, &(char){'c'}, &(int){2}, int_new(10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_FALSE(replaced);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
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
    int* value = hash_map_remove(hash_map, &(char){'k'});
    // then
    TEST_ASSERT_NULL(value);
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

static void* replace_by_2_times_original(void* key, void* value) {
    *(int*) value *= 2;
    return value;
}

void test_replace_all_values_from_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    hash_map_replace_all(hash_map, replace_by_2_times_original);
    // then
    CharIntEntry new_entries[] = { { 'a', 2 }, { 'b', 4 }, { 'c', 6 }, { 'd', 8 }, { 'e', 10 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(new_entries, hash_map);
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

void test_hash_map_iterator() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    Iterator* iterator = hash_map_iterator(hash_map);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_map_is_equal_to_it_self() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    bool equals = hash_map_equals(hash_map, hash_map);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_hash_map_is_equal_to_another_hash_map() {
    // given
    HashMap* other_hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
    // and
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    POPULATE_HASH_MAP(other_hash_map, entries);
    // when
    bool equals = hash_map_equals(hash_map, other_hash_map);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    hash_map_set_destructors(other_hash_map, free, free);
    hash_map_destroy(&other_hash_map);
}

void test_hash_map_is_not_equal_to_another_hash_map_with_different_size() {
    // given
    HashMap* other_hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
    // and
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    POPULATE_HASH_MAP(other_hash_map, entries);
    // and
    hash_map_put(other_hash_map, char_new('k'), int_new(10));

    // when
    bool equals = hash_map_equals(hash_map, other_hash_map);

    // then
    TEST_ASSERT_FALSE(equals);

    // clean up
    hash_map_set_destructors(other_hash_map, free, free);
    hash_map_destroy(&other_hash_map);
}

void test_hash_map_is_not_equal_to_another_hash_map_with_different_mappings() {
    // given
    HashMap* other_hash_map = hash_map_new(CHAR_INT_HASH_MAP_OPTIONS);
    // and
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // and
    CharIntEntry other_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'f', 6 } };
    POPULATE_HASH_MAP(other_hash_map, other_entries);

    // when
    bool equals = hash_map_equals(hash_map, other_hash_map);

    // then
    TEST_ASSERT_FALSE(equals);

    // clean up
    hash_map_set_destructors(other_hash_map, free, free);
    hash_map_destroy(&other_hash_map);
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
    TEST_ASSERT_NULL(hash_map_get(hash_map, &(char){'a'}));
    TEST_ASSERT_NULL(hash_map_get(hash_map, &(char){'b'}));
    TEST_ASSERT_NULL(hash_map_get(hash_map, &(char){'c'}));
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

void test_get_hash_map_keys() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    Collection keys = hash_map_keys(hash_map);
    // then
    Iterator* iterator = collection_iterator(keys); // iteration order is not guaranteed
    TEST_ASSERT_EQUAL('d', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('e', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('a', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('b', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('c', *(char*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_get_hash_map_values() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    Collection values = hash_map_values(hash_map);
    // then
    Iterator* iterator = collection_iterator(values); // iteration order is not guaranteed
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_get_hash_map_entries() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    Collection entries_2 = hash_map_entries(hash_map);
    // then
    Iterator* iterator = collection_iterator(entries_2); // iteration order is not guaranteed
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_clone_hash_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    HashMap* copy_hash_map = hash_map_clone(hash_map);
    // then
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_MAP(entries, copy_hash_map);
    // clean up
    hash_map_destroy(&copy_hash_map);
}

void test_get_hash_map_string_representation() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_HASH_MAP(hash_map, entries);
    // when
    char* string = hash_map_to_string(hash_map);
    // then
    TEST_ASSERT_EQUAL_STRING("[ d = 4, e = 5, a = 1, b = 2, c = 3 ]", string);
    // clean up
    free(string);
}

void test_get_empty_hash_map_string_representation() {
    // when
    char* string = hash_map_to_string(hash_map);
    // then
    TEST_ASSERT_EQUAL_STRING("[]", string);
    // clean up
    free(string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_map);
    RUN_TEST(test_do_not_create_hash_map_with_invalid_options);

    RUN_TEST(test_destroy_hash_map_set_it_to_null);
    RUN_TEST(test_destroy_null_hash_map_fails);

    RUN_TEST(test_compute_mapping_of_hash_map_if_remapper_return_value_is_null_remove_mapping);
    RUN_TEST(test_compute_mapping_of_hash_map_if_remapper_return_value_is_not_null_put_mapping);
    RUN_TEST(test_compute_mapping_of_hash_map_if_key_is_absent_and_remapper_return_null_do_nothing);

    RUN_TEST(test_add_entry_to_hash_map);
    RUN_TEST(test_update_entry_of_hash_map);
    RUN_TEST(test_add_entry_to_hash_map_if_absent);
    RUN_TEST(test_do_not_add_entry_to_hash_map_if_not_absent);

    RUN_TEST(test_get_value_from_hash_map);
    RUN_TEST(test_get_value_from_hash_map_no_mapping_fails);
    RUN_TEST(test_get_default_value_from_hash_map);

    RUN_TEST(test_replace_value_from_hash_map);
    RUN_TEST(test_replace_value_from_hash_map_no_mapping_fails);
    RUN_TEST(test_replace_entry_from_hash_map_matching_value);
    RUN_TEST(test_replace_entry_from_hash_map_no_matching_value_fails);

    RUN_TEST(test_remove_entry_from_hash_map);
    RUN_TEST(test_remove_entry_from_hash_map_no_mapping_fails);
    RUN_TEST(test_remove_entry_from_hash_map_matching_value);
    RUN_TEST(test_remove_entry_from_hash_map_no_matching_value_fails);
    RUN_TEST(test_replace_all_values_from_hash_map);

    RUN_TEST(test_get_hash_map_size);
    RUN_TEST(test_hash_map_is_empty);
    RUN_TEST(test_hash_map_is_not_empty);
    RUN_TEST(test_hash_map_iterator);

    RUN_TEST(test_hash_map_is_equal_to_it_self);
    RUN_TEST(test_hash_map_is_equal_to_another_hash_map);
    RUN_TEST(test_hash_map_is_not_equal_to_another_hash_map_with_different_size);
    RUN_TEST(test_hash_map_is_not_equal_to_another_hash_map_with_different_mappings);

    RUN_TEST(test_perform_action_for_each_entry_of_hash_map);
    RUN_TEST(test_clear_hash_map);

    RUN_TEST(test_hash_map_contains_entry);
    RUN_TEST(test_hash_map_does_not_contains_entry);
    RUN_TEST(test_hash_map_contains_key);
    RUN_TEST(test_hash_map_does_not_contains_key);
    RUN_TEST(test_hash_map_contains_value);
    RUN_TEST(test_hash_map_does_not_contains_value);

    RUN_TEST(test_get_hash_map_keys);
    RUN_TEST(test_get_hash_map_values);
    RUN_TEST(test_get_hash_map_entries);

    RUN_TEST(test_clone_hash_map);
    RUN_TEST(test_get_hash_map_string_representation);
    RUN_TEST(test_get_empty_hash_map_string_representation);
    return UNITY_END();
}