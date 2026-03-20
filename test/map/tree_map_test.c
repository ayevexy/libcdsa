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

void test_create_tree_map_from_entry_collection() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    TreeMap* new_tree_map = tree_map_from(tree_map_entries(tree_map), CHAR_INT_TREE_MAP_OPTIONS());
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, new_tree_map);
    // clean up
    tree_map_destroy(&new_tree_map);
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

static void* remapper_return_null(void* key, void* value) {
    return nullptr;
}

static void* remapper_return_new_value(void* key, void* value) {
    return new(int, 10);
}

static void* remapper_sum_values(void* old_value, void* new_value) {
    *(int*) old_value += *(int*) new_value;
    return old_value;
}

void test_compute_mapping_of_tree_map_if_remapper_return_value_is_null_remove_mapping() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute(tree_map, &(char){'a'}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_compute_mapping_of_tree_map_if_remapper_return_value_is_not_null_put_mapping() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    int* value = tree_map_compute(tree_map, &(char){'a'}, remapper_return_new_value);
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_compute_mapping_of_tree_map_if_key_is_absent_and_remapper_return_null_do_nothing() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute(tree_map, &(char){'a'}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

static void* mapper(void* key) {
    return new(int, 10);
}

void test_compute_mapping_of_tree_map_if_absent() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute_if_absent(tree_map, new(char, 'a'), mapper);
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_do_not_compute_mapping_of_tree_map_if_not_absent() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute_if_absent(tree_map, &(char){'a'}, mapper);
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_compute_mapping_of_tree_map_if_present() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute_if_present(tree_map, new(char, 'a'), remapper_return_new_value);
    // then
    CharIntEntry new_entries[] = { { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_do_not_compute_mapping_of_tree_map_if_not_present() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_compute_if_present(tree_map, &(char){'a'}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_merge_mapping_of_tree_map_if_old_value_is_null_insert_new_value() {
    // given
    CharIntEntry entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    int* value = tree_map_merge(tree_map, &(char){'a'}, new(int, 1), remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(1, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_merge_mapping_of_tree_map_if_old_value_is_not_null_merge_with_new_value() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_merge(tree_map, &(char){'a'}, &(int){1}, remapper_sum_values);
    // then
    CharIntEntry new_entries[] = { { 'a', 2 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(2, *value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
}

void test_merge_mapping_of_tree_map_if_merged_value_is_null_remove_mapping() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    int* value = tree_map_merge(tree_map, &(char){'a'}, &(int){1}, remapper_return_null);
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
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

void test_put_all_entries_to_tree_map() {
    // given
    TreeMap* new_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS());
    // and
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(new_tree_map, entries);
    // when
    tree_map_put_all(tree_map, tree_map_entries(new_tree_map));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
    // clean up
    tree_map_destroy(&new_tree_map);
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

void test_tree_map_iterator_forward_iteration() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_backward_iteration() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 5);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_previous(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_previous(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_detects_concurrent_modification() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    tree_map_remove(tree_map, &(char){'a'});
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_previous(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_remove(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_after_next() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // { 'a', 1 }
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_after_previous() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 5);
    // when
    iterator_remove(iterator); // { 'e', 5 }
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 } };
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_fails_if_no_next_or_previous_was_called() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    CharIntEntry new_entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_fails_if_called_twice_in_a_row() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_next(iterator);
    iterator_remove(iterator);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    CharIntEntry new_entries[] = { { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_ARRAY_EQUALS_TO_TREE_MAP(new_entries, tree_map);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_reset() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
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

void test_get_tree_map_keys() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Collection keys = tree_map_keys(tree_map);
    // then
    Iterator* iterator = collection_iterator(keys); // iteration order is guaranteed
    TEST_ASSERT_EQUAL('a', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('b', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('c', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('d', *(char*) iterator_next(iterator));
    TEST_ASSERT_EQUAL('e', *(char*) iterator_next(iterator));
    // and
    TEST_ASSERT_EQUAL('e', *(char*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL('d', *(char*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL('c', *(char*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL('b', *(char*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL('a', *(char*) iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_get_tree_map_values() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Collection values = tree_map_values(tree_map);
    // then
    Iterator* iterator = collection_iterator(values); // iteration order is guaranteed
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_EQUAL(5, *(int*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_get_tree_map_entries() {
    // given
    CharIntEntry entries[] = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 } };
    POPULATE_TREE_MAP(tree_map, entries);
    // when
    Collection entries_2 = tree_map_entries(tree_map);
    // then
    Iterator* iterator = collection_iterator(entries_2); // iteration order is guaranteed
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_next(iterator));
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_next(iterator));
    // and
    TEST_ASSERT_EQUAL_ENTRY('e', 5, iterator_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('d', 4, iterator_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('c', 3, iterator_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('b', 2, iterator_previous(iterator));
    TEST_ASSERT_EQUAL_ENTRY('a', 1, iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_tree_map);
    RUN_TEST(test_do_not_create_tree_map_with_invalid_options);
    RUN_TEST(test_create_tree_map_from_entry_collection);
    
    RUN_TEST(test_destroy_tree_map_set_it_to_null);
    RUN_TEST(test_destroy_null_tree_map_fails);
    
    RUN_TEST(test_compute_mapping_of_tree_map_if_remapper_return_value_is_null_remove_mapping);
    RUN_TEST(test_compute_mapping_of_tree_map_if_remapper_return_value_is_not_null_put_mapping);
    RUN_TEST(test_compute_mapping_of_tree_map_if_key_is_absent_and_remapper_return_null_do_nothing);

    RUN_TEST(test_compute_mapping_of_tree_map_if_absent);
    RUN_TEST(test_do_not_compute_mapping_of_tree_map_if_not_absent);
    RUN_TEST(test_compute_mapping_of_tree_map_if_present);
    RUN_TEST(test_do_not_compute_mapping_of_tree_map_if_not_present);

    RUN_TEST(test_merge_mapping_of_tree_map_if_old_value_is_null_insert_new_value);
    RUN_TEST(test_merge_mapping_of_tree_map_if_old_value_is_not_null_merge_with_new_value);
    RUN_TEST(test_merge_mapping_of_tree_map_if_merged_value_is_null_remove_mapping);

    RUN_TEST(test_put_entry_to_tree_map);
    RUN_TEST(test_put_entry_to_tree_map_if_key_already_exists_update_value);
    RUN_TEST(test_put_entry_to_tree_map_if_key_is_absent);
    RUN_TEST(test_do_not_put_entry_to_tree_map_if_key_is_not_absent);
    RUN_TEST(test_put_all_entries_to_tree_map);

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
    
    RUN_TEST(test_tree_map_iterator_forward_iteration);
    RUN_TEST(test_tree_map_iterator_backward_iteration);
    RUN_TEST(test_tree_map_iterator_detects_concurrent_modification);
    RUN_TEST(test_tree_map_iterator_remove_element_after_next);
    RUN_TEST(test_tree_map_iterator_remove_element_after_previous);
    RUN_TEST(test_tree_map_iterator_remove_element_fails_if_no_next_or_previous_was_called);
    RUN_TEST(test_tree_map_iterator_remove_element_fails_if_called_twice_in_a_row);
    RUN_TEST(test_tree_map_iterator_reset);
    
    RUN_TEST(test_tree_map_contains_entry);
    RUN_TEST(test_tree_map_does_not_contains_entry);
    RUN_TEST(test_tree_map_contains_key);
    RUN_TEST(test_tree_map_does_not_contains_key);
    RUN_TEST(test_tree_map_contains_value);
    RUN_TEST(test_tree_map_does_not_contains_value);
    
    RUN_TEST(test_get_tree_map_keys);
    RUN_TEST(test_get_tree_map_values);
    RUN_TEST(test_get_tree_map_entries);
    return UNITY_END();
}