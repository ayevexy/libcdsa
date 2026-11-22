#include "collections/set/hash_set.h"

#include "collections/collection.h"
#include "collections/iterator.h"
#include "collections/callbacks.h"
#include "core/errors.h"
#include "core/memory.h"
#include "core/array.h"
#include "core/string.h"

#include "test_utilities.h"
#include "unity.h"

#define POPULATE_HASH_SET(hash_set, T, ...)                 \
    do {                                                    \
        auto array = (T[]){__VA_ARGS__};                    \
        int length = COUNT_ARGS(__VA_ARGS__);               \
        for (int i = 0; i < length; i++) {                  \
            hash_set_add(hash_set, new(int, array[i]));     \
        }                                                   \
    } while (false)

#define TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, T, ...)        \
    do {                                                                \
        auto array = (T[]){__VA_ARGS__};                                \
        int length = COUNT_ARGS(__VA_ARGS__);                           \
        for (int i = 0; i < length; i++) {                              \
            TEST_ASSERT_TRUE(hash_set_contains(hash_set, &array[i]));   \
        }                                                               \
    } while (false)

#define TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, T, ...)     \
    do {                                                                    \
        auto array = (T[]){__VA_ARGS__};                                    \
        int length = COUNT_ARGS(__VA_ARGS__);                               \
        for (int i = 0; i < length; i++) {                                  \
            TEST_ASSERT_FALSE(hash_set_contains(hash_set, &array[i]));      \
        }                                                                   \
    } while (false)

static HashSet* hash_set;

void setUp(void) {
    hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    hash_set_change_destructor(hash_set, delete);
}

void tearDown(void) {
    hash_set_destroy(&hash_set);
}

static void test_create_hash_set(void) {
    TEST_ASSERT_NOT_NULL(hash_set);
}

