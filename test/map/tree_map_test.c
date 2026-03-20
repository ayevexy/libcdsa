#include "tree_map_test.h"

#include "map/tree_map.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static TreeMap* tree_map;

void setUp() {
    tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS());
}

void tearDown() {
    tree_map_set_key_destructor(tree_map, free);
    tree_map_set_value_destructor(tree_map, free);

    tree_map_destroy(&tree_map);
}

void test_create_tree_map() {
    TEST_ASSERT_NOT_NULL(tree_map);
}

void test_do_not_create_tree_map_with_invalid_options() {
    // when
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(&(TreeMapOptions) {}));
    // then
    TEST_ASSERT_NULL(new_tree_map);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_tree_map_set_it_to_null() {
    // given
    TreeMap* new_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS());
    // when
    tree_map_destroy(&new_tree_map);
    // then
    TEST_ASSERT_NULL(new_tree_map);
}

void test_destroy_null_tree_map_fails() {
    // given
    TreeMap* new_tree_map = nullptr;
    // when
    Error error = attempt(tree_map_destroy(&new_tree_map));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_put_entry_to_tree_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    void* old_value = tree_map_put(tree_map, new(char, 'k'), new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'k', 10 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_put_entry_to_tree_map_if_key_already_exists_update_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    void* old_value = tree_map_put(tree_map, new(char, 'a'), new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *(int*) old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_put_entry_to_tree_map_if_key_is_absent() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    void* old_value = tree_map_put_if_absent(tree_map, new(char, 'a'), new(int, 1));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_do_not_put_entry_to_tree_map_if_key_is_not_absent() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    void* old_value = tree_map_put_if_absent(tree_map, new(char, 'a'), new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *(int*) old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_get_value_from_tree_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_get(tree_map, &(char){'a'});
    // then
    TEST_ASSERT_EQUAL(1, *value);
}

void test_get_value_from_tree_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_get(tree_map, &(char){'k'});
    // then
    TEST_ASSERT_NULL(value);
}

void test_get_default_value_from_tree_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value_a = tree_map_get_or_default(tree_map, &(char){'a'}, nullptr);
    int* value_b = tree_map_get_or_default(tree_map, &(char){'k'}, &(int){10});
    // then
    TEST_ASSERT_EQUAL(1, *value_a);
    TEST_ASSERT_EQUAL(10, *value_b);
}

void test_replace_value_from_tree_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* old_value = tree_map_replace(tree_map, &(char){'a'}, new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_replace_value_from_tree_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* old_value = tree_map_replace(tree_map, &(char){'k'}, new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_replace_entry_from_tree_map_matching_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool replaced = tree_map_replace_if_equals(tree_map, &(char){'c'}, &(int){3}, new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 10 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_TRUE(replaced);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_replace_entry_from_tree_map_no_matching_value_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool replaced = tree_map_replace_if_equals(tree_map, &(char){'c'}, &(int){2}, new(int, 10));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_FALSE(replaced);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_remove_entry_from_tree_map() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_remove(tree_map, &(char){'c'});
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(3, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_remove_entry_from_tree_map_no_mapping_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_remove(tree_map, &(char){'k'});
    // then
    TEST_ASSERT_NULL(value);
}

void test_remove_entry_from_tree_map_matching_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool removed = tree_map_remove_if_equals(tree_map, &(char){'c'}, &(int){3});
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_remove_entry_from_tree_map_no_matching_value_fails() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool removed = tree_map_remove_if_equals(tree_map, &(char){'c'}, &(int){10});
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(entries, tree_map);
}

void test_get_tree_map_size() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int size = tree_map_size(tree_map);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

void test_tree_map_is_empty() {
    // when
    bool empty = tree_map_is_empty(tree_map);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_tree_map_is_not_empty() {
    // given
    tree_map_put(tree_map, new(char, 'k'), new(int, 10));
    // when
    bool empty = tree_map_is_empty(tree_map);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_tree_map_contains_entry() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_entry(tree_map, &(char){'c'}, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_entry() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_entry(tree_map, &(char){'c'}, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_tree_map_contains_key() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_key(tree_map, &(char){'c'});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_key() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_key(tree_map, &(char){'k'});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_tree_map_contains_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_value(tree_map, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    bool contains = tree_map_contains_value(tree_map, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_tree_map);
    RUN_TEST(test_do_not_create_tree_map_with_invalid_options);
    RUN_TEST(test_destroy_tree_map_set_it_to_null);
    RUN_TEST(test_destroy_null_tree_map_fails);

    RUN_TEST(test_put_entry_to_tree_map);
    RUN_TEST(test_put_entry_to_tree_map_if_key_already_exists_update_value);
    RUN_TEST(test_put_entry_to_tree_map_if_key_is_absent);
    RUN_TEST(test_do_not_put_entry_to_tree_map_if_key_is_not_absent);

    RUN_TEST(test_get_value_from_tree_map);
    RUN_TEST(test_get_value_from_tree_map_no_mapping_fails);
    RUN_TEST(test_get_default_value_from_tree_map);
    
    RUN_TEST(test_replace_value_from_tree_map);
    RUN_TEST(test_replace_value_from_tree_map_no_mapping_fails);
    RUN_TEST(test_replace_entry_from_tree_map_matching_value);
    RUN_TEST(test_replace_entry_from_tree_map_no_matching_value_fails);

    RUN_TEST(test_remove_entry_from_tree_map);
    RUN_TEST(test_remove_entry_from_tree_map_no_mapping_fails);
    RUN_TEST(test_remove_entry_from_tree_map_matching_value);
    RUN_TEST(test_remove_entry_from_tree_map_no_matching_value_fails);

    RUN_TEST(test_get_tree_map_size);
    RUN_TEST(test_tree_map_is_empty);
    RUN_TEST(test_tree_map_is_not_empty);
    
    RUN_TEST(test_tree_map_contains_entry);
    RUN_TEST(test_tree_map_does_not_contains_entry);
    RUN_TEST(test_tree_map_contains_key);
    RUN_TEST(test_tree_map_does_not_contains_key);
    RUN_TEST(test_tree_map_contains_value);
    RUN_TEST(test_tree_map_does_not_contains_value);
    return UNITY_END();
}