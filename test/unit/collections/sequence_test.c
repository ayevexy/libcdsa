#include "collections/sequence.h"

#include "collections/list/list.h"
#include "collections/collection.h"
#include "collections/callbacks.h"
#include "core/memory.h"
#include "core/array.h"
#include "core/optional.h"

#include "test_utilities.h"
#include "unity.h"

#define POPULATE_LIST(list, T, ...)                     \
    do {                                                \
        auto array = (T[]){__VA_ARGS__};                \
        int length = COUNT_ARGS(__VA_ARGS__);           \
        for (int i = 0; i < length; i++) {              \
            list_add_last(list, new(int, array[i]));    \
        }                                               \
    } while (false)

static bool is_less_or_equal_3(const void* number) {
    return *(int*) number <= 3;
}

static bool is_even(const void* number) {
    return *(int*) number % 2 == 0;
}

static bool is_not_null(const void* number) {
    return number != nullptr;
}

static bool is_null(const void* number) {
    return number == nullptr;
}

static void add_one(void* number) {
    *(int*) number += 1;
}

static void* replace_by_two_times(void* number) {
    return new(int, *(int*) number * 2);
}

static void* sum(void* result, void* next) {
    *(int*) result += *(int*) next;
    return result;
}

static ArrayList* list;
static ArrayList* empty_list;

void setUp(void) {
    list = list_new(ARRAY_LIST_OPTIONS(int));
    list_change_destructor(list, delete);

    empty_list = list_new(ARRAY_LIST_OPTIONS(int));

    POPULATE_LIST(list, int, 1, 2, 3, 4, 5);
}

void tearDown(void) {
    list_destroy(&list);
    list_destroy(&empty_list);
}

static void test_create_sequence_from_collection(void) {
    // given
    Collection collection = list_to_collection(list);
    // when
    Sequence* sequence = sequence_from(collection);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&elements);
}

static void test_create_sequence_of_given_elements(void) {
    // when
    Sequence* sequence = sequence_of(ref(int, 1), ref(int, 2), ref(int, 3), ref(int, 4), ref(int, 5));
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&elements);
}

static void test_create_empty_sequence(void) {
    // when
    Sequence* sequence = sequence_empty();
    // then
    TEST_ASSERT_EQUAL(0, sequence_count(sequence));
}

static void* int_generator(void) {
    static int counter = 1;
    return new(int, counter++);
}

static void test_generate_sequence(void) {
    // when
    Sequence* sequence = sequence_generate(int_generator, 5);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    for (int i = 0; i < array_length(elements); i++) {
        delete(elements[i]);
    }
    array_destroy(&elements);
}

static bool int_has_next(const void* number) {
    return number && *(const int*) number <= 5;
}

static void* int_next(void* number) {
    int value = *(int*) number;
    return value < 5 ? new(int, value + 1) : nullptr; // avoids allocating an extra element that would be rejected by the predicate
}

static void test_iterate_sequence(void) {
    // when
    Sequence* sequence = sequence_iterate(new(int, 1), int_has_next, int_next);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    for (int i = 0; i < array_length(elements); i++) {
        delete(elements[i]);
    }
    array_destroy(&elements);
}

static void test_filter_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_filter(sequence, is_even);
    // then
    TEST_ASSERT_EQUAL(2, sequence_count(sequence));
}

static void test_map_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_map(sequence, replace_by_two_times);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 2, 4, 6, 8, 10);
    // clean up
    for (int i = 0; i < array_length(elements); i++) {
        delete(elements[i]);
    }
    array_destroy(&elements);
}

static void test_peek_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_peek(sequence, add_one);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 2, 3, 4, 5, 6);
    // clean up
    array_destroy(&elements);
}

static void test_limit_n_elements_of_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_limit(sequence, 3);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3);
    TEST_ASSERT_EQUAL(3, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_skip_n_elements_of_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_skip(sequence, 3);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 4, 5);
    TEST_ASSERT_EQUAL(2, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_take_elements_of_sequence_while_predicate_is_truth(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_take_while(sequence, is_less_or_equal_3);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3);
    TEST_ASSERT_EQUAL(3, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_drop_elements_of_sequence_while_predicate_is_truth(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_drop_while(sequence, is_less_or_equal_3);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 4, 5);
    TEST_ASSERT_EQUAL(2, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_filter_distinct_elements_of_sequence(void) {
    // given
    list_add_last(list, new(int, 1)); // duplicate
    list_add_last(list, new(int, 5)); // duplicate
    // and
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_distinct(sequence, int_equals_generic);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    TEST_ASSERT_EQUAL(5, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_sort_elements_of_sequence(void) {
    // given
    list_clear(list);
    // and
    POPULATE_LIST(list, int, 1, 4, 3, 5, 2, 10, 6, 7, 9, 8);
    // and
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_sort(sequence, int_compare_generic, QUICK_SORT);
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    TEST_ASSERT_EQUAL(10, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static inline int int_compare_reversed_generic(const void* a, const void* b) {
    return int_compare_generic(b, a);
}

static void test_chain_sequence_operations(void) {
    // given
    list_clear(list);
    // and
    POPULATE_LIST(list, int, 7, 1, 9, 6, 10, 10, 8, 8, 2, 6, 3, 4, 5);
    // and
    Sequence* sequence = sequence_from(list_to_collection(list)); // 7, 1, 9, 6, 6, 10, 10, 8, 8, 2, 3, 5, 4
    // when
    sequence_filter(sequence, is_even); // 6, 6, 10, 10, 8, 8, 2, 4
    sequence_sort(sequence, int_compare_generic, QUICK_SORT); // 2, 4, 6, 6, 8, 8, 10, 10
    sequence_peek(sequence, add_one); // 3, 5, 7, 7, 9, 9, 11, 11
    sequence_distinct(sequence, int_equals_generic); // 3, 5, 7, 9, 11
    sequence_sort(sequence, int_compare_reversed_generic, MERGE_SORT); // 11, 9, 7, 5, 3
    sequence_map(sequence, replace_by_two_times); // 22, 18, 14, 10, 6
    // and
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 22, 18, 14, 10, 6);
    TEST_ASSERT_EQUAL(5, array_length(elements));
    // clean up
    for (int i = 0; i < array_length(elements); i++) {
        delete(elements[i]);
    }
    array_destroy(&elements);
}

static void test_apply_action_for_each_element_of_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_for_each(sequence, add_one);
    // and
    Array(void*) elements = list_to_array(list);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 2, 3, 4, 5, 6);
    // clean up
    array_destroy(&elements);
}

static void test_reduce_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    int* result = sequence_reduce(sequence, ref(int), sum);
    // then
    TEST_ASSERT_EQUAL(15, *result);
}

static void test_find_element_in_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_find(sequence);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(1, *(int*) result.value);
}

static void test_do_not_find_element_in_empty_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(empty_list));
    // when
    Optional result = sequence_find(sequence);
    // then
    TEST_ASSERT_FALSE(result.present);
}

