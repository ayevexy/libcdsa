#include "set/set.h"

#include "unity.h"
#include "../test_utilities.h"

#define INT_HASH_SET_OPTIONS() &(HashSetOptions) {      \
    .initial_capacity = 16,                             \
    .load_factor = 0.75f,                               \
    .hash = char_hash,                                  \
    .destruct = noop_destruct,                          \
    .equals = int_pointer_value_equals,                 \
    .to_string = int_pointer_value_to_string,           \
    .memory_alloc = memory_try_alloc,                   \
    .memory_dealloc = memory_dealloc                    \
}

#define INT_TREE_SET_OPTIONS() &(TreeSetOptions) {      \
    .destruct = noop_destruct,                          \
    .compare = int_pointer_value_compare,               \
    .to_string = int_pointer_value_to_string,           \
    .memory_alloc = memory_try_alloc,                   \
    .memory_dealloc = memory_dealloc                    \
}

#define POPULATE_SET(hash_set, array)           \
    for (int i = 0; i < SIZE(array); i++) {     \
        set_add(hash_set, new(int, array[i]));  \
    }

static HashSet* set_a;
static TreeSet* set_b;

void setUp() {
    set_a = set_new(INT_HASH_SET_OPTIONS());
    set_b = set_new(INT_TREE_SET_OPTIONS());
}

void tearDown() {
    set_change_destructor(set_a, delete);
    set_change_destructor(set_b, delete);

    set_destroy(&set_a);
    set_destroy(&set_b);
}

void test_sets_union() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 6, 7, 8, 9, 10};
    POPULATE_SET(set_b, other_elements);
    // when
    HashSet* union_set = sets_union(set_a, set_b, set_new(INT_HASH_SET_OPTIONS()));
    // then
    TEST_ASSERT_EQUAL(10, set_size(union_set));
    // and
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 1)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 2)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 3)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 4)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 5)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 6)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 7)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 8)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 9)));
    TEST_ASSERT_TRUE(set_contains(union_set, ref(int, 10)));
}

void test_sets_intersection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_SET(set_b, other_elements);
    // when
    HashSet* intersection_set = sets_intersection(set_a, set_b, set_new(INT_HASH_SET_OPTIONS()));
    // then
    TEST_ASSERT_EQUAL(3, set_size(intersection_set));
    // and
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 2)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 3)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 4)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 0)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 1)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 5)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 6)));
}

void test_sets_difference() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_SET(set_b, other_elements);
    // when
    HashSet* difference_set = sets_difference(set_a, set_b, set_new(INT_HASH_SET_OPTIONS()));
    // then
    TEST_ASSERT_EQUAL(2, set_size(difference_set));
    // and
    TEST_ASSERT_TRUE(set_contains(difference_set, ref(int, 1)));
    TEST_ASSERT_TRUE(set_contains(difference_set, ref(int, 5)));
    TEST_ASSERT_FALSE(set_contains(difference_set, ref(int, 0)));
    TEST_ASSERT_FALSE(set_contains(difference_set, ref(int, 2)));
    TEST_ASSERT_FALSE(set_contains(difference_set, ref(int, 3)));
    TEST_ASSERT_FALSE(set_contains(difference_set, ref(int, 4)));
    TEST_ASSERT_FALSE(set_contains(difference_set, ref(int, 6)));
}

void test_sets_symmetric_difference() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_SET(set_b, other_elements);
    // when
    HashSet* symmetric_different_set = sets_symmetric_difference(set_a, set_b, set_new(INT_HASH_SET_OPTIONS()));
    // then
    TEST_ASSERT_EQUAL(4, set_size(symmetric_different_set));
    // and
    TEST_ASSERT_TRUE(set_contains(symmetric_different_set, ref(int, 0)));
    TEST_ASSERT_TRUE(set_contains(symmetric_different_set, ref(int, 1)));
    TEST_ASSERT_TRUE(set_contains(symmetric_different_set, ref(int, 5)));
    TEST_ASSERT_TRUE(set_contains(symmetric_different_set, ref(int, 6)));
    TEST_ASSERT_FALSE(set_contains(symmetric_different_set, ref(int, 2)));
    TEST_ASSERT_FALSE(set_contains(symmetric_different_set, ref(int, 3)));
    TEST_ASSERT_FALSE(set_contains(symmetric_different_set, ref(int, 4)));
}

void test_set_is_subset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4 };
    POPULATE_SET(set_b, other_elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4 };
    POPULATE_SET(set_b, other_elements);
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
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_SET(set_a, elements);
    // and
    int other_elements[] = { 6, 7, 8, 9, 10};
    POPULATE_SET(set_b, other_elements);
    // when
    HashSet* union_set = sets_union(set_a, set_b, set_new(INT_HASH_SET_OPTIONS()));
    TreeSet* intersection_set = sets_intersection(set_a, union_set, set_new(INT_TREE_SET_OPTIONS()));
    // then
    TEST_ASSERT_EQUAL(10, set_size(union_set));
    TEST_ASSERT_EQUAL(5, set_size(intersection_set));
    // and
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 1)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 2)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 3)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 4)));
    TEST_ASSERT_TRUE(set_contains(intersection_set, ref(int, 5)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 6)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 7)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 8)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 9)));
    TEST_ASSERT_FALSE(set_contains(intersection_set, ref(int, 10)));
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

