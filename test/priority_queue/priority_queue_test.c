#include "priority_queue/priority_queue.h"

#include "../test_utilities.h"
#include "unity.h"

#define INT_PRIORITY_QUEUE_OPTIONS &(PriorityQueueOptions) {    \
    .initial_capacity = 10,                                     \
    .growth_factor = 2.0f,                                      \
    .destruct = noop_destruct,                                  \
    .compare = int_compare_generic,                             \
    .to_string = int_to_string_generic,                         \
    .memory_alloc = memory_try_alloc,                           \
    .memory_dealloc = memory_dealloc                            \
}

#define POPULATE_PRIORITY_QUEUE(priority_queue, T, ...)                     \
    do {                                                                    \
        auto array = (T[]){__VA_ARGS__};                                    \
        int length = COUNT_ARGS(__VA_ARGS__);                               \
        for (int i = 0; i < length; i++) {                                  \
            priority_queue_enqueue(priority_queue, new(int, array[i]));     \
        }                                                                   \
    } while (false)

#define TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, T, ...)      \
    do {                                                                        \
        auto destructor = priority_queue_destructor(priority_queue);            \
        if (destructor == delete) {                                             \
            priority_queue_change_destructor(priority_queue, noop_destruct);    \
        }                                                                       \
        auto array = (T[]){__VA_ARGS__};                                        \
        int length = COUNT_ARGS(__VA_ARGS__);                                   \
        TEST_ASSERT_EQUAL(length, priority_queue_size(priority_queue));         \
        void* elements[length];                                                 \
        for (int i = 0; i < length; i++) {                                      \
            void* removed = priority_queue_dequeue(priority_queue);             \
            TEST_ASSERT_EQUAL(array[i], *(T*) removed);                         \
            elements[i] = removed;                                              \
        }                                                                       \
        for (int i = 0; i < length; i++) {                                      \
            priority_queue_enqueue(priority_queue, elements[i]);                \
        }                                                                       \
        priority_queue_change_destructor(priority_queue, destructor);           \
    } while (false)

static PriorityQueue* priority_queue;

void setUp() {
    priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    priority_queue_change_destructor(priority_queue, delete);
}

void tearDown() {
    priority_queue_destroy(&priority_queue);
}

void test_create_priority_queue() {
    TEST_ASSERT_NOT_NULL(priority_queue);
}

void test_do_not_create_priority_queue_with_invalid_options() {
    // when
    PriorityQueue* new_priority_queue; Error error = attempt(new_priority_queue = priority_queue_new(ref(PriorityQueueOptions)));
    // then
    TEST_ASSERT_NULL(new_priority_queue);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_create_priority_queue_from_collection() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    PriorityQueue* new_priority_queue = priority_queue_from(priority_queue_to_collection(priority_queue), INT_PRIORITY_QUEUE_OPTIONS);
    // then
    TEST_ASSERT_NOT_NULL(new_priority_queue);
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 5, 4, 3, 2, 1);
    // clean up
    priority_queue_destroy(&new_priority_queue);
}

void test_do_not_create_priority_queue_with_invalid_options_from_collection() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    PriorityQueue* new_priority_queue; Error error = attempt(new_priority_queue = priority_queue_from(priority_queue_to_collection(priority_queue), ref(PriorityQueueOptions)));
    // then
    TEST_ASSERT_NULL(new_priority_queue);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_destroy_priority_queue_set_it_to_null() {
    // given
    PriorityQueue* new_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // when
    priority_queue_destroy(&new_priority_queue);
    // then
    TEST_ASSERT_NULL(new_priority_queue);
}

void test_destroy_null_priority_queue_fails() {
    // given
    PriorityQueue* new_priority_queue = nullptr;
    // when
    Error error = attempt(priority_queue_destroy(&new_priority_queue));
    // then
    TEST_ASSERT_EQUAL(NULL_POINTER_ERROR, error);
}

