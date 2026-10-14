#include "deque/deque.h"

#include "../test_utilities.h"
#include "unity.h"

#define INT_DEQUE_OPTIONS &(DequeOptions) {     \
    .initial_capacity = 8,                      \
    .destruct = noop_destruct,                  \
    .equals = int_pointer_value_equals,         \
    .to_string = int_pointer_value_to_string,   \
    .memory_alloc = memory_try_alloc,           \
    .memory_dealloc = memory_dealloc            \
}

#define POPULATE_DEQUE(deque, T, ...)                   \
    do {                                                \
        auto array = (T[]){__VA_ARGS__};                \
        int length = COUNT_ARGS(__VA_ARGS__);           \
        for (int i = 0; i < length; i++) {              \
            deque_add_last(deque, new(int, array[i]));  \
    }                                                   \
    } while (false)

#define TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, T, ...)    \
    do {                                                    \
        auto destructor = deque_destructor(deque);          \
        if (destructor == delete) {                         \
            deque_change_destructor(deque, noop_destruct);  \
        }                                                   \
        auto array = (T[]){__VA_ARGS__};                    \
        int length = COUNT_ARGS(__VA_ARGS__);               \
        TEST_ASSERT_EQUAL(length, deque_size(deque));       \
        void* elements[length];                             \
        for (int i = 0; i < length; i++) {                  \
            void* removed = deque_remove_first(deque);      \
            TEST_ASSERT_EQUAL(array[i], *(T*) removed);     \
            elements[i] = removed;                          \
        }                                                   \
        for (int i = 0; i < length; i++) {                  \
            deque_add_last(deque, elements[i]);             \
        }                                                   \
        deque_change_destructor(deque, destructor);         \
    } while (false)

static Deque* deque;

void setUp() {
    deque = deque_new(INT_DEQUE_OPTIONS);
    deque_change_destructor(deque, delete);
}

void tearDown() {
    deque_destroy(&deque);
}

void test_create_deque() {
    TEST_ASSERT_NOT_NULL(deque);
}

void test_do_not_create_deque_with_invalid_options() {
    // when
    Deque* new_deque; Error error = attempt(new_deque = deque_new(ref(DequeOptions)));
    // then
    TEST_ASSERT_NULL(new_deque);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_deque_from_collection() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5 );
    // when
    Deque* new_deque = deque_from(deque_to_collection(deque), INT_DEQUE_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_deque);
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(new_deque, int, 1, 2, 3, 4, 5);
    // clean up
    deque_destroy(&new_deque);
}

void test_do_not_create_deque_with_invalid_options_from_collection() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Deque* new_deque; Error error = attempt(new_deque = deque_from(deque_to_collection(deque), ref(DequeOptions)));
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_add_first(deque, new(int, 10));
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 10, 1, 2, 3, 4, 5);
}

void test_add_element_at_end_of_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_add_last(deque, new(int, 10));
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 1, 2, 3, 4, 5, 10);
}

void test_add_element_to_deque_exceeding_capacity_resize_it() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5, 6, 7, 8);
    // when
    deque_add_last(deque, new(int, 9));
    // then
    TEST_ASSERT_EQUAL(16, deque_capacity(deque));
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 1, 2, 3, 4, 5, 6, 7, 8, 9);
}

void test_add_all_elements_from_collection_at_beginning_of_deque() {
    // given
    Deque* existing_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5 );
    POPULATE_DEQUE(existing_deque, int, 10, 20, 30);
    // when
    deque_add_all_first(deque, deque_to_collection(existing_deque));
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 30, 20, 10, 1, 2, 3, 4, 5);
    // clean up
    deque_destroy(&existing_deque);
}

void test_add_all_elements_from_collection_at_end_of_deque() {
    // given
    Deque* existing_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(existing_deque, int, 10, 20, 30);
    // when
    deque_add_all_last(deque, deque_to_collection(existing_deque));
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 1, 2, 3, 4, 5, 10, 20, 30);
    // clean up
    deque_destroy(&existing_deque);
}

void test_get_first_element_from_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_remove_first(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 2, 3, 4, 5);
}