static void test_do_not_create_hash_set_with_invalid_options(void) {
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_new(ref(HashSetOptions)));
    // then
    TEST_ASSERT_NULL(new_hash_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

static void test_create_hash_set_from_collection(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* new_hash_set = hash_set_from(hash_set_to_collection(hash_set), HASH_SET_OPTIONS(int));
    // then
    TEST_ASSERT_NOT_NULL(new_hash_set);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(new_hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    hash_set_destroy(&new_hash_set);
}

static void test_do_not_create_hash_set_with_invalid_options_from_collection(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_from(hash_set_to_collection(hash_set), ref(HashSetOptions)));
    // then
    TEST_ASSERT_NULL(new_hash_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

static void test_destroy_hash_set_set_it_to_null(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // when
    hash_set_destroy(&new_hash_set);
    // then
    TEST_ASSERT_NULL(new_hash_set);
}

static void test_destroy_null_hash_set_fails(void) {
    // given
    HashSet* new_hash_set = nullptr;
    // when
    Error error = attempt(hash_set_destroy(&new_hash_set));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

static void test_add_element_to_hash_set(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = hash_set_add(hash_set, new(int, 10));
    // then
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5, 10);
}

static void test_do_not_add_element_to_hash_set_if_already_present(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = hash_set_add(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

static void test_add_all_elements_to_hash_set(void) {
    // given
    HashSet* existing_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(existing_hash_set, int, 10, 20, 30);
    // when
    bool changed = hash_set_add_all(hash_set, hash_set_to_collection(existing_hash_set));
    // then
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5, 10, 20, 30);
    // clean up
    hash_set_destroy(&existing_hash_set);
}

static void test_do_not_add_all_elements_to_hash_set_if_already_present(void) {
    // given
    HashSet* existing_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(existing_hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool changed = hash_set_add_all(hash_set, hash_set_to_collection(existing_hash_set));
    // then
    TEST_ASSERT_FALSE(changed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    hash_set_change_destructor(existing_hash_set, delete);
    hash_set_destroy(&existing_hash_set);
}

static void test_add_elements_to_hash_set_exceeding_threshold_resizes_it(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    // when
    hash_set_add(hash_set, new(int, 13));
    // then
    TEST_ASSERT_EQUAL(32, hash_set_capacity(hash_set));
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
}

static void test_remove_element_from_hash_set(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = hash_set_remove(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 4, 5);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 3);
}

static void test_do_not_remove_element_from_hash_set_if_not_present(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = hash_set_remove(hash_set,ref(int, 10));
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

static void test_remove_all_elements_from_hash_set_matching_collection(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(new_hash_set, int, 2, 3, 4);
    // when
    int count = hash_set_remove_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 5);
    TEST_ASSERT_EQUAL(3, count);
    // clean up
    hash_set_change_destructor(new_hash_set, delete);
    hash_set_destroy(&new_hash_set);
}

static bool is_odd(const void* element) {
    return *(int*) element % 2 != 0;
}

static void test_remove_elements_from_hash_set_matching_predicate(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int count = hash_set_remove_if(hash_set, is_odd);
    // then
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 2, 4);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1, 3, 5);
}

static void test_retain_all_elements_from_collection_in_hash_set(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(new_hash_set, int, 2, 4);
    // when
    int count = hash_set_retain_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 2, 4);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1, 3, 5);
    // clean up
    hash_set_change_destructor(new_hash_set, delete);
    hash_set_destroy(&new_hash_set);
}

static void test_get_hash_set_size(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int size = hash_set_size(hash_set);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

static void test_get_hash_set_capacity(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int capacity = hash_set_capacity(hash_set);
    // then
    TEST_ASSERT_EQUAL(16, capacity);
}

static void test_hash_set_is_empty(void) {
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_TRUE(empty);
}

static void test_hash_set_is_not_empty(void) {
    // given
    hash_set_add(hash_set, new(int, 10));
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_FALSE(empty);
}

static void test_hash_set_iterator_forward_iteration(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
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

static void test_hash_set_iterator_backward_iteration_unsupported(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
    // then
    TEST_ASSERT_EQUAL(UNSUPPORTED_OPERATION_ERROR, attempt(iterator_has_previous(iterator)));
    TEST_ASSERT_EQUAL(UNSUPPORTED_OPERATION_ERROR, attempt(iterator_previous(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_iterator_detects_concurrent_modification(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
    hash_set_clear(hash_set);
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_remove(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_iterator_remove_element(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // 5
    // then
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 2, 3, 4, 5);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1);
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_iterator_remove_element_fails_if_no_next_was_called(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_iterator_remove_element_fails_if_called_twice_in_a_row(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    iterator_next(iterator);
    iterator_remove(iterator);
    // when
    Error error = attempt(iterator_remove(iterator));
    // then
    TEST_ASSERT_EQUAL(ILLEGAL_STATE_ERROR, error);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 2, 3, 4, 5);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1);
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_iterator_reset(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

static void test_hash_set_is_equal_to_it_self(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool equals = hash_set_equals(hash_set, hash_set);
    // then
    TEST_ASSERT_TRUE(equals);
}

static void test_hash_set_is_equal_to_another_hash_set(void) {
    // given
    HashSet* other_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(other_hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    hash_set_change_destructor(other_hash_set, delete);
    hash_set_destroy(&other_hash_set);
}

static void test_hash_set_is_not_equal_to_another_hash_set_with_different_size(void) {
    // given
    HashSet* other_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(other_hash_set, int, 1, 2, 3, 4, 5);
    // and
    hash_set_add(other_hash_set, new(int, 10));
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    hash_set_change_destructor(other_hash_set, delete);
    hash_set_destroy(&other_hash_set);
}

static void test_hash_set_is_not_equal_to_another_hash_set_with_different_elements(void) {
    // given
    HashSet* other_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(other_hash_set, int, 2, 3, 4, 5, 6);
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    hash_set_change_destructor(other_hash_set, delete);
    hash_set_destroy(&other_hash_set);
}

static int action_count = 0;

static void action(void*) {
    action_count++;
}

static void test_perform_action_for_each_element_of_hash_set(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    hash_set_for_each(hash_set, action);
    // then
    TEST_ASSERT_EQUAL(5, action_count);
}

static void test_clear_hash_set(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    hash_set_clear(hash_set);
    // then
    TEST_ASSERT_EQUAL(0, hash_set_size(hash_set));
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

static void test_hash_set_contains_element(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = hash_set_contains(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

static void test_hash_set_does_not_contains_element(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = hash_set_contains(hash_set,ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

static void test_hash_set_contains_all_elements(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(new_hash_set, int, 2, 3, 4);
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    hash_set_change_destructor(new_hash_set, delete);
    hash_set_destroy(&new_hash_set);
}

static void test_empty_hash_set_contains_all_elements_of_empty_collection(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    hash_set_destroy(&new_hash_set);
}

static void test_hash_set_does_not_contains_all_elements(void) {
    // given
    HashSet* new_hash_set = hash_set_new(HASH_SET_OPTIONS(int));
    // and
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    POPULATE_HASH_SET(new_hash_set, int, 2, 10, 4);
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    hash_set_change_destructor(new_hash_set, delete);
    hash_set_destroy(&new_hash_set);
}

static void test_clone_hash_set(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* copy_hash_set = hash_set_clone(hash_set);
    // then
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(copy_hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    hash_set_destroy(&copy_hash_set);
}

static void test_hash_set_to_collection(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Collection collection = hash_set_to_collection(hash_set);
    // then
    Iterator* iterator = collection_iterator(collection); // iteration order is not guaranteed
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

static void test_convert_hash_set_to_array(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) element_array = hash_set_to_array(hash_set);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(element_array, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&element_array);
}

static void test_get_hash_set_string_representation(void) {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    String string = hash_set_to_string(hash_set);
    // then
    TEST_ASSERT_EQUAL_STRING("( 1, 2, 3, 4, 5 )", string_data(string));
    // clean up
    string_destroy(&string);
}

static void test_get_empty_hash_set_string_representation(void) {
    // when
    String string = hash_set_to_string(hash_set);
    // then
    TEST_ASSERT_EQUAL_STRING("()", string_data(string));
    // clean up
    string_destroy(&string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_set);
    RUN_TEST(test_do_not_create_hash_set_with_invalid_options);
    RUN_TEST(test_create_hash_set_from_collection);
    RUN_TEST(test_do_not_create_hash_set_with_invalid_options_from_collection);

    RUN_TEST(test_destroy_hash_set_set_it_to_null);
    RUN_TEST(test_destroy_null_hash_set_fails);

    RUN_TEST(test_add_element_to_hash_set);
    RUN_TEST(test_do_not_add_element_to_hash_set_if_already_present);
    RUN_TEST(test_add_all_elements_to_hash_set);
    RUN_TEST(test_do_not_add_all_elements_to_hash_set_if_already_present);
    RUN_TEST(test_add_elements_to_hash_set_exceeding_threshold_resizes_it);

    RUN_TEST(test_remove_element_from_hash_set);
    RUN_TEST(test_do_not_remove_element_from_hash_set_if_not_present);
    RUN_TEST(test_remove_all_elements_from_hash_set_matching_collection);
    RUN_TEST(test_remove_elements_from_hash_set_matching_predicate);
    RUN_TEST(test_retain_all_elements_from_collection_in_hash_set);

    RUN_TEST(test_get_hash_set_size);
    RUN_TEST(test_get_hash_set_capacity);
    RUN_TEST(test_hash_set_is_empty);
    RUN_TEST(test_hash_set_is_not_empty);

    RUN_TEST(test_hash_set_iterator_forward_iteration);
    RUN_TEST(test_hash_set_iterator_backward_iteration_unsupported);
    RUN_TEST(test_hash_set_iterator_detects_concurrent_modification);
    RUN_TEST(test_hash_set_iterator_remove_element);
    RUN_TEST(test_hash_set_iterator_remove_element_fails_if_no_next_was_called);
    RUN_TEST(test_hash_set_iterator_remove_element_fails_if_called_twice_in_a_row);
    RUN_TEST(test_hash_set_iterator_reset);

    RUN_TEST(test_hash_set_is_equal_to_it_self);
    RUN_TEST(test_hash_set_is_equal_to_another_hash_set);
    RUN_TEST(test_hash_set_is_not_equal_to_another_hash_set_with_different_size);
    RUN_TEST(test_hash_set_is_not_equal_to_another_hash_set_with_different_elements);
    RUN_TEST(test_perform_action_for_each_element_of_hash_set);
    RUN_TEST(test_clear_hash_set);

    RUN_TEST(test_hash_set_contains_element);
    RUN_TEST(test_hash_set_does_not_contains_element);
    RUN_TEST(test_hash_set_contains_all_elements);
    RUN_TEST(test_empty_hash_set_contains_all_elements_of_empty_collection);
    RUN_TEST(test_hash_set_does_not_contains_all_elements);

    RUN_TEST(test_clone_hash_set);
    RUN_TEST(test_hash_set_to_collection);
    RUN_TEST(test_convert_hash_set_to_array);
    RUN_TEST(test_get_hash_set_string_representation);
    RUN_TEST(test_get_empty_hash_set_string_representation);
    return UNITY_END();
}