void test_enqueue_element_to_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    priority_queue_enqueue(priority_queue, new(int, 10));
    // then
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 10, 5, 4, 3, 2, 1);
}

void test_enqueue_element_to_priority_queue_exceeding_capacity_resize_it() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    // when
    priority_queue_enqueue(priority_queue, new(int, 100));
    // then
    TEST_ASSERT_EQUAL(20, priority_queue_capacity(priority_queue));
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 100, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
}

void test_enqueue_all_elements_of_a_collection_in_priority_queue() {
    // given
    PriorityQueue* existing_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(existing_priority_queue, int,  10, 20, 30);
    // when
    priority_queue_enqueue_all(priority_queue, priority_queue_to_collection(existing_priority_queue));
    // then
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 30, 20, 10, 5, 4, 3, 2, 1);
    // clean up
    priority_queue_destroy(&existing_priority_queue);
}

void test_peek_element_from_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    int* element = priority_queue_peek(priority_queue);
    // then
    TEST_ASSERT_EQUAL(5, *element);
}

void test_peek_element_from_empty_priority_queue_fails() {
    // when
    int* element; Error error = attempt(element = priority_queue_peek(priority_queue));
    // then
    TEST_ASSERT_NULL(element);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_dequeue_element_from_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    priority_queue_dequeue(priority_queue);
    // then
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 4, 3, 2, 1);
}

void test_dequeue_element_from_priority_queue_no_destruct() {
    // given
    priority_queue_change_destructor(priority_queue, noop_destruct);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    int* value = priority_queue_dequeue(priority_queue);
    // then
    TEST_ASSERT_EQUAL(5, *value);
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 4, 3, 2, 1);
    // clean up
    delete(value);
    priority_queue_change_destructor(priority_queue, delete);
}

void test_dequeue_element_from_empty_priority_queue_fails() {
    // when
    int* value; Error error = attempt(value = priority_queue_dequeue(priority_queue));
    // then
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, error);
}

void test_get_priority_queue_size() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    int size = priority_queue_size(priority_queue);
    // then
    TEST_ASSERT_EQUAL(size, 5);
}

void test_get_priority_queue_capacity() {
    // when
    int capacity = priority_queue_capacity(priority_queue);
    // then
    TEST_ASSERT_EQUAL(10, capacity);
}

void test_priority_queue_is_empty() {
    // when
    bool empty = priority_queue_is_empty(priority_queue);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_priority_queue_is_not_empty() {
    // given
    priority_queue_enqueue(priority_queue, new(int, 10));
    // when
    bool empty = priority_queue_is_empty(priority_queue);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_priority_queue_iterator_forward_iteration() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = priority_queue_iterator(priority_queue); // iteration order is not guaranteed
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_priority_queue_iterator_detects_concurrent_modification() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    Iterator* iterator = priority_queue_iterator(priority_queue);
    priority_queue_enqueue(priority_queue, new(int, 100));
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

void test_priority_queue_iterator_reset() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // and
    Iterator* iterator = priority_queue_iterator(priority_queue);
    iterator_advance(iterator, 3);
    // when
    iterator_reset(iterator);
    // then
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    // clean up
    iterator_destroy(&iterator);
}

void test_priority_queue_is_equal_to_it_self() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    bool equals = priority_queue_equals(priority_queue, priority_queue);
    // then
    TEST_ASSERT_TRUE(equals);
}

void test_priority_queue_is_equal_to_another_priority_queue() {
    // given
    PriorityQueue* other_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(other_priority_queue, int, 1, 2, 3, 4, 5);
    // when
    bool equals = priority_queue_equals(priority_queue, other_priority_queue);
    // then
    TEST_ASSERT_TRUE(equals);
    // clean up
    priority_queue_change_destructor(other_priority_queue, delete);
    priority_queue_destroy(&other_priority_queue);
}

