#include "set/set.h"

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

#define INT_TREE_SET_OPTIONS &(TreeSetOptions) {    \
    .destruct = noop_destruct,                      \
    .compare = int_pointer_value_compare,           \
    .to_string = int_pointer_value_to_string,       \
    .memory_alloc = memory_try_alloc,               \
    .memory_dealloc = memory_dealloc                \
}

#define POPULATE_SET(set, T, ...)               \
    do {                                        \
        auto array = (T[]){__VA_ARGS__};        \
        int length = count_args(__VA_ARGS__);   \
        for (int i = 0; i < length; i++) {      \
            set_add(set, new(int, array[i]));   \
        }                                       \
    } while (false)

#define TEST_ASSERT_SET_CONTAINS_ELEMENTS(set, T, ...)          \
    do {                                                        \
        auto array = (T[]){__VA_ARGS__};                        \
        int length = count_args(__VA_ARGS__);                   \
        for (int i = 0; i < length; i++) {                      \
            TEST_ASSERT_TRUE(set_contains(set, &array[i]));     \
        }                                                       \
    } while (false)

#define TEST_ASSERT_SET_DO_NOT_CONTAINS_ELEMENTS(set, T, ...)   \
    do {                                                        \
        auto array = (T[]){__VA_ARGS__};                        \
        int length = count_args(__VA_ARGS__);                   \
        for (int i = 0; i < length; i++) {                      \
            TEST_ASSERT_FALSE(set_contains(set, &array[i]));    \
        }                                                       \
    } while (false)

static HashSet* set_a;
static TreeSet* set_b;

void setUp() {
    set_a = set_new(INT_HASH_SET_OPTIONS);
    set_b = set_new(INT_TREE_SET_OPTIONS);

    set_change_destructor(set_a, delete);
    set_change_destructor(set_b, delete);
}

void tearDown() {
    set_destroy(&set_a);
    set_destroy(&set_b);
}

void test_sets_union() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 6, 7, 8, 9, 10);
    // when
    HashSet* union_set = sets_union(set_a, set_b, set_new(INT_HASH_SET_OPTIONS));
    // then
    TEST_ASSERT_EQUAL(10, set_size(union_set));
    TEST_ASSERT_SET_CONTAINS_ELEMENTS(union_set, int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    // clean up
    set_destroy(&union_set);
}

void test_sets_intersection() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 0, 2, 3, 4, 6);
    // when
    HashSet* intersection_set = sets_intersection(set_a, set_b, set_new(INT_HASH_SET_OPTIONS));
    // then
    TEST_ASSERT_EQUAL(3, set_size(intersection_set));
    TEST_ASSERT_SET_CONTAINS_ELEMENTS(intersection_set, int, 2, 3, 4);
    TEST_ASSERT_SET_DO_NOT_CONTAINS_ELEMENTS(intersection_set, int, 0, 1, 5, 6);
    // clean up
    set_destroy(&intersection_set);
}

void test_sets_difference() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 0, 2, 3, 4, 6);
    // when
    HashSet* difference_set = sets_difference(set_a, set_b, set_new(INT_HASH_SET_OPTIONS));
    // then
    TEST_ASSERT_EQUAL(2, set_size(difference_set));
    TEST_ASSERT_SET_CONTAINS_ELEMENTS(difference_set, int, 1, 5);
    TEST_ASSERT_SET_DO_NOT_CONTAINS_ELEMENTS(difference_set, int, 0, 2, 3, 4, 6);
    // clean up
    set_destroy(&difference_set);
}

void test_sets_symmetric_difference() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 0, 2, 3, 4, 6);
    // when
    HashSet* symmetric_different_set = sets_symmetric_difference(set_a, set_b, set_new(INT_HASH_SET_OPTIONS));
    // then
    TEST_ASSERT_EQUAL(4, set_size(symmetric_different_set));
    TEST_ASSERT_SET_CONTAINS_ELEMENTS(symmetric_different_set, int, 0, 1, 5, 6);
    TEST_ASSERT_SET_DO_NOT_CONTAINS_ELEMENTS(symmetric_different_set, int, 2, 3, 4);
    // clean up
    set_destroy(&symmetric_different_set);
}

void test_set_is_subset() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 2, 3, 4);
    // when
    bool subset = sets_is_subset(set_b, set_a);
    // then
    TEST_ASSERT_TRUE(subset);
    // when
    set_add(set_b, new(int, 6));
    subset = sets_is_subset(set_b, set_a);
    // then
    TEST_ASSERT_FALSE(subset);
}

void test_set_is_superset() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 2, 3, 4);
    // when
    bool superset = sets_is_superset(set_a, set_b);
    // then
    TEST_ASSERT_TRUE(superset);
    // when
    set_add(set_b, new(int, 6));
    superset = sets_is_superset(set_a, set_b);
    // then
    TEST_ASSERT_FALSE(superset);
}

void test_multiple_sets_operations() {
    // given
    POPULATE_SET(set_a, int, 1, 2, 3, 4, 5);
    POPULATE_SET(set_b, int, 6, 7, 8, 9, 10);
    // when
    HashSet* union_set = sets_union(set_a, set_b, set_new(INT_HASH_SET_OPTIONS));
    TreeSet* intersection_set = sets_intersection(set_a, union_set, set_new(INT_TREE_SET_OPTIONS));
    // then
    TEST_ASSERT_EQUAL(10, set_size(union_set));
    TEST_ASSERT_EQUAL(5, set_size(intersection_set));
    // and
    TEST_ASSERT_SET_CONTAINS_ELEMENTS(intersection_set, int, 1, 2, 3, 4, 5);
    TEST_ASSERT_SET_DO_NOT_CONTAINS_ELEMENTS(intersection_set, int, 6, 7, 8, 9, 10);
    // clean up
    set_destroy(&union_set);
    set_destroy(&intersection_set);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sets_union);
    RUN_TEST(test_sets_intersection);
    RUN_TEST(test_sets_difference);
    RUN_TEST(test_sets_symmetric_difference);
    RUN_TEST(test_set_is_subset);
    RUN_TEST(test_set_is_superset);
    RUN_TEST(test_multiple_sets_operations);
    return UNITY_END();
}

