#include "deque_test.h"

#include "deque/deque.h"
#include "util/memory.h"
#include "util/errors.h"

#include "unity.h"

static Deque* deque;

void setUp() {
    deque = deque_new(INT_DEQUE_OPTIONS);
}

void tearDown() {
    deque_set_destructor(deque, free);
    deque_destroy(&deque);
}

void test_create_deque() {
    TEST_ASSERT_NOT_NULL(deque);
}

void test_do_not_create_deque_with_invalid_options() {
    // when
    Deque* new_deque; Error error = attempt(new_deque = deque_new(&(DequeOptions) {}));
    // then
    TEST_ASSERT_NULL(new_deque);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_deque_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    Deque* new_deque = deque_from(deque_to_collection(deque), INT_DEQUE_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_deque);
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(values, new_deque);
    // clean up
    deque_destroy(&new_deque);
}

void test_do_not_create_deque_with_invalid_options_from_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    Deque* new_deque; Error error = attempt(new_deque = deque_from(deque_to_collection(deque), &(DequeOptions) {}));
    // then
    TEST_ASSERT_NULL(new_deque);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_deque_set_it_to_null() {
    // given
    Deque* new_deque = deque_new(INT_DEQUE_OPTIONS);
    // when
    deque_destroy(&new_deque);
    // then
    TEST_ASSERT_NULL(new_deque);
}

void test_destroy_null_deque_fails() {
    // given
    Deque* new_deque = nullptr;
    // when
    Error error = attempt(deque_destroy(&new_deque));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_add_element_at_beginning_of_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    deque_add_first(deque, new(int, 10));
    // then
    int new_values[] = { 10, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
}

void test_add_element_at_end_of_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    deque_add_last(deque, new(int, 10));
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
}

void test_add_all_elements_from_collection_at_beginning_of_deque() {
    // given
    Deque* existing_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_DEQUE(existing_deque, other_values);
    // when
    deque_add_all_first(deque, deque_to_collection(existing_deque));
    // then
    int new_values[] = { 10, 20, 30, 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
    // clean up
    deque_destroy(&existing_deque);
}

void test_add_all_elements_from_collection_at_end_of_deque() {
    // given
    Deque* existing_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // and
    int other_values[] = { 10, 20, 30 };
    POPULATE_DEQUE(existing_deque, other_values);
    // when
    deque_add_all_last(deque, deque_to_collection(existing_deque));
    // then
    int new_values[] = { 1, 2, 3, 4, 5, 10, 20, 30 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
    // clean up
    deque_destroy(&existing_deque);
}

void test_get_first_element_from_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int* element = deque_get_first(deque);
    // then
    TEST_ASSERT_EQUAL(1, *element);
}

void test_get_first_element_from_empty_deque_fails() {
    // when
    int* element; Error error = attempt(element = deque_get_first(deque));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_last_element_from_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int* element = deque_get_last(deque);
    // then
    TEST_ASSERT_EQUAL(5, *element);
}

void test_get_last_element_from_empty_deque_fails() {
    // when
    int* element; Error error = attempt(element = deque_get_last(deque));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_first_element_from_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int* element = deque_remove_first(deque);
    // then
    int new_values[] = { 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
    TEST_ASSERT_EQUAL(1, *element);
}

void test_remove_first_element_from_empty_deque_fails() {
    // when
    int* element; Error error = attempt(element = deque_remove_first(deque));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_remove_last_element_from_deque() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int* element = deque_remove_last(deque);
    // then
    int new_values[] = { 1, 2, 3, 4 };
    TEST_ASSERT_ARRAY_EQUALS_TO_DEQUE(new_values, deque);
    TEST_ASSERT_EQUAL(5, *element);
}

void test_remove_last_element_from_empty_deque_fails() {
    // when
    int* element; Error error = attempt(element = deque_remove_last(deque));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_deque_size() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    int size = deque_size(deque);
    // then
    TEST_ASSERT_EQUAL(size, SIZE(values));
}

void test_deque_is_empty() {
    // when
    bool empty = deque_is_empty(deque);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_deque_is_not_empty() {
    // given
    deque_add_last(deque, new(int, 10));
    // when
    bool empty = deque_is_empty(deque);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_deque_iterator_forward_iteration() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    Iterator* iterator = deque_iterator(deque);
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

void test_deque_iterator_detects_concurrent_modification() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    Iterator* iterator = deque_iterator(deque);
    deque_add_last(deque, new(int, 10));
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_deque_iterator_reset() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // and
    Iterator* iterator = deque_iterator(deque);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_deque_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    bool contains = deque_contains(deque, &(int){3});
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_deque_does_not_contains_element() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    bool contains = deque_contains(deque, &(int){10});
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_convert_deque_to_collection() {
    // given
    int values[] = { 1, 2, 3, 4, 5 };
    POPULATE_DEQUE(deque, values);
    // when
    Collection collection = deque_to_collection(deque);
    // then
    TEST_ASSERT_EQUAL(deque, collection.data_structure);
    TEST_ASSERT_EQUAL(deque_size(deque), collection_size(collection));
    // and
    Iterator* iter_a = deque_iterator(deque);
    Iterator* iter_b = collection_iterator(collection);
    TEST_ASSERT_EQUAL(iterator_next(iter_a), iterator_next(iter_b));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_deque);
    RUN_TEST(test_do_not_create_deque_with_invalid_options);
    RUN_TEST(test_create_deque_from_collection);
    RUN_TEST(test_do_not_create_deque_with_invalid_options_from_collection);

    RUN_TEST(test_destroy_deque_set_it_to_null);
    RUN_TEST(test_destroy_null_deque_fails);

    RUN_TEST(test_add_element_at_beginning_of_deque);
    RUN_TEST(test_add_element_at_end_of_deque);
    RUN_TEST(test_add_all_elements_from_collection_at_beginning_of_deque);
    RUN_TEST(test_add_all_elements_from_collection_at_end_of_deque);

    RUN_TEST(test_get_first_element_from_deque);
    RUN_TEST(test_get_first_element_from_empty_deque_fails);
    RUN_TEST(test_get_last_element_from_deque);
    RUN_TEST(test_get_last_element_from_empty_deque_fails);

    RUN_TEST(test_remove_first_element_from_deque);
    RUN_TEST(test_remove_first_element_from_empty_deque_fails);
    RUN_TEST(test_remove_last_element_from_deque);
    RUN_TEST(test_remove_last_element_from_empty_deque_fails);

    RUN_TEST(test_get_deque_size);
    RUN_TEST(test_deque_is_empty);
    RUN_TEST(test_deque_is_not_empty);

    RUN_TEST(test_deque_iterator_forward_iteration);
    RUN_TEST(test_deque_iterator_detects_concurrent_modification);
    RUN_TEST(test_deque_iterator_reset);

    RUN_TEST(test_deque_contains_element);
    RUN_TEST(test_deque_does_not_contains_element);

    RUN_TEST(test_convert_deque_to_collection);
    return UNITY_END();
}