void test_remove_first_element_from_deque_no_destruct() {
    // given
    deque_change_destructor(deque, noop_destruct);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    int* element = deque_remove_first(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 2, 3, 4, 5 );
    TEST_ASSERT_EQUAL(1, *element);
    // clean up
    delete(element);
    deque_change_destructor(deque, delete);
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_remove_last(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 1, 2, 3, 4);
}

void test_remove_last_element_from_deque_no_destruct() {
    // given
    deque_change_destructor(deque, noop_destruct);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    int* element = deque_remove_last(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 1, 2, 3, 4);
    TEST_ASSERT_EQUAL(5, *element);
    // clean up
    delete(element);
    deque_change_destructor(deque, delete);
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    int size = deque_size(deque);
    // then
    TEST_ASSERT_EQUAL(size, 5);
}

void test_get_deque_capacity() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    // when
    int capacity = deque_capacity(deque);
    // then
    TEST_ASSERT_EQUAL(16, capacity);
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
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
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

void test_deque_iterator_backward_iteration() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = deque_iterator(deque);
    // and
    iterator_advance(iterator, 5);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_previous(iterator));
    // then
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

void test_deque_iterator_detects_concurrent_modification() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = deque_iterator(deque);
    deque_add_last(deque, new(int, 10));
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_previous(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_deque_iterator_reset() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
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

void test_deque_iterator_at_specified_position() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = deque_iterator_at(deque, 3);
    // then
    TEST_ASSERT_TRUE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_previous(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_deque_iterator_at_invalid_position_fails() {
    // given
    Iterator* iterator;
    // then
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = deque_iterator_at(deque, -1)));
    TEST_ASSERT_NULL(iterator);
    // and
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, attempt(iterator = deque_iterator_at(deque, 6)));
    TEST_ASSERT_NULL(iterator);
}

void test_deque_is_equal_to_it_self() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    bool equals = deque_equals(deque, deque);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_deque_is_equal_to_another_deque() {
    // given
    Deque* other_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(other_deque, int, 1, 2, 3, 4, 5);
    // when
    bool equals = deque_equals(deque, other_deque);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    deque_change_destructor(other_deque, delete);
    deque_destroy(&other_deque);
}

void test_deque_is_not_equal_to_another_deque_with_different_size() {
    // given
    Deque* other_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(other_deque, int, 1, 2, 3, 4, 5);
    // and
    deque_add_last(other_deque, new(int, 10));
    // when
    bool equals = deque_equals(deque, other_deque);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    deque_change_destructor(other_deque, delete);
    deque_destroy(&other_deque);
}

void test_deque_is_not_equal_to_another_deque_with_different_elements() {
    // given
    Deque* other_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(other_deque, int, 2, 3, 4, 5, 6);
    // when
    bool equals = deque_equals(deque, other_deque);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    deque_change_destructor(other_deque, delete);
    deque_destroy(&other_deque);
}

static void action_add_one(void* element) {
    *(int*) element += 1;
}

void test_perform_action_for_each_element_of_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_for_each(deque, action_add_one);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 2, 3, 4, 5, 6);
}

void test_reverse_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    deque_reverse(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(deque, int, 5, 4, 3, 2, 1);
}

void test_clear_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3 );
    // when
    deque_clear(deque);
    // then
    TEST_ASSERT_EQUAL(0, deque_size(deque));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(deque_get_first(deque)));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(deque_get_last(deque)));
}

void test_deque_contains_element() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    bool contains = deque_contains(deque, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_deque_does_not_contains_element() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    bool contains = deque_contains(deque, ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_deque_contains_all_elements() {
    // given
    Deque* new_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(new_deque, int, 2, 3, 4);
    // when
    bool contains_all = deque_contains_all(deque, deque_to_collection(new_deque));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    deque_change_destructor(new_deque, delete);
    deque_destroy(&new_deque);
}

void test_empty_deque_contains_all_elements_of_empty_collection() {
    // given
    Deque* new_deque = deque_new(INT_DEQUE_OPTIONS);
    // when
    bool contains_all = deque_contains_all(deque, deque_to_collection(new_deque));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    deque_destroy(&new_deque);
}

void test_deque_does_not_contains_all_elements() {
    // given
    Deque* new_deque = deque_new(INT_DEQUE_OPTIONS);
    // and
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    POPULATE_DEQUE(new_deque, int, 2, 10, 4);
    // when
    bool contains_all = deque_contains_all(deque, deque_to_collection(new_deque));
    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    deque_change_destructor(new_deque, delete);
    deque_destroy(&new_deque);
}

void test_clone_deque() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Deque* copy_deque = deque_clone(deque);
    // then
    TEST_ASSERT_DEQUE_ELEMENTS_EQUALS(copy_deque, int, 1, 2, 3, 4, 5);
    // clean up
    deque_destroy(&copy_deque);
}

void test_convert_deque_to_collection() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Collection collection = deque_to_collection(deque);
    // then
    TEST_ASSERT_EQUAL(DEQUE, collection.type);
    TEST_ASSERT_EQUAL(deque, collection.data_structure);
    TEST_ASSERT_EQUAL(deque_size(deque), collection_size(collection));
    // and
    Iterator* iter_a = deque_iterator(deque);
    Iterator* iter_b = collection_iterator(collection);
    TEST_ASSERT_EQUAL(iterator_next(iter_a), iterator_next(iter_b));
    // clean up
    iterator_destroy(&iter_a);
    iterator_destroy(&iter_b);
}

