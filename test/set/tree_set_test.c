#include "set/tree_set.h"

#include "../test_utilities.h"
#include "unity.h"

#define INT_TREE_SET_OPTIONS &(TreeSetOptions) {    \
    .destruct = noop_destruct,                      \
    .compare = int_pointer_value_compare,           \
    .to_string = int_pointer_value_to_string,       \
    .memory_alloc = memory_try_alloc,               \
    .memory_dealloc = memory_dealloc                \
}

#define POPULATE_TREE_SET(tree_set, T, ...)                 \
    do {                                                    \
        auto array = (T[]){__VA_ARGS__};                    \
        int length = count_args(__VA_ARGS__);               \
        for (int i = 0; i < length; i++) {                  \
            tree_set_add(tree_set, new(int, array[i]));     \
        }                                                   \
    } while (false)

#define TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, T, ...)        \
    do {                                                                \
        auto array = (T[]){__VA_ARGS__};;                               \
        int length = count_args(__VA_ARGS__);                           \
        for (int i = 0; i < length; i++) {                              \
            TEST_ASSERT_TRUE(tree_set_contains(tree_set, &array[i]));   \
        }                                                               \
    } while (false)

#define TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, T, ...)     \
    do {                                                                    \
        auto array = (T[]){__VA_ARGS__};;                                   \
        int length = count_args(__VA_ARGS__);                               \
        for (int i = 0; i < length; i++) {                                  \
            TEST_ASSERT_FALSE(tree_set_contains(tree_set, &array[i]));      \
        }                                                                   \
    } while (false)

static TreeSet* tree_set;

void setUp() {
    tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    tree_set_change_destructor(tree_set, delete);
}

void tearDown() {
    tree_set_destroy(&tree_set);
}

void test_create_tree_set() {
    TEST_ASSERT_NOT_NULL(tree_set);
}

void test_do_not_create_tree_set_with_invalid_options() {
    // when
    TreeSet* new_tree_set; Error error = attempt(new_tree_set = tree_set_new(ref(TreeSetOptions)));
    // then
    TEST_ASSERT_NULL(new_tree_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_tree_set_from_collection() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set = tree_set_from(tree_set_to_collection(tree_set), INT_TREE_SET_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_tree_set);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(new_tree_set, int, 1, 2, 3, 4, 5);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_do_not_create_tree_set_with_invalid_options_from_collection() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set; Error error = attempt(new_tree_set = tree_set_from(tree_set_to_collection(tree_set), ref(TreeSetOptions)));
    // then
    TEST_ASSERT_NULL(new_tree_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_tree_set_set_it_to_null() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = tree_set_add(tree_set, new(int, 10));
    // then
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5, 10);
}

void test_do_not_add_element_to_tree_set_if_already_present() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = tree_set_add(tree_set, ref(int, 3));
    // then
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5);
}

void test_add_all_elements_to_tree_set() {
    // given
    TreeSet* existing_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(existing_tree_set, int, 10, 20, 30);
    // when
    bool changed = tree_set_add_all(tree_set, tree_set_to_collection(existing_tree_set));
    // then
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5, 10, 20, 30);
    // clean up
    tree_set_destroy(&existing_tree_set);
}

void test_do_not_add_all_elements_to_tree_set_if_already_present() {
    // given
    TreeSet* existing_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(existing_tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool changed = tree_set_add_all(tree_set, tree_set_to_collection(existing_tree_set));
    // then
    TEST_ASSERT_FALSE(changed);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5);
    // clean up
    tree_set_change_destructor(existing_tree_set, delete);
    tree_set_destroy(&existing_tree_set);
}

void test_get_first_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = tree_set_remove(tree_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 4, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 3);
}

void test_do_not_remove_element_from_tree_set_if_not_present() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = tree_set_remove(tree_set, ref(int, 10));
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5);
}

void test_remove_first_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    tree_set_remove_first(tree_set);
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 3, 4, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1);
}

void test_remove_first_element_from_tree_set_no_destruct() {
    // given
    tree_set_change_destructor(tree_set, noop_destruct);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    int* element = tree_set_remove_first(tree_set);
    // then
    TEST_ASSERT_EQUAL(1, *element);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 3, 4, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1);
    // clean up
    delete(element);
    tree_set_change_destructor(tree_set, delete);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    tree_set_remove_last(tree_set);
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 5);
}

void test_remove_last_element_from_tree_set_no_destruct() {
    // given
    tree_set_change_destructor(tree_set, noop_destruct);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    int* element = tree_set_remove_last(tree_set);
    // then
    TEST_ASSERT_EQUAL(5, *element);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 5);
    // clean up
    delete(element);
    tree_set_change_destructor(tree_set, delete);
}