void test_priority_queue_is_not_equal_to_another_priority_queue_with_different_size() {
    // given
    PriorityQueue* other_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(other_priority_queue, int, 1, 2, 3, 4, 5);
    // and
    priority_queue_enqueue(other_priority_queue, new(int, 10));
    // when
    bool equals = priority_queue_equals(priority_queue, other_priority_queue);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    priority_queue_change_destructor(other_priority_queue, delete);
    priority_queue_destroy(&other_priority_queue);
}

void test_priority_queue_is_not_equal_to_another_priority_queue_with_different_elements() {
    // given
    PriorityQueue* other_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(other_priority_queue, int,  2, 3, 4, 5, 6);
    // when
    bool equals = priority_queue_equals(priority_queue, other_priority_queue);
    // then
    TEST_ASSERT_FALSE(equals);
    // clean up
    priority_queue_change_destructor(other_priority_queue, delete);
    priority_queue_destroy(&other_priority_queue);
}

static void action_add_one(void* element) {
    *(int*) element += 1;
}

void test_perform_action_for_each_element_of_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    priority_queue_for_each(priority_queue, action_add_one);
    // then
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(priority_queue, int, 6, 5, 4, 3, 2);
}

void test_clear_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3);
    // when
    priority_queue_clear(priority_queue);
    // then
    TEST_ASSERT_EQUAL(0, priority_queue_size(priority_queue));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(priority_queue_peek(priority_queue)));
}

void test_priority_queue_contains_element() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    bool contains = priority_queue_contains(priority_queue, ref(int, 3));
    // then
    TEST_ASSERT_TRUE(contains);
}

void test_priority_queue_does_not_contains_element() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    bool contains = priority_queue_contains(priority_queue, ref(int, 10));
    // then
    TEST_ASSERT_FALSE(contains);
}

void test_priority_queue_contains_all_elements() {
    // given
    PriorityQueue* new_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(new_priority_queue, int, 2, 3, 4);
    // when
    bool contains_all = priority_queue_contains_all(priority_queue, priority_queue_to_collection(new_priority_queue));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    priority_queue_change_destructor(new_priority_queue, delete);
    priority_queue_destroy(&new_priority_queue);
}

void test_empty_priority_queue_contains_all_elements_of_empty_collection() {
    // given
    PriorityQueue* new_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // when
    bool contains_all = priority_queue_contains_all(priority_queue, priority_queue_to_collection(new_priority_queue));
    // then
    TEST_ASSERT_TRUE(contains_all);
    // clean up
    priority_queue_destroy(&new_priority_queue);
}

void test_priority_queue_does_not_contains_all_elements() {
    // given
    PriorityQueue* new_priority_queue = priority_queue_new(INT_PRIORITY_QUEUE_OPTIONS);
    // and
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    POPULATE_PRIORITY_QUEUE(new_priority_queue, int, 2, 10, 4);
    // when
    bool contains_all = priority_queue_contains_all(priority_queue, priority_queue_to_collection(new_priority_queue));
    // then
    TEST_ASSERT_FALSE(contains_all);
    // clean up
    priority_queue_change_destructor(new_priority_queue, delete);
    priority_queue_destroy(&new_priority_queue);
}

void test_clone_priority_queue() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    PriorityQueue* copy_priority_queue = priority_queue_clone(priority_queue);
    // then
    TEST_ASSERT_PRIORITY_QUEUE_ELEMENTS_EQUALS(copy_priority_queue, int, 5, 4, 3, 2, 1);
    // clean up
    priority_queue_destroy(&copy_priority_queue);
}

