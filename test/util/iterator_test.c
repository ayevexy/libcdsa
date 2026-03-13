#include "iterator_test.h"

#include "util/errors.h"
#include "unity.h"

static DataStructure* data_structure;

static Iterator* iterator;

void setUp() {
    data_structure = DATA_STRUCTURE_NEW(DATA_STRUCTURE_DEFAULT_OPTS());

    POPULATE_DATA_STRUCTURE(data_structure);

    iterator = DATA_STRUCTURE_ITERATOR(data_structure);
}

void tearDown() {
    iterator_destroy(&iterator);
    DATA_STRUCTURE_DESTROY(&data_structure);
}

void test_iterator_forward_iteration() {
    FOR_EACH_ELEMENT_OF_DATA_STRUCTURE(element, data_structure) {
        TEST_ASSERT_TRUE(iterator_has_next(iterator));
        TEST_ASSERT_EQUAL_ELEMENT(element, iterator_next(iterator));
    }
    TEST_ASSERT_FALSE(iterator_has_next(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_next(iterator)));
}

void test_iterator_backward_iteration() {
    TEST_IGNORE();
    // given
    iterator_next(iterator);
    iterator_next(iterator);
    iterator_next(iterator);
    iterator_next(iterator);
    iterator_next(iterator);
    // then
    FOR_EACH_ELEMENT_OF_DATA_STRUCTURE_REVERSED(element, data_structure) {
        TEST_ASSERT_TRUE(iterator_has_previous(iterator));
        TEST_ASSERT_EQUAL_ELEMENT(element, iterator_previous(iterator));
    }
    TEST_ASSERT_FALSE(iterator_has_previous(iterator));
    TEST_ASSERT_EQUAL(NO_SUCH_ELEMENT_ERROR, attempt(iterator_previous(iterator)));
}

void test_iterator_forward_iteration_fail_on_concurrent_modification() {
    // when
    DATA_STRUCTURE_CLEAR(data_structure);
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_next(iterator)));
}

void test_iterator_backward_iteration_fail_on_concurrent_modification() {
    TEST_IGNORE();
    // when
    DATA_STRUCTURE_CLEAR(data_structure);
    // then
    TEST_ASSERT_EQUAL(CONCURRENT_MODIFICATION_ERROR, attempt(iterator_previous(iterator)));
}

void test_iterator_forward_iteration_after_reset() {
    // given
    test_iterator_forward_iteration();
    // when
    iterator_reset(iterator);
    // then
    test_iterator_forward_iteration();
}

void test_iterator_backward_iteration_after_reset() {
    TEST_IGNORE();
    // given
    test_iterator_backward_iteration();
    // when
    iterator_reset(iterator);
    // then
    test_iterator_backward_iteration();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_iterator_forward_iteration);
    RUN_TEST(test_iterator_backward_iteration);
    RUN_TEST(test_iterator_forward_iteration_fail_on_concurrent_modification);
    RUN_TEST(test_iterator_backward_iteration_fail_on_concurrent_modification);
    RUN_TEST(test_iterator_forward_iteration_after_reset);
    RUN_TEST(test_iterator_backward_iteration_after_reset);
    return UNITY_END();
}
