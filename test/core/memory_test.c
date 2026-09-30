#include "core/memory.h"

#include "unity.h"

void setUp() {

}

void tearDown() {

}

typedef struct {
    float x, y;
} Point;

void test_memory_new_and_delete() {
    // when
    int* n = new(int);
    int* m = new(int, 10);
    Point* p = new(Point);
    Point* q = new(Point, 10, 20);
    char* c = new(char, 'a');
    // then
    TEST_ASSERT_EQUAL(0, *n);
    TEST_ASSERT_EQUAL(10, *m);
    TEST_ASSERT(0 == p->x && 0 == p->y);
    TEST_ASSERT(10 == q->x && 20 == q->y);
    TEST_ASSERT_EQUAL('a', *c);
    // clean up
    delete(n);
    delete(m);
    delete(p);
    delete(q);
    delete(c);
    // then
    TEST_ASSERT_NULL(n);
    TEST_ASSERT_NULL(m);
    TEST_ASSERT_NULL(p);
    TEST_ASSERT_NULL(q);
    TEST_ASSERT_NULL(c);
}

void test_memory_ref() {
    // when
    int* n = ref(int);
    int* m = ref(int, 10);
    Point* p = ref(Point);
    Point* q = ref(Point, 10, 20);
    char* c = ref(char, 'a');
    // then
    TEST_ASSERT_EQUAL(0, *n);
    TEST_ASSERT_EQUAL(10, *m);
    TEST_ASSERT(0 == p->x && 0 == p->y);
    TEST_ASSERT(10 == q->x && 20 == q->y);
    TEST_ASSERT_EQUAL('a', *c);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_memory_new_and_delete);
    RUN_TEST(test_memory_ref);
    return UNITY_END();
}