void test_convert_priority_queue_to_collection() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    Collection collection = priority_queue_to_collection(priority_queue);
    // then
    TEST_ASSERT_EQUAL(PRIORITY_QUEUE, collection.type);
    TEST_ASSERT_EQUAL(priority_queue, collection.data_structure);
    TEST_ASSERT_EQUAL(priority_queue_size(priority_queue), collection_size(collection));
    // and
    Iterator* iter_a = priority_queue_iterator(priority_queue);
    Iterator* iter_b = collection_iterator(collection);
    TEST_ASSERT_EQUAL(iterator_next(iter_a), iterator_next(iter_b));
    // clean up
    iterator_destroy(&iter_a);
    iterator_destroy(&iter_b);
}

void test_convert_priority_queue_to_array() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    Array(void*) elements = priority_queue_to_array(priority_queue);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 5, 4, 2, 1, 3); // order is not guaranteed
    // clean up
    array_destroy(&elements);
}

void test_get_priority_queue_string_representation() {
    // given
    POPULATE_PRIORITY_QUEUE(priority_queue, int, 1, 2, 3, 4, 5);
    // when
    String string = priority_queue_to_string(priority_queue);
    // then
    TEST_ASSERT_EQUAL_STRING("| 5, 4, 2, 1, 3 |", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_get_empty_priority_queue_string_representation() {
    // when
    String string = priority_queue_to_string(priority_queue);
    // then
    TEST_ASSERT_EQUAL_STRING("||", string_data(string));
    // clean up
    string_destroy(&string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_priority_queue);
    RUN_TEST(test_do_not_create_priority_queue_with_invalid_options);
    RUN_TEST(test_create_priority_queue_from_collection);
    RUN_TEST(test_do_not_create_priority_queue_with_invalid_options_from_collection);

    RUN_TEST(test_destroy_priority_queue_set_it_to_null);
    RUN_TEST(test_destroy_null_priority_queue_fails);

    RUN_TEST(test_enqueue_element_to_priority_queue);
    RUN_TEST(test_enqueue_element_to_priority_queue_exceeding_capacity_resize_it);
    RUN_TEST(test_enqueue_all_elements_of_a_collection_in_priority_queue);

    RUN_TEST(test_peek_element_from_priority_queue);
    RUN_TEST(test_peek_element_from_empty_priority_queue_fails);

    RUN_TEST(test_dequeue_element_from_priority_queue);
    RUN_TEST(test_dequeue_element_from_empty_priority_queue_fails);
    RUN_TEST(test_dequeue_element_from_priority_queue_no_destruct);

    RUN_TEST(test_get_priority_queue_size);
    RUN_TEST(test_get_priority_queue_capacity);
    RUN_TEST(test_priority_queue_is_empty);
    RUN_TEST(test_priority_queue_is_not_empty);

    RUN_TEST(test_priority_queue_iterator_forward_iteration);
    RUN_TEST(test_priority_queue_iterator_detects_concurrent_modification);
    RUN_TEST(test_priority_queue_iterator_reset);

    RUN_TEST(test_priority_queue_is_equal_to_it_self);
    RUN_TEST(test_priority_queue_is_equal_to_another_priority_queue);
    RUN_TEST(test_priority_queue_is_not_equal_to_another_priority_queue_with_different_size);
    RUN_TEST(test_priority_queue_is_not_equal_to_another_priority_queue_with_different_elements);
    RUN_TEST(test_perform_action_for_each_element_of_priority_queue);
    RUN_TEST(test_clear_priority_queue);
    
    RUN_TEST(test_priority_queue_contains_element);
    RUN_TEST(test_priority_queue_does_not_contains_element);
    RUN_TEST(test_priority_queue_contains_all_elements);
    RUN_TEST(test_empty_priority_queue_contains_all_elements_of_empty_collection);
    RUN_TEST(test_priority_queue_does_not_contains_all_elements);

    RUN_TEST(test_clone_priority_queue);
    RUN_TEST(test_convert_priority_queue_to_collection);
    RUN_TEST(test_convert_priority_queue_to_array);
    RUN_TEST(test_get_priority_queue_string_representation);
    RUN_TEST(test_get_empty_priority_queue_string_representation);
    return UNITY_END();
}