static void test_find_max_element_of_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_max(sequence, int_compare_generic);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(5, *(int*) result.value);
}

static void test_find_min_element_of_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_min(sequence, int_compare_generic);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(1, *(int*) result.value);
}

static void test_sequence_all_match_predicate(void) {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_all_match(sequence_a, is_not_null));
    TEST_ASSERT_FALSE(sequence_all_match(sequence_b, is_even));
}

static void test_sequence_any_match_predicate(void) {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_any_match(sequence_a, is_not_null));
    TEST_ASSERT_FALSE(sequence_any_match(sequence_b, is_null));
}

static void test_sequence_none_match_predicate(void) {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_none_match(sequence_a, is_null));
    TEST_ASSERT_FALSE(sequence_none_match(sequence_b, is_even));
}

static void test_count_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    int count = sequence_count(sequence);
    // then
    TEST_ASSERT_EQUAL(list_size(list), count);
}

static void test_convert_sequence_to_array(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(elements, int, 1, 2, 3, 4, 5);
    // clean up
    array_destroy(&elements);
}

static void test_convert_empty_sequence_to_array(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(empty_list));
    // when
    Array(void*) elements = sequence_to_array(sequence);
    // then
    TEST_ASSERT_EQUAL(0, array_length(elements));
    // clean up
    array_destroy(&elements);
}

static void test_collect_sequence(void) {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    LinkedList* other_list = sequence_collect(sequence, list_to_collection(list_new(DEFAULT_LINKED_LIST_OPTIONS())));
    // then
    TEST_ASSERT_EQUAL(list_size(list), list_size(other_list));
    TEST_ASSERT_EQUAL(list_get(list, 0), list_get(other_list, 0));
    TEST_ASSERT_EQUAL(list_get(list, 1), list_get(other_list, 1));
    TEST_ASSERT_EQUAL(list_get(list, 2), list_get(other_list, 2));
    TEST_ASSERT_EQUAL(list_get(list, 3), list_get(other_list, 3));
    TEST_ASSERT_EQUAL(list_get(list, 4), list_get(other_list, 4));
    // clean up
    list_destroy(&other_list);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_sequence_from_collection);
    RUN_TEST(test_create_sequence_of_given_elements);
    RUN_TEST(test_create_empty_sequence);
    RUN_TEST(test_generate_sequence);
    RUN_TEST(test_iterate_sequence);
    RUN_TEST(test_filter_sequence);
    RUN_TEST(test_map_sequence);
    RUN_TEST(test_peek_sequence);
    RUN_TEST(test_limit_n_elements_of_sequence);
    RUN_TEST(test_skip_n_elements_of_sequence);
    RUN_TEST(test_take_elements_of_sequence_while_predicate_is_truth);
    RUN_TEST(test_drop_elements_of_sequence_while_predicate_is_truth);
    RUN_TEST(test_filter_distinct_elements_of_sequence);
    RUN_TEST(test_sort_elements_of_sequence);
    RUN_TEST(test_chain_sequence_operations);
    RUN_TEST(test_apply_action_for_each_element_of_sequence);
    RUN_TEST(test_reduce_sequence);
    RUN_TEST(test_find_element_in_sequence);
    RUN_TEST(test_do_not_find_element_in_empty_sequence);
    RUN_TEST(test_find_max_element_of_sequence);
    RUN_TEST(test_find_min_element_of_sequence);
    RUN_TEST(test_sequence_all_match_predicate);
    RUN_TEST(test_sequence_any_match_predicate);
    RUN_TEST(test_sequence_none_match_predicate);
    RUN_TEST(test_count_sequence);
    RUN_TEST(test_convert_sequence_to_array);
    RUN_TEST(test_convert_empty_sequence_to_array);
    RUN_TEST(test_collect_sequence);
    return UNITY_END();
}