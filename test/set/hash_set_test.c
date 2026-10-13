#include "set/hash_set.h"

#include "../test_utilities.h"
#include "unity.h"

#define INT_HASH_SET_OPTIONS &(HashSetOptions) {    \
    .initial_capacity = 16,                         \
    .load_factor = 0.75f,                           \
    .hash = char_hash,                              \
    .destruct = noop_destruct,                      \
    .equals = int_pointer_value_equals,             \
    .to_string = int_pointer_value_to_string,       \
    .memory_alloc = memory_try_alloc,               \
    .memory_dealloc = memory_dealloc                \
}

#define POPULATE_HASH_SET(hash_set, T, ...)                 \
    do {                                                    \
        auto array = (T[]){__VA_ARGS__};                    \
        int length = count_args(__VA_ARGS__);               \
        for (int i = 0; i < length; i++) {                  \
            hash_set_add(hash_set, new(int, array[i]));     \
        }                                                   \
    } while (false)

#define TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, T, ...)        \
    do {                                                                \
        auto array = (T[]){__VA_ARGS__};                                \
        int length = count_args(__VA_ARGS__);                           \
        for (int i = 0; i < length; i++) {                              \
            TEST_ASSERT_TRUE(hash_set_contains(hash_set, &array[i]));   \
        }                                                               \
    } while (false)

#define TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, T, ...)     \
    do {                                                                    \
        auto array = (T[]){__VA_ARGS__};                                    \
        int length = count_args(__VA_ARGS__);                               \
        for (int i = 0; i < length; i++) {                                  \
            TEST_ASSERT_FALSE(hash_set_contains(hash_set, &array[i]));      \
        }                                                                   \
    } while (false)

static HashSet* hash_set;

void setUp() {
    hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    hash_set_change_destructor(hash_set, delete);
}

void tearDown() {
    hash_set_destroy(&hash_set);
}

void test_create_hash_set() {
    TEST_ASSERT_NOT_NULL(hash_set);
}

void test_do_not_create_hash_set_with_invalid_options() {
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_new(ref(HashSetOptions)));
    // then
    TEST_ASSERT_NULL(new_hash_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_hash_set_from_collection() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* new_hash_set = hash_set_from(hash_set_to_collection(hash_set), INT_HASH_SET_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_hash_set);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(new_hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    hash_set_destroy(&new_hash_set);
}

void test_do_not_create_hash_set_with_invalid_options_from_collection() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_from(hash_set_to_collection(hash_set), ref(HashSetOptions)));
    // then
    TEST_ASSERT_NULL(new_hash_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_hash_set_set_it_to_null() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // when
    hash_set_destroy(&new_hash_set);
    // then
    TEST_ASSERT_NULL(new_hash_set);
}

void test_destroy_null_hash_set_fails() {
    // given
    HashSet* new_hash_set = nullptr;
    // when
    Error error = attempt(hash_set_destroy(&new_hash_set));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_to_hash_set() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = hash_set_add(hash_set, new(int, 10));
    // then
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5, 10);
}

void test_do_not_add_element_to_hash_set_if_already_present() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool added = hash_set_add(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

void test_add_all_elements_to_hash_set() {
    // given
    HashSet* existing_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_do_not_add_all_elements_to_hash_set_if_already_present() {
    // given
    HashSet* existing_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_add_elements_to_hash_set_exceeding_threshold_resizes_it() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    // when
    hash_set_add(hash_set, new(int, 13));
    // then
    TEST_ASSERT_EQUAL(32, hash_set_capacity(hash_set));
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
}

void test_remove_element_from_hash_set() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = hash_set_remove(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 4, 5);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 3);
}

void test_do_not_remove_element_from_hash_set_if_not_present() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool removed = hash_set_remove(hash_set,ref(int, 10));
    // then
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

void test_remove_all_elements_from_hash_set_matching_collection() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_remove_elements_from_hash_set_matching_predicate() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int count = hash_set_remove_if(hash_set, is_odd);
    // then
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 2, 4);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1, 3, 5);
}

void test_retain_all_elements_from_collection_in_hash_set() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_get_hash_set_size() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int size = hash_set_size(hash_set);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

void test_get_hash_set_capacity() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    int capacity = hash_set_capacity(hash_set);
    // then
    TEST_ASSERT_EQUAL(16, capacity);
}

void test_hash_set_is_empty() {
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_hash_set_is_not_empty() {
    // given
    hash_set_add(hash_set, new(int, 10));
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_hash_set_iterator_forward_iteration() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // and
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
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_set_iterator_backward_iteration_unsupported() {
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

void test_hash_set_iterator_detects_concurrent_modification() {
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

void test_hash_set_iterator_remove_element() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    iterator_next(iterator);
    // when
    iterator_remove(iterator); // 5
    // then
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 5);
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_set_iterator_remove_element_fails_if_no_next_was_called() {
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

void test_hash_set_iterator_remove_element_fails_if_called_twice_in_a_row() {
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
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4);
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 5);
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_set_iterator_reset() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = hash_set_iterator(hash_set);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_set_is_equal_to_it_self() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool equals = hash_set_equals(hash_set, hash_set);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_hash_set_is_equal_to_another_hash_set() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_hash_set_is_not_equal_to_another_hash_set_with_different_size() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_hash_set_is_not_equal_to_another_hash_set_with_different_elements() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

static void action(void* element) {
    action_count++;
}

void test_perform_action_for_each_element_of_hash_set() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    hash_set_for_each(hash_set, action);
    // then
    TEST_ASSERT_EQUAL(5, action_count);
}

void test_clear_hash_set() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    hash_set_clear(hash_set);
    // then
    TEST_ASSERT_EQUAL(0, hash_set_size(hash_set));
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS_ELEMENTS(hash_set, int, 1, 2, 3, 4, 5);
}

void test_hash_set_contains_element() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = hash_set_contains(hash_set, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_hash_set_does_not_contains_element() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    bool contains = hash_set_contains(hash_set,ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_hash_set_contains_all_elements() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_empty_hash_set_contains_all_elements_of_empty_collection() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    hash_set_destroy(&new_hash_set);
}

void test_hash_set_does_not_contains_all_elements() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
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

void test_clone_hash_set() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    HashSet* copy_hash_set = hash_set_clone(hash_set);
    // then
    TEST_ASSERT_HASH_SET_CONTAINS_ELEMENTS(copy_hash_set, int, 1, 2, 3, 4, 5);
    // clean up
    hash_set_destroy(&copy_hash_set);
}

void test_hash_set_to_collection() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Collection collection = hash_set_to_collection(hash_set);
    // then
    Iterator* iterator = collection_iterator(collection); // iteration order is not guaranteed
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_convert_hash_set_to_array() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) element_array = hash_set_to_array(hash_set);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(element_array, int, 5, 1, 2, 3, 4);
    // clean up
    array_destroy(&element_array);
}

void test_get_hash_set_string_representation() {
    // given
    POPULATE_HASH_SET(hash_set, int, 1, 2, 3, 4, 5);
    // when
    String string = hash_set_to_string(hash_set);
    // then
    TEST_ASSERT_EQUAL_STRING("( 5, 1, 2, 3, 4 )", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_get_empty_hash_set_string_representation() {
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