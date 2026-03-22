#include "tree_set_test.h"

#include "set/tree_set.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static TreeSet* tree_set;

void setUp() {
    tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
}

void tearDown() {
    tree_set_set_destructor(tree_set, free);
    tree_set_destroy(&tree_set);
}

void test_create_tree_set() {
    TEST_ASSERT_NOT_NULL(tree_set);
}

void test_do_not_create_tree_set_with_invalid_options() {
    // when
    TreeSet* new_tree_set; Error error = attempt(new_tree_set = tree_set_new(&(TreeSetOptions) {}));
    // then
    TEST_ASSERT_NULL(new_tree_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_tree_set_set_it_to_null() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // when
    tree_set_destroy(&new_tree_set);
    // then
    TEST_ASSERT_NULL(new_tree_set);
}

void test_destroy_null_tree_set_fails() {
    // given
    TreeSet* new_tree_set = nullptr;
    // when
    Error error = attempt(tree_set_destroy(&new_tree_set));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_to_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool added = tree_set_add(tree_set, new(int, 10));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_do_not_add_element_to_tree_set_if_already_present() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool added = tree_set_add(tree_set, new(int, 3));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_get_first_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_get_first(tree_set);
    // then
    TEST_ASSERT_EQUAL(1, *element);
}

void test_get_first_element_from_empty_tree_set_fails() {
    // when
    int* element; Error error = attempt(element = tree_set_get_first(tree_set));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_last_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_get_last(tree_set);
    // then
    TEST_ASSERT_EQUAL(5, *element);
}

void test_get_last_element_from_empty_tree_set_fails() {
    // when
    int* element; Error error = attempt(element = tree_set_get_last(tree_set));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool removed = tree_set_remove(tree_set, &(int){3});
    // then
    int new_elements[] = { 1, 2, 4, 5 };
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){3}));
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_do_not_remove_element_from_tree_set_if_not_present() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool removed = tree_set_remove(tree_set, &(int){10});
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_get_tree_set_size() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int size = tree_set_size(tree_set);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

void test_tree_set_is_empty() {
    // when
    bool empty = tree_set_is_empty(tree_set);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_tree_set_is_not_empty() {
    // given
    tree_set_add(tree_set, new(int, 10));
    // when
    bool empty = tree_set_is_empty(tree_set);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_tree_set_contains_element() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool contains = tree_set_contains(tree_set, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_set_does_not_contains_element() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool contains = tree_set_contains(tree_set, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_tree_set);
    RUN_TEST(test_do_not_create_tree_set_with_invalid_options);

    RUN_TEST(test_destroy_tree_set_set_it_to_null);
    RUN_TEST(test_destroy_null_tree_set_fails);

    RUN_TEST(test_add_element_to_tree_set);
    RUN_TEST(test_do_not_add_element_to_tree_set_if_already_present);

    RUN_TEST(test_get_first_element_from_tree_set);
    RUN_TEST(test_get_first_element_from_empty_tree_set_fails);
    RUN_TEST(test_get_last_element_from_tree_set);
    RUN_TEST(test_get_last_element_from_empty_tree_set_fails);

    RUN_TEST(test_remove_element_from_tree_set);
    RUN_TEST(test_do_not_remove_element_from_tree_set_if_not_present);

    RUN_TEST(test_get_tree_set_size);
    RUN_TEST(test_tree_set_is_empty);
    RUN_TEST(test_tree_set_is_not_empty);
    
    RUN_TEST(test_tree_set_contains_element);
    RUN_TEST(test_tree_set_does_not_contains_element);
    return UNITY_END();
}