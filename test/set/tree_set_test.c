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

void test_create_tree_set_from_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    TreeSet* new_tree_set = tree_set_from(tree_set_to_collection(tree_set), INT_TREE_SET_OPTIONS());
    // then
    TEST_ASSERT_NOT_NULL(new_tree_set);
    TEST_ASSERT_TREE_SET_CONTAINS(new_tree_set, elements);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_do_not_create_tree_set_with_invalid_options_from_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    TreeSet* new_tree_set; Error error = attempt(new_tree_set = tree_set_from(tree_set_to_collection(tree_set), &(TreeSetOptions) {}));
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

void test_add_all_elements_to_tree_set() {
    // given
    TreeSet* existing_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    int other_elements[] = { 10, 20, 30 };
    POPULATE_TREE_SET(existing_tree_set, other_elements);
    // when
    bool changed = tree_set_add_all(tree_set, tree_set_to_collection(existing_tree_set));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 10, 20, 30 };
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_do_not_add_all_elements_to_tree_set_if_already_present() {
    // given
    TreeSet* existing_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    POPULATE_TREE_SET(existing_tree_set, elements);
    // when
    bool changed = tree_set_add_all(tree_set, tree_set_to_collection(existing_tree_set));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(changed);
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

void test_remove_first_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_remove_first(tree_set);
    // then
    int new_elements[] = { 2, 3, 4, 5 };
    TEST_ASSERT_EQUAL(1, *element);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){1}));
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_remove_first_element_from_empty_tree_set_fails() {
    // when
    int* element; Error error = attempt(element = tree_set_remove_first(tree_set));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_last_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_remove_last(tree_set);
    // then
    int new_elements[] = { 1, 2, 3, 4 };
    TEST_ASSERT_EQUAL(5, *element);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){5}));
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
}

void test_remove_last_element_from_empty_tree_set_fails() {
    // when
    int* element; Error error = attempt(element = tree_set_remove_last(tree_set));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
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

void test_tree_set_iterator_forward_iteration() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    Iterator* iterator = tree_set_iterator(tree_set);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_backward_iteration() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_advance(iterator, 5);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_previous(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_previous(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_previous(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_detects_concurrent_modification() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    Iterator* iterator = tree_set_iterator(tree_set);
    tree_set_clear(tree_set);
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_previous(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_remove(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_after_next() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // 1
    // then
    int new_elements[] = { 2, 3, 4, 5 };
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){1}));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_after_previous() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_advance(iterator, 5);
    // when
    iterator_remove(iterator); // 5
    // then
    int new_elements[] = { 1, 2, 3, 4 };
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){5}));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_fails_if_no_next_or_previous_was_called() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_fails_if_called_twice_in_a_row() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_next(iterator);
    iterator_remove(iterator);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    int new_elements[] = { 2, 3, 4, 5 };
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_SET_CONTAINS(tree_set, new_elements);
    TEST_ASSERT_FALSE(tree_set_contains(tree_set, &(int){1}));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_reset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_is_equal_to_it_self() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    bool equals = tree_set_equals(tree_set, tree_set);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_tree_set_is_equal_to_another_tree_set() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    POPULATE_TREE_SET(other_tree_set, elements);
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    tree_set_set_destructor(other_tree_set, free);
    tree_set_destroy(&other_tree_set);
}

void test_tree_set_is_not_equal_to_another_tree_set_with_different_size() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    POPULATE_TREE_SET(other_tree_set, elements);
    tree_set_add(other_tree_set, new(int, 10));
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_set_set_destructor(other_tree_set, free);
    tree_set_destroy(&other_tree_set);
}

void test_tree_set_is_not_equal_to_another_tree_set_with_different_elements() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    int other_elements[] = { 2, 3, 4, 5, 6 };
    POPULATE_TREE_SET(other_tree_set, other_elements);
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_set_set_destructor(other_tree_set, free);
    tree_set_destroy(&other_tree_set);
}

static int action_count = 0;

static void action(void* element) {
    action_count++;
}

void test_perform_action_for_each_element_of_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    tree_set_for_each(tree_set, action);
    // then
    TEST_ASSERT_EQUAL(5, action_count);
}

void test_clear_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    tree_set_clear(tree_set);
    // then
    TEST_ASSERT_EQUAL(0, tree_set_size(tree_set));
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS(tree_set, elements);
}

void test_get_higher_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_higher(tree_set, &(int){3});
    int* not_found = tree_set_higher(tree_set, &(int){6});
    // then
    TEST_ASSERT_EQUAL(4, *element);
    TEST_ASSERT_NULL(not_found);
}

