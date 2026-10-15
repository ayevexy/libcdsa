#include "map/tree_map.h"

#include "../test_utilities.h"
#include "unity.h"

#define CHAR_INT_TREE_MAP_OPTIONS &(TreeMapOptions) {   \
    .key_destruct = noop_destruct,                      \
    .key_compare = char_pointer_value_compare,          \
    .key_to_string = char_pointer_value_to_string,      \
    .value_destruct = noop_destruct,                    \
    .value_equals = int_pointer_value_equals,           \
    .value_to_string = int_pointer_value_to_string,     \
    .memory_alloc = memory_try_alloc,                   \
    .memory_dealloc = memory_dealloc                    \
}

#define POPULATE_TREE_MAP(tree_map, T, ...)                     \
    do {                                                        \
        auto array = (T[]){__VA_ARGS__};                        \
        int length = COUNT_ARGS(__VA_ARGS__) / 2;               \
        for (int i = 0; i < length; i++) {                      \
            tree_map_put(tree_map,                              \
                new(typeof(array[i].key), array[i].key),        \
                new(typeof(array[i].value), array[i].value)     \
            );                                                  \
        }                                                       \
    } while (false)

#define TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, T, ...)                       \
    do {                                                                            \
        auto array = (T[]){__VA_ARGS__};                                            \
        int length = COUNT_ARGS(__VA_ARGS__) / 2;                                   \
        TEST_ASSERT_EQUAL(length, tree_map_size(tree_map));                         \
        for (int i = 0; i < length; i++) {                                          \
            TEST_ASSERT_EQUAL(array[i].value,                                       \
                *(typeof(array[i].value)*) tree_map_get(tree_map, &array[i].key)    \
            );                                                                      \
        }                                                                           \
    } while (false)

#define TEST_ASSERT_ENTRY_EQUALS(key_, value_, entry)       \
    do {                                                    \
        MapEntry* entry_ = (entry);                         \
        TEST_ASSERT_EQUAL(key_, *(char*) entry_->key);      \
        TEST_ASSERT_EQUAL(value_, *(int*) entry_->value);   \
    } while (false)

static TreeMap* tree_map;

void setUp() {
    tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
    
    tree_map_change_key_destructor(tree_map, delete);
    tree_map_change_value_destructor(tree_map, delete);
}

void tearDown() {
    tree_map_destroy(&tree_map);
}

void test_create_tree_map() {
    TEST_ASSERT_NOT_NULL(tree_map);
}

void test_do_not_create_tree_map_with_invalid_options() {
    // when
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(ref(TreeMapOptions)));
    // then
    TEST_ASSERT_NULL(new_tree_map);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_tree_map_from_entry_collection() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* new_tree_map = tree_map_from(tree_map_entries(tree_map), CHAR_INT_TREE_MAP_OPTIONS);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(new_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    tree_map_destroy(&new_tree_map);
}

