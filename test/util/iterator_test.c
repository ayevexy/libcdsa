#include "util/iterator.h"

#include "list/array_list.h"
#include "../test_utilities.h"

#include "unity.h"

#define POPULATE_ARRAY_LIST(array_list, T, ...)                     \
    do {                                                            \
        auto array = (T[]){__VA_ARGS__};                            \
        int length = COUNT_ARGS(__VA_ARGS__);                       \
        for (int i = 0; i < length; i++) {                          \
            array_list_add_last(array_list, new(int, array[i]));    \
        }                                                           \
    } while (false)


static ArrayList* array_list;

void setUp() {
    array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS());
    array_list_change_destructor(array_list, delete);

    POPULATE_ARRAY_LIST(array_list, int, 1, 2, 3, 4, 5);
}

void tearDown() {
    array_list_destroy(&array_list);
}

void test_iterator_advance() {
    // given
    Iterator* iterator = array_list_iterator(array_list);
    // then
    TEST_ASSERT_EQUAL(3, iterator_advance(iterator, 3));
    TEST_ASSERT_EQUAL(4, *(int*) iterator_next(iterator));
    TEST_ASSERT_EQUAL(1, iterator_advance(iterator, 4));
    // clean up
    iterator_destroy(&iterator);
}

void test_iterator_retreat() {
    // given
    Iterator* iterator = array_list_iterator(array_list);
    // and
    iterator_advance(iterator, 5);
    // then
    TEST_ASSERT_EQUAL(3, iterator_retreat(iterator, 3));
    TEST_ASSERT_EQUAL(2, *(int*) iterator_previous(iterator));
    TEST_ASSERT_EQUAL(1, iterator_retreat(iterator, 4));
    // clean up
    iterator_destroy(&iterator);
}

void test_iterator_element() {
    // given
    Iterator* iterator = array_list_iterator(array_list);
    // then
    TEST_ASSERT_EQUAL(4, *(int*) iterator_element(iterator, 3));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_element(iterator, 4)));
    // clean up
    iterator_destroy(&iterator);
}

void test_iterator_element_or_default() {
    // given
    Iterator* iterator = array_list_iterator(array_list);
    // then
    TEST_ASSERT_EQUAL(4, *(int*) iterator_element_or_default(iterator, 3, ref(int, 10)));
    TEST_ASSERT_EQUAL(10, *(int*) iterator_element_or_default(iterator, 4, ref(int, 10)));
    // clean up
    iterator_destroy(&iterator);
}

void test_iterator_size() {
    // given
    Iterator* iterator = array_list_iterator(array_list);
    // then
    TEST_ASSERT_EQUAL(5, iterator_size(iterator));
    TEST_ASSERT_EQUAL(0, iterator_size(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
    // clean up
    iterator_destroy(&iterator);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_iterator_advance);
    RUN_TEST(test_iterator_retreat);
    RUN_TEST(test_iterator_element);
    RUN_TEST(test_iterator_element_or_default);
    RUN_TEST(test_iterator_size);
    return UNITY_END();
}

