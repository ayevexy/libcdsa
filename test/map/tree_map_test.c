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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_tree_map);
    RUN_TEST(test_do_not_create_tree_map_with_invalid_options);
    RUN_TEST(test_destroy_tree_map_set_it_to_null);
    RUN_TEST(test_destroy_null_tree_map_fails);
    return UNITY_END();
}