void test_destroy_tree_map_set_it_to_null() {
    // given
    TreeMap* new_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
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
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute(tree_map, ref(char, 'a'), remapper_return_null);
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_compute_mapping_of_tree_map_if_remapper_return_value_is_not_null_put_mapping() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    int* value = tree_map_compute(tree_map, ref(char, 'a'), remapper_return_new_value);
    // then
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_compute_mapping_of_tree_map_if_key_is_absent_and_remapper_return_null_do_nothing() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute(tree_map, ref(char, 'a'), remapper_return_null);
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

static void* mapper(void* key) {
    return new(int, 10);
}

void test_compute_mapping_of_tree_map_if_absent() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute_if_absent(tree_map, new(char, 'a'), mapper);
    // then
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_do_not_compute_mapping_of_tree_map_if_not_absent() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute_if_absent(tree_map, ref(char, 'a'), mapper);
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_compute_mapping_of_tree_map_if_present() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute_if_present(tree_map, ref(char, 'a'), remapper_return_new_value);
    // then
    TEST_ASSERT_EQUAL(10, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_do_not_compute_mapping_of_tree_map_if_not_present() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_compute_if_present(tree_map, ref(char, 'a'), remapper_return_null);
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_merge_mapping_of_tree_map_if_old_value_is_null_insert_new_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    int* value = tree_map_merge(tree_map, ref(char, 'a'), new(int, 1), remapper_return_null);
    // then
    TEST_ASSERT_EQUAL(1, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_merge_mapping_of_tree_map_if_old_value_is_not_null_merge_with_new_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_merge(tree_map, ref(char, 'a'), ref(int, 1), remapper_sum_values);
    // then
    TEST_ASSERT_EQUAL(2, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 2 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_merge_mapping_of_tree_map_if_merged_value_is_null_remove_mapping() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_merge(tree_map, ref(char, 'a'), ref(int, 1), remapper_return_null);
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_put_entry_to_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    void* old_value = tree_map_put(tree_map, new(char, 'k'), new(int, 10));
    // then
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'k', 10 }
    );
}

void test_put_entry_to_tree_map_if_key_already_exists_update_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_put(tree_map, ref(char, 'a'), new(int, 10));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_put_entry_to_tree_map_if_key_already_exists_update_value_no_destruct() {
    // given
    tree_map_change_value_destructor(tree_map, noop_destruct);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    void* old_value = tree_map_put(tree_map, ref(char, 'a'), new(int, 10));
    // then
    TEST_ASSERT_EQUAL(1, *(int*) old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    delete(old_value);
    tree_map_change_value_destructor(tree_map, delete);
}

void test_put_entry_to_tree_map_if_key_is_absent() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    tree_map_put(tree_map, new(char, 'a'), nullptr);
    // when
    void* old_value = tree_map_put_if_absent(tree_map, ref(char, 'a'), new(int, 1));
    // then
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_do_not_put_entry_to_tree_map_if_key_is_not_absent() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    void* old_value = tree_map_put_if_absent(tree_map, ref(char, 'a'), ref(int, 10));
    // then
    TEST_ASSERT_EQUAL(1, *(int*) old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_put_all_entries_to_tree_map() {
    // given
    TreeMap* new_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
    // and
    POPULATE_TREE_MAP(new_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_put_all(tree_map, tree_map_entries(new_tree_map));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    tree_map_destroy(&new_tree_map);
}

void test_get_value_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_get(tree_map, ref(char, 'a'));
    // then
    TEST_ASSERT_EQUAL(1, *value);
}

void test_get_value_from_tree_map_no_mapping_fails() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_get(tree_map, ref(char, 'k'));
    // then
    TEST_ASSERT_NULL(value);
}

void test_get_default_value_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value_a = tree_map_get_or_default(tree_map, ref(char, 'a'), nullptr);
    int* value_b = tree_map_get_or_default(tree_map, ref(char, 'k'), ref(int, 10));
    // then
    TEST_ASSERT_EQUAL(1, *value_a);
    TEST_ASSERT_EQUAL(10, *value_b);
}

void test_get_first_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry entry = tree_map_get_first(tree_map);
    // then
    TEST_ASSERT_ENTRY_EQUALS('a', 1, &entry);
}

