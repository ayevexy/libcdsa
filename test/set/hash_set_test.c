#include "hash_set_test.h"

#include "set/hash_set.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static HashSet* hash_set;

void setUp() {
    hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
}

void tearDown() {
    hash_set_set_destructor(hash_set, free);
    hash_set_destroy(&hash_set);
}

void test_create_hash_set() {
    TEST_ASSERT_NOT_NULL(hash_set);
}

void test_do_not_create_hash_set_with_invalid_options() {
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_new(&(HashSetOptions) {}));
    // then
    TEST_ASSERT_NULL(new_hash_set);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_hash_set_from_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    HashSet* new_hash_set = hash_set_from(hash_set_to_collection(hash_set), INT_HASH_SET_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_hash_set);
    TEST_ASSERT_HASH_SET_CONTAINS(new_hash_set, elements);
    // clean up
    hash_set_destroy(&new_hash_set);
}

void test_do_not_create_hash_set_with_invalid_options_from_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    HashSet* new_hash_set; Error error = attempt(new_hash_set = hash_set_from(hash_set_to_collection(hash_set), &(HashSetOptions) {}));
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool added = hash_set_add(hash_set, new(int, 10));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_do_not_add_element_to_hash_set_if_already_present() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool added = hash_set_add(hash_set, new(int, 3));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_add_all_elements_to_hash_set() {
    // given
    HashSet* existing_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int other_elements[] = { 10, 20, 30 };
    POPULATE_HASH_SET(existing_hash_set, other_elements);
    // when
    bool changed = hash_set_add_all(hash_set, hash_set_to_collection(existing_hash_set));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 10, 20, 30 };
    TEST_ASSERT_TRUE(changed);
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_do_not_add_all_elements_to_hash_set_if_already_present() {
    // given
    HashSet* existing_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    POPULATE_HASH_SET(existing_hash_set, elements);
    // when
    bool changed = hash_set_add_all(hash_set, hash_set_to_collection(existing_hash_set));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(changed);
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_add_elements_to_hash_set_exceeding_threshold_resizes_it() {
    // given
    int elements[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    hash_set_add(hash_set, new(int, 13));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    TEST_ASSERT_EQUAL(32, hash_set_capacity(hash_set));
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_remove_element_from_hash_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool removed = hash_set_remove(hash_set, &(int){3});
    // then
    int new_elements[] = { 1, 2, 4, 5 };
    TEST_ASSERT_TRUE(removed);
    TEST_ASSERT_FALSE(hash_set_contains(hash_set, &(int){3}));
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_do_not_remove_element_from_hash_set_if_not_present() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool removed = hash_set_remove(hash_set, &(int){10});
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
}

void test_remove_all_elements_from_hash_set_matching_collection() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int sub_elements[] = { 2, 3, 4 };
    POPULATE_HASH_SET(new_hash_set, sub_elements);
    // when
    int count = hash_set_remove_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    int new_elements[] = { 1, 5 };
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
    TEST_ASSERT_EQUAL(3, count);
    // clean up
    hash_set_set_destructor(new_hash_set, free);
    hash_set_destroy(&new_hash_set);
}

static bool is_odd(const void* element) {
    return *(int*) element % 2 != 0;
}

void test_remove_elements_from_hash_set_matching_predicate() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    int count = hash_set_remove_if(hash_set, is_odd);
    // then
    TEST_ASSERT_EQUAL(3, count);
    int new_elements[] = { 2, 4 };
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, new_elements);
    int removed_elements[] = { 1, 3, 5 };
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS(hash_set, removed_elements);
}

void test_retain_all_elements_from_collection_in_hash_set() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int new_elements[] = { 2, 4 };
    POPULATE_HASH_SET(new_hash_set, new_elements);
    // when
    int count = hash_set_retain_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_EQUAL(3, count);
    int retained_elements[] = { 2, 4 };
    TEST_ASSERT_HASH_SET_CONTAINS(hash_set, retained_elements);
    int removed_elements[] = { 1, 3, 5 };
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS(hash_set, removed_elements);
    // clean up
    hash_set_set_destructor(new_hash_set, free);
    hash_set_destroy(&new_hash_set);
}