void test_remove_last_element_from_empty_tree_set_fails() {
    // when
    int* element; Error error = attempt(element = tree_set_remove_last(tree_set));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_all_elements_from_tree_set_matching_collection() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(new_tree_set, int, 2, 3, 4);
    // when
    int count = tree_set_remove_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 2, 3, 4);
    TEST_ASSERT_EQUAL(3, count);
    // clean up
    tree_set_change_destructor(new_tree_set, delete);
    tree_set_destroy(&new_tree_set);
}

static bool is_odd(const void* element) {
    return *(int*) element % 2 != 0;
}

void test_remove_elements_from_tree_set_matching_predicate() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    int count = tree_set_remove_if(tree_set, is_odd);
    // then
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 4);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1, 3, 5);
}

void test_retain_all_elements_from_collection_in_tree_set() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(new_tree_set, int, 2, 4);
    // when
    int count = tree_set_retain_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 4);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1, 3, 5);
    // clean up
    tree_set_change_destructor(new_tree_set, delete);
    tree_set_destroy(&new_tree_set);
}

void test_get_tree_set_size() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // 1
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 3, 4, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_after_previous() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_advance(iterator, 5);
    // when
    iterator_remove(iterator); // 5
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 5);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_fails_if_no_next_or_previous_was_called() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_remove_element_fails_if_called_twice_in_a_row() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = tree_set_iterator(tree_set);
    iterator_next(iterator);
    iterator_remove(iterator);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(tree_set, int, 2, 3, 4, 5);
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1);
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_reset() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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

void test_tree_set_iterator_at_specified_position() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = tree_set_iterator_at(tree_set, 3);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_tree_set_iterator_at_invalid_position_fails() {
    // given
    Iterator* iterator;
    // then
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = tree_set_iterator_at(tree_set, -1)));
    TEST_ASSERT_NULL(iterator);
    // and
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = tree_set_iterator_at(tree_set, 6)));
    TEST_ASSERT_NULL(iterator);
}

void test_tree_set_is_equal_to_it_self() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool equals = tree_set_equals(tree_set, tree_set);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_tree_set_is_equal_to_another_tree_set() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(other_tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    tree_set_change_destructor(other_tree_set, delete);
    tree_set_destroy(&other_tree_set);
}

void test_tree_set_is_not_equal_to_another_tree_set_with_different_size() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(other_tree_set, int, 1, 2, 3, 4, 5);
    // and
    tree_set_add(other_tree_set, new(int, 10));
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_set_change_destructor(other_tree_set, delete);
    tree_set_destroy(&other_tree_set);
}

void test_tree_set_is_not_equal_to_another_tree_set_with_different_elements() {
    // given
    TreeSet* other_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(other_tree_set, int, 2, 3, 4, 5, 6);
    // when
    bool equals = tree_set_equals(tree_set, other_tree_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    tree_set_change_destructor(other_tree_set, delete);
    tree_set_destroy(&other_tree_set);
}

static int action_count = 0;

static void action(void* element) {
    action_count++;
}

void test_perform_action_for_each_element_of_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    tree_set_for_each(tree_set, action);
    // then
    TEST_ASSERT_EQUAL(5, action_count);
}

void test_clear_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    tree_set_clear(tree_set);
    // then
    TEST_ASSERT_EQUAL(0, tree_set_size(tree_set));
    TEST_ASSERT_TREE_SET_DO_NOT_CONTAINS_ELEMENTS(tree_set, int, 1, 2, 3, 4, 5);
}

void test_get_higher_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 4, 5);;
    // when
    int* next_greater = tree_set_higher(tree_set, ref(int, 3));
    int* no_successor = tree_set_higher(tree_set, ref(int, 5));
    // then
    TEST_ASSERT_EQUAL(4, *next_greater);
    TEST_ASSERT_NULL(no_successor);
}

void test_get_ceiling_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 5);
    // when
    int* exact_match = tree_set_ceiling(tree_set, ref(int, 5));
    int* next_greater = tree_set_ceiling(tree_set, ref(int, 4));
    int* no_successor = tree_set_ceiling(tree_set, ref(int, 6));
    // then
    TEST_ASSERT_EQUAL(5, *exact_match);
    TEST_ASSERT_EQUAL(5, *next_greater);
    TEST_ASSERT_NULL(no_successor);
}

void test_get_floor_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 3, 4, 5);
    // when
    int* exact_match = tree_set_floor(tree_set, ref(int, 1));
    int* next_least = tree_set_floor(tree_set, ref(int, 2));
    int* no_predecessor = tree_set_floor(tree_set, ref(int, 0));
    // then
    TEST_ASSERT_EQUAL(1, *exact_match);
    TEST_ASSERT_EQUAL(1, *next_least);
    TEST_ASSERT_NULL(no_predecessor);
}

void test_get_lower_element_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 4, 5);
    // when
    int* next_least = tree_set_lower(tree_set, ref(int, 3));
    int* no_predecessor = tree_set_lower(tree_set, ref(int, 1));
    // then
    TEST_ASSERT_EQUAL(2, *next_least);
    TEST_ASSERT_NULL(no_predecessor);
}