void test_get_first_entry_from_empty_tree_map_fails() {
    // when
    MapEntry entry; Error error = attempt(entry = tree_map_get_first(tree_map));
    // then
    TEST_ASSERT_NULL(entry.key);
    TEST_ASSERT_NULL(entry.value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_last_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry entry = tree_map_get_last(tree_map);
    // then
    TEST_ASSERT_ENTRY_EQUALS('e', 5, &entry);
}

void test_get_last_entry_from_empty_tree_map_fails() {
    // when
    MapEntry entry; Error error = attempt(entry = tree_map_get_last(tree_map));
    // then
    TEST_ASSERT_NULL(entry.key);
    TEST_ASSERT_NULL(entry.value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_replace_value_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_replace(tree_map, ref(char, 'a'), new(int, 10));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_replace_value_from_tree_map_no_destruct() {
    // given
    tree_map_change_value_destructor(tree_map, noop_destruct);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* old_value = tree_map_replace(tree_map, ref(char, 'a'), new(int, 10));
    // then
    TEST_ASSERT_EQUAL(1, *old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 10 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    delete(old_value);
    tree_map_change_value_destructor(tree_map, delete);
}

void test_replace_value_from_tree_map_no_mapping_fails() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* old_value = tree_map_replace(tree_map, ref(char, 'k'), ref(int, 10));
    // then
    TEST_ASSERT_NULL(old_value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_replace_entry_from_tree_map_matching_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool replaced = tree_map_replace_if_equals(tree_map, ref(char, 'c'), ref(int, 3), new(int, 10));
    // then
    TEST_ASSERT_TRUE(replaced);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 10 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_replace_entry_from_tree_map_no_matching_value_fails() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool replaced = tree_map_replace_if_equals(tree_map, ref(char, 'c'), ref(int, 2), ref(int, 10));
    // then
    TEST_ASSERT_FALSE(replaced);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_remove_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_remove(tree_map, ref(char, 'c'));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_remove_entry_from_tree_map_no_destruct() {
    // given
    tree_map_change_value_destructor(tree_map, noop_destruct);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_remove(tree_map, ref(char, 'c'));
    // then
    TEST_ASSERT_EQUAL(3, *value);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    delete(value);
    tree_map_change_value_destructor(tree_map, delete);
}

void test_remove_entry_from_tree_map_no_mapping_fails() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    int* value = tree_map_remove(tree_map, ref(char, 'k'));
    // then
    TEST_ASSERT_NULL(value);
}

void test_remove_entry_from_tree_map_matching_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool removed = tree_map_remove_if_equals(tree_map, ref(char, 'c'), ref(int, 3));
    // then
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_remove_entry_from_tree_map_no_matching_value_fails() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool removed = tree_map_remove_if_equals(tree_map, ref(char, 'c'), ref(int, 10));
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_remove_first_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_remove_first(tree_map);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
}

void test_remove_first_entry_from_tree_map_no_destruct() {
    // given
    tree_map_change_key_destructor(tree_map, noop_destruct);
    tree_map_change_value_destructor(tree_map, noop_destruct);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry entry = tree_map_remove_first(tree_map);
    // then
    TEST_ASSERT_ENTRY_EQUALS('a', 1, &entry);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // then
    delete(entry.key);
    delete(entry.value);
    // and
    tree_map_change_key_destructor(tree_map, delete);
    tree_map_change_value_destructor(tree_map, delete);
}

void test_remove_first_entry_from_empty_tree_map_fails() {
    // when
    MapEntry entry; Error error = attempt(entry = tree_map_remove_first(tree_map));
    // then
    TEST_ASSERT_NULL(entry.key);
    TEST_ASSERT_NULL(entry.value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_last_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_remove_last(tree_map);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }
    );
}

void test_remove_last_entry_from_tree_map_no_destruct() {
    // given
    tree_map_change_key_destructor(tree_map, noop_destruct);
    tree_map_change_value_destructor(tree_map, noop_destruct);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry entry = tree_map_remove_last(tree_map);
    // then
    TEST_ASSERT_ENTRY_EQUALS('e', 5, &entry);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }
    );
    // then
    delete(entry.key);
    delete(entry.value);
    // and
    tree_map_change_key_destructor(tree_map, delete);
    tree_map_change_value_destructor(tree_map, delete);
}

void test_remove_last_entry_from_empty_tree_map_fails() {
    // when
    MapEntry entry; Error error = attempt(entry = tree_map_remove_last(tree_map));
    // then
    TEST_ASSERT_NULL(entry.key);
    TEST_ASSERT_NULL(entry.value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

static void* replace_by_2_times_original(void* key, void* value) {
    *(int*) value *= 2;
    return value;
}

void test_replace_all_values_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_replace_all(tree_map, replace_by_2_times_original);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 2 }, { 'b', 4 }, { 'c', 6 }, { 'd', 8 }, { 'e', 10 }
    );
}

