#include "util/sequence.h"

#include "list/list.h"
#include "core/memory.h"

#include "unity.h"
#include "../test_utilities.h"

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

void setUp() {
    list = list_new(DEFAULT_ARRAY_LIST_OPTIONS(
        .destruct = delete,
        .equals = int_pointer_value_equals,
        .to_string = int_pointer_value_to_string
    ));
    empty_list = list_new(DEFAULT_ARRAY_LIST_OPTIONS(
        .equals = int_pointer_value_equals,
        .to_string = int_pointer_value_to_string
    ));
    list_add_last(list, new(int, 1));
    list_add_last(list, new(int, 2));
    list_add_last(list, new(int, 3));
    list_add_last(list, new(int, 4));
    list_add_last(list, new(int, 5));
}

void tearDown() {
    list_destroy(&list);
    list_destroy(&empty_list);
}

void test_create_sequence_from_collection() {
    // given
    Collection collection = list_to_collection(list);
    // when
    Sequence* sequence = sequence_from(collection);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_create_sequence_of_given_elements() {
    // given
    Sequence* sequence = sequence_of(&(int){1}, &(int){2}, &(int){3}, &(int){4}, &(int){5});
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_create_empty_sequence() {
    // given
    Sequence* sequence = sequence_empty();
    // then
    TEST_ASSERT_EQUAL(0, sequence_count(sequence));
}

void* int_generator() {
    static int counter = 1;
    return new(int, counter++);
}

void test_generate_sequence() {
    // given
    Sequence* sequence = sequence_generate(int_generator, 5);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

bool int_has_next(const void* number) {
    return *(int*) number <= 5;
}

void* int_next(void* number) {
    return new(int, *(int*) number + 1);
}

void test_iterate_sequence() {
    // given
    Sequence* sequence = sequence_iterate(&(int){1}, int_has_next, int_next);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_filter_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_filter(sequence, is_even);
    // then
    TEST_ASSERT_EQUAL(2, sequence_count(sequence));
}

void test_map_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_map(sequence, replace_by_two_times);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 2, 4, 6, 8, 10 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_peek_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_peek(sequence, add_one);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 2, 3, 4, 5, 6 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_limit_n_elements_of_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_limit(sequence, 3);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(3, array_length(elements));
}

void test_skip_n_elements_of_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_skip(sequence, 3);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(2, array_length(elements));
}

void test_take_elements_of_sequence_while_predicate_is_truth() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_take_while(sequence, is_less_or_equal_3);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(3, array_length(elements));
}

void test_drop_elements_of_sequence_while_predicate_is_truth() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_drop_while(sequence, is_less_or_equal_3);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(2, array_length(elements));
}

void test_filter_distinct_elements_of_sequence() {
    // given
    list_add_last(list, new(int, 1)); // duplicate
    list_add_last(list, new(int, 5)); // duplicate
    // and
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_distinct(sequence, int_pointer_value_equals);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(5, array_length(elements));
}

void test_sort_elements_of_sequence() {
    // given
    list_add(list, 1, new(int, 6));
    list_add(list, 0, new(int, 7));
    list_add(list, 3, new(int, 8));
    list_add(list, 2, new(int, 9));
    list_add(list, 4, new(int, 10));
    // and
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_sort(sequence, int_pointer_value_compare, QUICK_SORT);
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(10, array_length(elements));
}

void test_chain_sequence_operations() {
    // given
    list_add(list, 1, new(int, 6));
    list_add(list, 1, new(int, 6));
    list_add(list, 0, new(int, 7));
    list_add(list, 3, new(int, 8));
    list_add(list, 3, new(int, 8));
    list_add(list, 2, new(int, 9));
    list_add(list, 4, new(int, 10));
    list_add(list, 4, new(int, 10));
    list_swap(list, 8, 9);
    // and
    Sequence* sequence = sequence_from(list_to_collection(list)); // 7, 1, 9, 6, 6, 10, 10, 8, 8, 2, 3, 5, 4
    // when
    sequence_filter(sequence, is_even); // 6, 6, 10, 10, 8, 8, 2, 4
    sequence_sort(sequence, int_pointer_value_compare, QUICK_SORT); // 2, 4, 6, 6, 8, 8, 10, 10
    sequence_peek(sequence, add_one); // 3, 5, 7, 7, 9, 9, 11, 11
    sequence_distinct(sequence, int_pointer_value_equals); // 3, 5, 7, 9, 11
    sequence_sort(sequence, int_pointer_value_compare_reversed, MERGE_SORT); // 11, 9, 7, 5, 3
    sequence_map(sequence, replace_by_two_times); // 22, 18, 14, 10, 6
    // then
    Array(void*) elements = sequence_to_array(sequence);
    int expected[] = { 22, 18, 14, 10, 6 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    TEST_ASSERT_EQUAL(5, array_length(elements));
}

void test_apply_action_for_each_element_of_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    sequence_for_each(sequence, add_one);
    // then
    Array(void*) elements = list_to_array(list);
    int expected[] = { 2, 3, 4, 5, 6 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
    // clean up
    array_destroy(&elements);
}

void test_reduce_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    int* result = sequence_reduce(sequence, &(int){0}, sum);
    // then
    TEST_ASSERT_EQUAL(15, *result);
}

void test_find_element_in_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_find(sequence);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(1, *(int*) result.value);
}

void test_do_not_find_element_in_empty_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(empty_list));
    // when
    Optional result = sequence_find(sequence);
    // then
    TEST_ASSERT_FALSE(result.present);
}

void test_find_max_element_of_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_max(sequence, int_pointer_value_compare);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(5, *(int*) result.value);
}

void test_find_min_element_of_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Optional result = sequence_min(sequence, int_pointer_value_compare);
    // then
    TEST_ASSERT_TRUE(result.present);
    TEST_ASSERT_EQUAL(1, *(int*) result.value);
}

void test_sequence_all_match_predicate() {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_all_match(sequence_a, is_not_null));
    TEST_ASSERT_FALSE(sequence_all_match(sequence_b, is_even));
}

void test_sequence_any_match_predicate() {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_any_match(sequence_a, is_not_null));
    TEST_ASSERT_FALSE(sequence_any_match(sequence_b, is_null));
}

void test_sequence_none_match_predicate() {
    // given
    Sequence* sequence_a = sequence_from(list_to_collection(list));
    Sequence* sequence_b = sequence_from(list_to_collection(list));
    // then
    TEST_ASSERT_TRUE(sequence_none_match(sequence_a, is_null));
    TEST_ASSERT_FALSE(sequence_none_match(sequence_b, is_even));
}

void test_count_sequence() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    int count = sequence_count(sequence);
    // then
    TEST_ASSERT_EQUAL(list_size(list), count);
}

void test_convert_sequence_to_array() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(list));
    // when
    Array(void*) elements = sequence_to_array(sequence);
    // then
    int expected[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
}

void test_convert_empty_sequence_to_array() {
    // given
    Sequence* sequence = sequence_from(list_to_collection(empty_list));
    // when
    Array(void*) elements = sequence_to_array(sequence);
    // then
    int expected[] = {};
    TEST_ASSERT_ARRAY_EQUALS(expected, elements);
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
    return UNITY_END();
}