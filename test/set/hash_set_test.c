#include "hash_set_test.h"

#include "set/hash_set.h"
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
    bool added = hash_set_add(hash_set, int_new(10));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5, 10 };
    TEST_ASSERT_TRUE(added);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_SET(new_elements, hash_set);
}

void test_do_not_add_element_to_hash_set_if_already_present() {
    // given
    int elements[] = { 1, 2, 3, 4, 5 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    bool added = hash_set_add(hash_set, int_new(3));
    // then
    int new_elements[] = { 1, 2, 3, 4, 5 };
    TEST_ASSERT_FALSE(added);
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_SET(new_elements, hash_set);
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
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_SET(new_elements, hash_set);
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
    TEST_ASSERT_ARRAY_EQUALS_TO_HASH_SET(new_elements, hash_set);
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

void test_hash_set_is_empty() {
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_hash_set_is_not_empty() {
    // given
    hash_set_add(hash_set, int_new(10));
    // when
    bool empty = hash_set_is_empty(hash_set);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_hash_set_iterator() {
    // given
    int elements[] = { 1, 2, 3 };
    POPULATE_HASH_SET(hash_set, elements);
    // when
    Iterator* iterator = hash_set_iterator(hash_set);
    // then
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(elements[0], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(elements[1], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_TRUE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(elements[2], *(int*) iterator_next(iterator));
    // and
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_hash_set);
    RUN_TEST(test_do_not_create_hash_set_with_invalid_options);

    RUN_TEST(test_destroy_hash_set_set_it_to_null);
    RUN_TEST(test_destroy_null_hash_set_fails);

    RUN_TEST(test_add_element_to_hash_set);
    RUN_TEST(test_do_not_add_element_to_hash_set_if_already_present);

    RUN_TEST(test_remove_element_from_hash_set);
    RUN_TEST(test_do_not_remove_element_from_hash_set_if_not_present);

    RUN_TEST(test_get_hash_set_size);
    RUN_TEST(test_hash_set_is_empty);
    RUN_TEST(test_hash_set_is_not_empty);

    RUN_TEST(test_hash_set_iterator);

    RUN_TEST(test_hash_set_contains_element);
    RUN_TEST(test_hash_set_does_not_contains_element);

    RUN_TEST(test_hash_set_to_collection);
    return UNITY_END();
}