void test_get_tree_map_size() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
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
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('a', 1, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('b', 2, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('c', 3, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('d', 4, iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('e', 5, iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_backward_iteration() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 5);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('e', 5, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('d', 4, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('c', 3, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('b', 2, iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('a', 1, iterator_previous(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_previous(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_detects_concurrent_modification() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    Iterator* iterator = tree_map_iterator(tree_map);
    tree_map_remove(tree_map, ref(char, 'a'));
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_previous(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_remove(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_after_next() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // { 'a', 1 }
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_after_previous() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 5);
    // when
    iterator_remove(iterator); // { 'e', 5 }
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }
    );
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_fails_if_no_next_or_previous_was_called() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_remove_element_fails_if_called_twice_in_a_row() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_next(iterator);
    iterator_remove(iterator);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_reset() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    Iterator* iterator = tree_map_iterator(tree_map);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_ENTRY_EQUALS('a', 1, iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_at_specified_position() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    Iterator* iterator = tree_map_iterator_at(tree_map, 3);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('c', 3, iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_map_iterator_at_invalid_position_fails() {
    // given
    Iterator* iterator;
    // then
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = tree_map_iterator_at(tree_map, -1)));
    TEST_ASSERT_NULL(iterator);
    // and
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = tree_map_iterator_at(tree_map, 6)));
    TEST_ASSERT_NULL(iterator);
}

void test_tree_map_is_equal_to_it_self() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool equals = tree_map_equals(tree_map, tree_map);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_tree_map_is_equal_to_another_tree_map() {
    // given
    TreeMap* other_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    POPULATE_TREE_MAP(other_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool equals = tree_map_equals(tree_map, other_tree_map);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    tree_map_change_key_destructor(other_tree_map, delete);
    tree_map_change_value_destructor(other_tree_map, delete);
    tree_map_destroy(&other_tree_map);
}

void test_tree_map_is_not_equal_to_another_tree_map_with_different_size() {
    // given
    TreeMap* other_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    POPULATE_TREE_MAP(other_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // and
    tree_map_put(other_tree_map, new(char, 'k'), new(int, 10));
    // when
    bool equals = tree_map_equals(tree_map, other_tree_map);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_map_change_key_destructor(other_tree_map, delete);
    tree_map_change_value_destructor(other_tree_map, delete);
    tree_map_destroy(&other_tree_map);
}

void test_tree_map_is_not_equal_to_another_tree_map_with_different_mappings() {
    // given
    TreeMap* other_tree_map = tree_map_new(CHAR_INT_TREE_MAP_OPTIONS);
    // and
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    POPULATE_TREE_MAP(other_tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'f', 6 }
    );
    // when
    bool equals = tree_map_equals(tree_map, other_tree_map);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_map_change_key_destructor(other_tree_map, delete);
    tree_map_change_value_destructor(other_tree_map, delete);
    tree_map_destroy(&other_tree_map);
}

static void action_add_one_every_two_keys(void* key, void* value) {
    switch (*(char*) key) {
        case 'a':
        case 'c':
        case 'e': { *(int*) value += 1; break; }
        default: ;
    }
}

void test_perform_action_for_each_entry_of_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    tree_map_for_each(tree_map, action_add_one_every_two_keys);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(tree_map, CharIntEntry,
        { 'a', 2 }, { 'b', 2 }, { 'c', 4 }, { 'd', 4 }, { 'e', 6 }
    );
}

void test_get_higher_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry next_greater = tree_map_higher(tree_map, ref(char, 'c'));
    MapEntry no_successor = tree_map_higher(tree_map, ref(char, 'e'));
    // then
    TEST_ASSERT_ENTRY_EQUALS('d', 4, &next_greater);
    TEST_ASSERT(!no_successor.key && !no_successor.value);
}

void test_get_ceiling_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'e', 5 }
    );
    // when
    MapEntry exact_match = tree_map_ceiling(tree_map, ref(char, 'e'));
    MapEntry next_greater = tree_map_ceiling(tree_map, ref(char, 'd'));
    MapEntry no_successor = tree_map_ceiling(tree_map, ref(char, 'f'));
    // then
    TEST_ASSERT_ENTRY_EQUALS('e', 5, &exact_match);
    TEST_ASSERT_ENTRY_EQUALS('e', 5, &next_greater);
    TEST_ASSERT(!no_successor.key && !no_successor.value);
}

void test_get_floor_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry exact_match = tree_map_floor(tree_map, ref(char, 'a'));
    MapEntry next_least = tree_map_floor(tree_map, ref(char, 'b'));
    MapEntry no_predecessor = tree_map_floor(tree_map, ref(char, '`')); // backtick is below 'a' in ASCII
    // then
    TEST_ASSERT_ENTRY_EQUALS('a', 1, &exact_match);
    TEST_ASSERT_ENTRY_EQUALS('a', 1, &next_least);
    TEST_ASSERT(!no_predecessor.key && !no_predecessor.value);
}

void test_get_lower_entry_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    MapEntry next_least = tree_map_lower(tree_map, ref(char, 'c'));
    MapEntry no_predecessor = tree_map_lower(tree_map, ref(char, 'a'));
    // then
    TEST_ASSERT_ENTRY_EQUALS('b', 2, &next_least);
    TEST_ASSERT(!no_predecessor.key && !no_predecessor.value);
}

void test_clear_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }
    );
    // when
    tree_map_clear(tree_map);
    // then
    TEST_ASSERT_EQUAL(0, tree_map_size(tree_map));
    TEST_ASSERT_NULL(tree_map_get(tree_map, ref(char, 'a')));
    TEST_ASSERT_NULL(tree_map_get(tree_map, ref(char, 'b')));
    TEST_ASSERT_NULL(tree_map_get(tree_map, ref(char, 'c')));
}

