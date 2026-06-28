#include "set/hash_set.h"
#include "util/sets.h" // should be included after any set

#include "core/memory.h"
#include "core/errors.h"

#include "unity.h"
#include "../test_utilities.h"

#define INT_HASH_SET_OPTIONS() &(HashSetOptions) {      \
    .initial_capacity = 16,                             \
    .load_factor = 0.75f,                               \
    .hash = char_hash,                                  \
    .destruct = noop_destruct,                          \
    .equals = int_pointer_value_equals,                 \
    .to_string = int_pointer_value_to_string,           \
    .memory_alloc = malloc,                             \
    .memory_dealloc = free                              \
}

#define POPULATE_HASH_SET(hash_set, array)          \
    for (int i = 0; i < SIZE(array); i++) {         \
        hash_set_add(hash_set, new(int, array[i])); \
    }

static HashSet* hash_set_a;
static HashSet* hash_set_b;
static HashSet* hash_set_c;

void setUp() {
    hash_set_a = hash_set_new(INT_HASH_SET_OPTIONS());
    hash_set_b = hash_set_new(INT_HASH_SET_OPTIONS());
    hash_set_c = hash_set_new(INT_HASH_SET_OPTIONS());
}

void tearDown() {
    hash_set_change_destructor(hash_set_a, delete);
    hash_set_change_destructor(hash_set_b, delete);
    hash_set_change_destructor(hash_set_c, delete);

    hash_set_destroy(&hash_set_a);
    hash_set_destroy(&hash_set_b);
    hash_set_destroy(&hash_set_c);
}

void test_set_union() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 6, 7, 8, 9, 10};
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    SetView new_set = sets_union(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_EQUAL(10, set_view_size(&new_set));
    // and
    Iterator* iterator = set_view_iterator(&new_set);
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(10, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(6, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(7, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(8, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(9, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // and
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){1}));
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){6}));
}

void test_set_intersection() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    SetView new_set = sets_intersection(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_EQUAL(3, set_view_size(&new_set));
    // and
    Iterator* iterator = set_view_iterator(&new_set);
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // and
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){2}));
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){4}));
}

void test_set_difference() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    SetView new_set = sets_difference(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_EQUAL(2, set_view_size(&new_set));
    // and
    Iterator* iterator = set_view_iterator(&new_set);
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // and
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){1}));
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){5}));
}

void test_set_symmetric_difference() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 0, 2, 3, 4, 6};
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    SetView new_set = sets_symmetric_difference(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_EQUAL(4, set_view_size(&new_set));
    // and
    Iterator* iterator = set_view_iterator(&new_set);
    TEST_ASSERT_EQUAL(5, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(0, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(6, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // and
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){1}));
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){0}));
}

void test_set_is_subset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4 };
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    bool subset = sets_is_subset(hash_set_b, hash_set_a);
    // then
    TEST_ASSERT_TRUE(subset);
}

void test_set_is_not_subset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4, 6 };
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    bool subset = sets_is_subset(hash_set_b, hash_set_a);
    // then
    TEST_ASSERT_FALSE(subset);
}

void test_set_is_superset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4 };
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    bool superset = sets_is_superset(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_TRUE(superset);
}

void test_set_is_not_superset() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 2, 3, 4, 6 };
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // when
    bool superset = sets_is_superset(hash_set_a, hash_set_b);
    // then
    TEST_ASSERT_FALSE(superset);
}

void test_set_chained_operations_passing_set_view_as_argument() {
    // given
    int elements[] = { 1, 2 };
    POPULATE_HASH_SET(hash_set_a, elements);
    // and
    int other_elements[] = { 3, 4 };
    POPULATE_HASH_SET(hash_set_b, other_elements);
    // and
    int another_elements[] = { 0, 2, 3, 5 };
    POPULATE_HASH_SET(hash_set_c, another_elements);
    // when
    SetView union_set = sets_union(hash_set_a, hash_set_b);
    SetView new_set = sets_intersection(hash_set_c, &union_set);
    // then
    TEST_ASSERT_EQUAL(2, set_view_size(&new_set));
    // and
    Iterator* iterator = set_view_iterator(&new_set);
    TEST_ASSERT_EQUAL(2, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(3, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // and
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){2}));
    TEST_ASSERT_TRUE(set_view_contains(&new_set, &(int){3}));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_set_union);
    RUN_TEST(test_set_intersection);
    RUN_TEST(test_set_difference);
    RUN_TEST(test_set_symmetric_difference);
    RUN_TEST(test_set_is_subset);
    RUN_TEST(test_set_is_not_subset);
    RUN_TEST(test_set_is_superset);
    RUN_TEST(test_set_is_not_superset);
    RUN_TEST(test_set_chained_operations_passing_set_view_as_argument);
    return UNITY_END();
}