void test_get_ceiling_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_ceiling(tree_set, &(int){4});
    int* not_found = tree_set_ceiling(tree_set, &(int){6});
    // then
    TEST_ASSERT_EQUAL(5, *element);
    TEST_ASSERT_NULL(not_found);
}

void test_get_floor_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_floor(tree_set, &(int){1});
    int* not_found = tree_set_floor(tree_set, &(int){0});
    // then
    TEST_ASSERT_EQUAL(1, *element);
    TEST_ASSERT_NULL(not_found);
}

void test_get_lower_element_from_tree_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    int* element = tree_set_lower(tree_set, &(int){3});
    int* not_found = tree_set_lower(tree_set, &(int){0});
    // then
    TEST_ASSERT_EQUAL(2, *element);
    TEST_ASSERT_NULL(not_found);
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

void test_tree_set_contains_all_elements() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // and
    int other_elements[] = { 2, 3, 4};
    POPULATE_TREE_SET(new_tree_set, other_elements);
    // when
    bool contains_all = tree_set_contains_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    tree_set_set_destructor(new_tree_set, free);
    tree_set_destroy(&new_tree_set);
}

void test_empty_tree_set_contains_all_elements_of_empty_collection() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS());
    // when
    bool contains_all = tree_set_contains_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_tree_set_to_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_TREE_SET(tree_set, elements);
    // when
    Collection collection = tree_set_to_collection(tree_set);
    // then
    Iterator* iterator = collection_iterator(collection);
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_tree_set);
    RUN_TEST(test_do_not_create_tree_set_with_invalid_options);
    RUN_TEST(test_create_tree_set_from_collection);
    RUN_TEST(test_do_not_create_tree_set_with_invalid_options_from_collection);

    RUN_TEST(test_destroy_tree_set_set_it_to_null);
    RUN_TEST(test_destroy_null_tree_set_fails);

    RUN_TEST(test_add_element_to_tree_set);
    RUN_TEST(test_do_not_add_element_to_tree_set_if_already_present);
    RUN_TEST(test_add_all_elements_to_tree_set);
    RUN_TEST(test_do_not_add_all_elements_to_tree_set_if_already_present);

    RUN_TEST(test_get_first_element_from_tree_set);
    RUN_TEST(test_get_first_element_from_empty_tree_set_fails);
    RUN_TEST(test_get_last_element_from_tree_set);
    RUN_TEST(test_get_last_element_from_empty_tree_set_fails);

    RUN_TEST(test_remove_element_from_tree_set);
    RUN_TEST(test_do_not_remove_element_from_tree_set_if_not_present);

    RUN_TEST(test_remove_first_element_from_tree_set);
    RUN_TEST(test_remove_first_element_from_empty_tree_set_fails);
    RUN_TEST(test_remove_last_element_from_tree_set);
    RUN_TEST(test_remove_last_element_from_empty_tree_set_fails);

    RUN_TEST(test_get_tree_set_size);
    RUN_TEST(test_tree_set_is_empty);
    RUN_TEST(test_tree_set_is_not_empty);
    
    RUN_TEST(test_tree_set_iterator_forward_iteration);
    RUN_TEST(test_tree_set_iterator_backward_iteration);
    RUN_TEST(test_tree_set_iterator_detects_concurrent_modification);
    RUN_TEST(test_tree_set_iterator_remove_element_after_next);
    RUN_TEST(test_tree_set_iterator_remove_element_after_previous);
    RUN_TEST(test_tree_set_iterator_remove_element_fails_if_no_next_or_previous_was_called);
    RUN_TEST(test_tree_set_iterator_remove_element_fails_if_called_twice_in_a_row);
    RUN_TEST(test_tree_set_iterator_reset);

    RUN_TEST(test_tree_set_is_equal_to_it_self);
    RUN_TEST(test_tree_set_is_equal_to_another_tree_set);
    RUN_TEST(test_tree_set_is_not_equal_to_another_tree_set_with_different_size);
    RUN_TEST(test_tree_set_is_not_equal_to_another_tree_set_with_different_elements);
    RUN_TEST(test_perform_action_for_each_element_of_tree_set);
    RUN_TEST(test_clear_tree_set);

    RUN_TEST(test_get_higher_element_from_tree_set);
    RUN_TEST(test_get_ceiling_element_from_tree_set);
    RUN_TEST(test_get_floor_element_from_tree_set);
    RUN_TEST(test_get_lower_element_from_tree_set);

    RUN_TEST(test_tree_set_contains_element);
    RUN_TEST(test_tree_set_does_not_contains_element);
    RUN_TEST(test_tree_set_contains_all_elements);
    RUN_TEST(test_empty_tree_set_contains_all_elements_of_empty_collection);

    RUN_TEST(test_tree_set_to_collection);
    return UNITY_END();
}