void test_tree_map_contains_entry() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains(tree_map, ref(char, 'c'), ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_entry() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains(tree_map, ref(char, 'c'), ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_tree_map_contains_key() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains_key(tree_map, ref(char, 'c'));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_key() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains_key(tree_map, ref(char, 'k'));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_tree_map_contains_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains_value(tree_map, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_map_does_not_contains_value() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    bool contains = tree_map_contains_value(tree_map, ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_get_tree_map_entries() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    Collection entries_2 = tree_map_entries(tree_map);
    // then
    Iterator* iterator = collection_iterator(entries_2); // iteration order is guaranteed
    TEST_ASSERT_ENTRY_EQUALS('a', 1, iterator_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('b', 2, iterator_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('c', 3, iterator_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('d', 4, iterator_next(iterator));
    TEST_ASSERT_ENTRY_EQUALS('e', 5, iterator_next(iterator));
    // and
    TEST_ASSERT_ENTRY_EQUALS('e', 5, iterator_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('d', 4, iterator_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('c', 3, iterator_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('b', 2, iterator_previous(iterator));
    TEST_ASSERT_ENTRY_EQUALS('a', 1, iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_get_tree_map_keys() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
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
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
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

void test_create_head_map_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* new_tree_map = tree_map_head_map(tree_map, ref(char, 'c'));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(new_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }
    );
    // clean up
    tree_map_destroy(&new_tree_map);
}

void test_create_tail_map_from_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* new_tree_map = tree_map_tail_map(tree_map, ref(char, 'c'));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(new_tree_map, CharIntEntry,
        { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    tree_map_destroy(&new_tree_map);
}

void test_create_sub_map_of_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* new_tree_map = tree_map_sub_map(tree_map, ref(char, 'b'), ref(char, 'e'));
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(new_tree_map, CharIntEntry,
        { 'b', 2 }, { 'c', 3 }, { 'd', 4 }
    );
    // clean up
    tree_map_destroy(&new_tree_map);
}

void test_create_empty_sub_map_of_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* new_tree_map = tree_map_sub_map(tree_map, ref(char, 'c'), ref(char, 'c'));
    // then
    TEST_ASSERT_EQUAL(0, tree_map_size(new_tree_map));
    // clean up
    tree_map_destroy(&new_tree_map);
}

static void sub_map_inexistent_key_test_helper(char* start_key, char* end_key) {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* sub_map; Error error = attempt(sub_map = tree_map_sub_map(tree_map, start_key, end_key));
    // then
    TEST_ASSERT_NULL(sub_map);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_sub_map_inexistent_end_key_fails() {
    sub_map_inexistent_key_test_helper(ref(char, 'a'), ref(char, 'f'));
}

void test_create_sub_map_inexistent_start_key_fails() {
    sub_map_inexistent_key_test_helper(ref(char, '`'), ref(char, 'e')); // backtick is below 'a' in ASCII
}

void test_create_sub_map_start_key_greater_than_end_key_fails() {
    sub_map_inexistent_key_test_helper(ref(char, 'e'), ref(char, 'c'));
}

void test_clone_tree_map() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    TreeMap* copy_tree_map = tree_map_clone(tree_map);
    // then
    TEST_ASSERT_TREE_MAP_ENTRIES_EQUALS(copy_tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // clean up
    tree_map_destroy(&copy_tree_map);
}

void test_get_tree_map_string_representation() {
    // given
    POPULATE_TREE_MAP(tree_map, CharIntEntry,
        { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }
    );
    // when
    String string = tree_map_to_string(tree_map);
    // then
    TEST_ASSERT_EQUAL_STRING("[ a = 1, b = 2, c = 3, d = 4, e = 5 ]", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_get_empty_tree_map_string_representation() {
    // when
    String string = tree_map_to_string(tree_map);
    // then
    TEST_ASSERT_EQUAL_STRING("[]", string_data(string));
    // clean up
    string_destroy(&string);
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
    RUN_TEST(test_put_entry_to_tree_map_if_key_already_exists_update_value_no_destruct);
    RUN_TEST(test_put_entry_to_tree_map_if_key_is_absent);
    RUN_TEST(test_do_not_put_entry_to_tree_map_if_key_is_not_absent);
    RUN_TEST(test_put_all_entries_to_tree_map);

    RUN_TEST(test_get_value_from_tree_map);
    RUN_TEST(test_get_value_from_tree_map_no_mapping_fails);
    RUN_TEST(test_get_default_value_from_tree_map);
    RUN_TEST(test_get_first_entry_from_tree_map);
    RUN_TEST(test_get_first_entry_from_empty_tree_map_fails);
    RUN_TEST(test_get_last_entry_from_tree_map);
    RUN_TEST(test_get_last_entry_from_empty_tree_map_fails);

    RUN_TEST(test_replace_value_from_tree_map);
    RUN_TEST(test_replace_value_from_tree_map_no_destruct);
    RUN_TEST(test_replace_value_from_tree_map_no_mapping_fails);
    RUN_TEST(test_replace_entry_from_tree_map_matching_value);
    RUN_TEST(test_replace_entry_from_tree_map_no_matching_value_fails);

    RUN_TEST(test_remove_entry_from_tree_map);
    RUN_TEST(test_remove_entry_from_tree_map_no_destruct);
    RUN_TEST(test_remove_entry_from_tree_map_no_mapping_fails);
    RUN_TEST(test_remove_entry_from_tree_map_matching_value);
    RUN_TEST(test_remove_entry_from_tree_map_no_matching_value_fails);
    RUN_TEST(test_remove_first_entry_from_tree_map);
    RUN_TEST(test_remove_first_entry_from_tree_map_no_destruct);
    RUN_TEST(test_remove_first_entry_from_empty_tree_map_fails);
    RUN_TEST(test_remove_last_entry_from_tree_map);
    RUN_TEST(test_remove_last_entry_from_tree_map_no_destruct);
    RUN_TEST(test_remove_last_entry_from_empty_tree_map_fails);
    RUN_TEST(test_replace_all_values_from_tree_map);

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
    RUN_TEST(test_tree_map_iterator_at_specified_position);
    RUN_TEST(test_tree_map_iterator_at_invalid_position_fails);

    RUN_TEST(test_tree_map_is_equal_to_it_self);
    RUN_TEST(test_tree_map_is_equal_to_another_tree_map);
    RUN_TEST(test_tree_map_is_not_equal_to_another_tree_map_with_different_size);
    RUN_TEST(test_tree_map_is_not_equal_to_another_tree_map_with_different_mappings);
    
    RUN_TEST(test_perform_action_for_each_entry_of_tree_map);
    RUN_TEST(test_clear_tree_map);

    RUN_TEST(test_get_higher_entry_from_tree_map);
    RUN_TEST(test_get_ceiling_entry_from_tree_map);
    RUN_TEST(test_get_floor_entry_from_tree_map);
    RUN_TEST(test_get_lower_entry_from_tree_map);
    
    RUN_TEST(test_tree_map_contains_entry);
    RUN_TEST(test_tree_map_does_not_contains_entry);
    RUN_TEST(test_tree_map_contains_key);
    RUN_TEST(test_tree_map_does_not_contains_key);
    RUN_TEST(test_tree_map_contains_value);
    RUN_TEST(test_tree_map_does_not_contains_value);

    RUN_TEST(test_get_tree_map_entries);
    RUN_TEST(test_get_tree_map_keys);
    RUN_TEST(test_get_tree_map_values);

    RUN_TEST(test_create_head_map_from_tree_map);
    RUN_TEST(test_create_tail_map_from_tree_map);

    RUN_TEST(test_create_sub_map_of_tree_map);
    RUN_TEST(test_create_empty_sub_map_of_tree_map);
    RUN_TEST(test_create_sub_map_inexistent_end_key_fails);
    RUN_TEST(test_create_sub_map_inexistent_start_key_fails);
    RUN_TEST(test_create_sub_map_start_key_greater_than_end_key_fails);

    RUN_TEST(test_clone_tree_map);
    RUN_TEST(test_get_tree_map_string_representation);
    RUN_TEST(test_get_empty_tree_map_string_representation);
    return UNITY_END();
}