void test_get_hash_set_size() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    int size = hash_set_size(hash_set);
    // then
    TEST_ASSERT_EQUAL(5, size);
}

void test_get_hash_set_capacity() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
    hash_set_clear(hash_set);
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_hash_set_iterator_reset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool equals = hash_set_equals(hash_set, hash_set);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_hash_set_is_equal_to_another_hash_set() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    POPULATE_HASH_SET(other_hash_set, elements);
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    hash_set_set_destructor(other_hash_set, free);
    hash_set_destroy(&other_hash_set);
}

void test_hash_set_is_not_equal_to_another_hash_set_with_different_size() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    POPULATE_HASH_SET(other_hash_set, elements);
    hash_set_add(other_hash_set, new(int, 10));
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    hash_set_set_destructor(other_hash_set, free);
    hash_set_destroy(&other_hash_set);
}

void test_hash_set_is_not_equal_to_another_hash_set_with_different_elements() {
    // given
    HashSet* other_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int other_elements[] = { 2, 3, 4, 5, 6 };
    POPULATE_HASH_SET(other_hash_set, other_elements);
    // when
    bool equals = hash_set_equals(hash_set, other_hash_set);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    hash_set_set_destructor(other_hash_set, free);
    hash_set_destroy(&other_hash_set);
}

static int action_count = 0;

static void action(void* element) {
    action_count++;
}

void test_perform_action_for_each_element_of_hash_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    hash_set_for_each(hash_set, action);
    // then
    TEST_ASSERT_EQUAL(5, action_count);
}

void test_clear_hash_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    hash_set_clear(hash_set);
    // then
    TEST_ASSERT_EQUAL(0, hash_set_size(hash_set));
    TEST_ASSERT_HASH_SET_DO_NOT_CONTAINS(hash_set, elements);
}

void test_hash_set_contains_element() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool contains = hash_set_contains(hash_set, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_hash_set_does_not_contains_element() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool contains = hash_set_contains(hash_set, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_hash_set_contains_all_elements() {
    // given
    HashSet* new_hash_set = hash_set_new(INT_HASH_SET_OPTIONS);
    // and
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int other_elements[] = { 2, 3, 4};
    POPULATE_HASH_SET(new_hash_set, other_elements);
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    hash_set_set_destructor(new_hash_set, free);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // and
    int other_elements[] = { 2, 10, 4};
    POPULATE_HASH_SET(new_hash_set, other_elements);
    // when
    bool contains_all = hash_set_contains_all(hash_set, hash_set_to_collection(new_hash_set));
    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    hash_set_set_destructor(new_hash_set, free);
    hash_set_destroy(&new_hash_set);
}

void test_clone_hash_set() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    HashSet* copy_hash_set = hash_set_clone(hash_set);
    // then
    TEST_ASSERT_HASH_SET_CONTAINS(copy_hash_set, elements);
    // clean up
    hash_set_destroy(&copy_hash_set);
}

void test_hash_set_to_collection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    void** element_array = hash_set_to_array(hash_set);
    // then
    int new_elements[] = { 5, 1, 2, 3, 4 };
    TEST_ASSERT_ARRAY_EQUALS(new_elements, element_array);
    // clean up
    free(element_array);
}

void test_get_hash_set_string_representation() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, values);
    // when
    char* string = hash_set_to_string(hash_set);
    // then
    TEST_ASSERT_EQUAL_STRING("( 5, 1, 2, 3, 4 )", string);
    // clean up
    free(string);
}

void test_get_empty_hash_set_string_representation() {
    // when
    char* string = hash_set_to_string(hash_set);
    // then
    TEST_ASSERT_EQUAL_STRING("()", string);
    // clean up
    free(string);
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