void test_tree_set_contains_element() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = tree_set_contains(tree_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_tree_set_does_not_contains_element() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = tree_set_contains(tree_set, ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_tree_set_contains_all_elements() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // and
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    POPULATE_TREE_SET(new_tree_set, int, 2, 3, 4);
    // when
    bool contains_all = tree_set_contains_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    tree_set_change_destructor(new_tree_set, delete);
    tree_set_destroy(&new_tree_set);
}

void test_empty_tree_set_contains_all_elements_of_empty_collection() {
    // given
    TreeSet* new_tree_set = tree_set_new(INT_TREE_SET_OPTIONS);
    // when
    bool contains_all = tree_set_contains_all(tree_set, tree_set_to_collection(new_tree_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_create_head_set_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set = tree_set_head_set(tree_set, ref(int, 3));
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(new_tree_set, int, 1, 2, 3);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_create_tail_set_from_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set = tree_set_tail_set(tree_set, ref(int, 3));
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(new_tree_set, int, 3, 4, 5);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_create_sub_set_of_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set = tree_set_sub_set(tree_set, ref(int, 2), ref(int, 5));
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(new_tree_set, int, 2, 3, 4);
    // clean up
    tree_set_destroy(&new_tree_set);
}

void test_create_empty_sub_set_of_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* new_tree_set = tree_set_sub_set(tree_set, ref(int, 3), ref(int, 3));
    // then
    TEST_ASSERT_EQUAL(0, tree_set_size(new_tree_set));
    // clean up
    tree_set_destroy(&new_tree_set);
}

static void sub_set_inexistent_element_test_helper(int* start_element, int* end_element) {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* sub_set; Error error = attempt(sub_set = tree_set_sub_set(tree_set, start_element, end_element));
    // then
    TEST_ASSERT_NULL(sub_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_sub_set_inexistent_end_element_fails() {
    sub_set_inexistent_element_test_helper(ref(int, 1), ref(int, 6));
}

void test_create_sub_set_inexistent_start_element_fails() {
    sub_set_inexistent_element_test_helper(ref(int, 0), ref(int, 5));
}

void test_create_sub_set_start_element_greater_than_end_element_fails() {
    sub_set_inexistent_element_test_helper(ref(int, 5), ref(int, 3));
}

void test_clone_tree_set() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    TreeSet* copy_tree_set = tree_set_clone(tree_set);
    // then
    TEST_ASSERT_TREE_SET_CONTAINS_ELEMENTS(copy_tree_set, int, 1, 2, 3, 4, 5);
    // clean up
    tree_set_destroy(&copy_tree_set);
}

void test_tree_set_to_collection() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
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

void test_convert_tree_set_to_array() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) element_array = tree_set_to_array(tree_set);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(element_array, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&element_array);
}

void test_get_tree_set_string_representation() {
    // given
    POPULATE_TREE_SET(tree_set, int, 1, 2, 3, 4, 5);
    // when
    String string = tree_set_to_string(tree_set);
    // then
    TEST_ASSERT_EQUAL_STRING("( 1, 2, 3, 4, 5 )", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_get_empty_tree_set_string_representation() {
    // when
    String string = tree_set_to_string(tree_set);
    // then
    TEST_ASSERT_EQUAL_STRING("()", string_data(string));
    // clean up
    string_destroy(&string);
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
    RUN_TEST(test_remove_first_element_from_tree_set_no_destruct);
    RUN_TEST(test_remove_first_element_from_empty_tree_set_fails);
    RUN_TEST(test_remove_last_element_from_tree_set);
    RUN_TEST(test_remove_last_element_from_tree_set_no_destruct);
    RUN_TEST(test_remove_last_element_from_empty_tree_set_fails);
    RUN_TEST(test_remove_all_elements_from_tree_set_matching_collection);
    RUN_TEST(test_remove_elements_from_tree_set_matching_predicate);
    RUN_TEST(test_retain_all_elements_from_collection_in_tree_set);

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
    RUN_TEST(test_tree_set_iterator_at_specified_position);
    RUN_TEST(test_tree_set_iterator_at_invalid_position_fails);

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
    
    RUN_TEST(test_create_head_set_from_tree_set);
    RUN_TEST(test_create_tail_set_from_tree_set);

    RUN_TEST(test_create_sub_set_of_tree_set);
    RUN_TEST(test_create_empty_sub_set_of_tree_set);
    RUN_TEST(test_create_sub_set_inexistent_end_element_fails);
    RUN_TEST(test_create_sub_set_inexistent_start_element_fails);
    RUN_TEST(test_create_sub_set_start_element_greater_than_end_element_fails);

    RUN_TEST(test_clone_tree_set);
    RUN_TEST(test_tree_set_to_collection);
    RUN_TEST(test_convert_tree_set_to_array);
    RUN_TEST(test_get_tree_set_string_representation);
    RUN_TEST(test_get_empty_tree_set_string_representation);
    return UNITY_END();
}