void test_convert_deque_to_array() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) elements = deque_to_array(deque);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&elements);
}

void test_get_deque_string_representation() {
    // given
    POPULATE_DEQUE(deque, int, 1, 2, 3, 4, 5);
    // when
    String string = deque_to_string(deque);
    // then
    TEST_ASSERT_EQUAL_STRING("| 1, 2, 3, 4, 5 |", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_get_empty_deque_string_representation() {
    // when
    String string = deque_to_string(deque);
    // then
    TEST_ASSERT_EQUAL_STRING("||", string_data(string));
    // clean up
    string_destroy(&string);
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
    RUN_TEST(test_add_element_to_deque_exceeding_capacity_resize_it);
    RUN_TEST(test_add_all_elements_from_collection_at_beginning_of_deque);
    RUN_TEST(test_add_all_elements_from_collection_at_end_of_deque);

    RUN_TEST(test_get_first_element_from_deque);
    RUN_TEST(test_get_first_element_from_empty_deque_fails);
    RUN_TEST(test_get_last_element_from_deque);
    RUN_TEST(test_get_last_element_from_empty_deque_fails);

    RUN_TEST(test_remove_first_element_from_deque);
    RUN_TEST(test_remove_first_element_from_deque_no_destruct);
    RUN_TEST(test_remove_first_element_from_empty_deque_fails);
    RUN_TEST(test_remove_last_element_from_deque);
    RUN_TEST(test_remove_last_element_from_deque_no_destruct);
    RUN_TEST(test_remove_last_element_from_empty_deque_fails);

    RUN_TEST(test_get_deque_size);
    RUN_TEST(test_get_deque_capacity);
    RUN_TEST(test_deque_is_empty);
    RUN_TEST(test_deque_is_not_empty);

    RUN_TEST(test_deque_iterator_forward_iteration);
    RUN_TEST(test_deque_iterator_backward_iteration);
    RUN_TEST(test_deque_iterator_detects_concurrent_modification);
    RUN_TEST(test_deque_iterator_reset);
    RUN_TEST(test_deque_iterator_at_specified_position);
    RUN_TEST(test_deque_iterator_at_invalid_position_fails);

    RUN_TEST(test_deque_is_equal_to_it_self);
    RUN_TEST(test_deque_is_equal_to_another_deque);
    RUN_TEST(test_deque_is_not_equal_to_another_deque_with_different_size);
    RUN_TEST(test_deque_is_not_equal_to_another_deque_with_different_elements);
    RUN_TEST(test_perform_action_for_each_element_of_deque);
    RUN_TEST(test_reverse_deque);
    RUN_TEST(test_clear_deque);

    RUN_TEST(test_deque_contains_element);
    RUN_TEST(test_deque_does_not_contains_element);
    RUN_TEST(test_deque_contains_all_elements);
    RUN_TEST(test_empty_deque_contains_all_elements_of_empty_collection);
    RUN_TEST(test_deque_does_not_contains_all_elements);

    RUN_TEST(test_clone_deque);
    RUN_TEST(test_convert_deque_to_collection);
    RUN_TEST(test_convert_deque_to_array);
    RUN_TEST(test_get_deque_string_representation);
    RUN_TEST(test_get_empty_deque_string_representation);
    return UNITY_END();
}