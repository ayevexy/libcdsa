#include "util/allocators.h"

#include "core/errors.h"
#include "unity.h"

static MemoryArena* arena;
static MemoryPool* pool;

void setUp() {
    arena = memory_arena_new(1024);
    TEST_ASSERT_NOT_NULL(arena);

    pool = memory_pool_new(4, 32); // 4 blocks of 32 bytes == 128 bytes
    TEST_ASSERT_NOT_NULL(pool);
}

void tearDown() {
    memory_arena_destroy(&arena);
    TEST_ASSERT_NULL(arena);

    memory_pool_destroy(&pool);
    TEST_ASSERT_NULL(pool);
}

void test_memory_arena_alloc() {
    // when
    void* first_block = memory_arena_alloc(arena, 256);
    void* second_block = memory_arena_alloc(arena, 256);
    // then
    TEST_ASSERT_NOT_NULL(first_block);
    TEST_ASSERT_NOT_NULL(second_block);
    // and
    TEST_ASSERT_NOT_EQUAL(first_block, second_block);
    // and
    TEST_ASSERT_EQUAL(0, (uintptr) first_block % alignof(max_align_t));
    TEST_ASSERT_EQUAL(0, (uintptr) second_block % alignof(max_align_t));
}

void test_memory_arena_overflow() {
    // when
    void* block; Error error = attempt(block = memory_arena_alloc(arena, 2048));
    // then
    TEST_ASSERT_EQUAL(MEMORY_ALLOCATION_ERROR, error);
    TEST_ASSERT_NULL(block);
}

void test_memory_arena_reset() {
    // given
    memory_arena_alloc(arena, 1024);
    TEST_ASSERT_EQUAL(MEMORY_ALLOCATION_ERROR, attempt(memory_arena_alloc(arena, 1)));
    // when
    memory_arena_reset(arena);
    // then
    void* block = memory_arena_alloc(arena, 1024);
    TEST_ASSERT_NOT_NULL(block);
}

void test_memory_pool_alloc() {
    // when
    void* first_block = memory_pool_alloc(pool);
    void* second_block = memory_pool_alloc(pool);
    // then
    TEST_ASSERT_NOT_NULL(first_block);
    TEST_ASSERT_NOT_NULL(second_block);
    // and
    TEST_ASSERT_NOT_EQUAL(first_block, second_block);
    // and
    TEST_ASSERT_EQUAL(0, (uintptr) first_block % alignof(max_align_t));
    TEST_ASSERT_EQUAL(0, (uintptr) second_block % alignof(max_align_t));
}

void test_memory_pool_overflow() {
    // given
    memory_pool_alloc(pool); // 32
    memory_pool_alloc(pool); // 64
    memory_pool_alloc(pool); // 96
    memory_pool_alloc(pool); // 128
    // when
    void* block; Error error = attempt(block = memory_pool_alloc(pool)); // 160
    // then
    TEST_ASSERT_EQUAL(MEMORY_ALLOCATION_ERROR, error);
    TEST_ASSERT_NULL(block);
}

void test_memory_pool_dealloc() {
    // given
    void* block_1 = memory_pool_alloc(pool); // 32
    void* block_2 = memory_pool_alloc(pool); // 64
    void* block_3 = memory_pool_alloc(pool); // 96
    void* block_4 = memory_pool_alloc(pool); // 128
    TEST_ASSERT_EQUAL(MEMORY_ALLOCATION_ERROR, attempt(memory_pool_alloc(pool))); // 160
    // when
    memory_pool_dealloc(pool, block_1);
    memory_pool_dealloc(pool, block_2);
    memory_pool_dealloc(pool, block_3);
    memory_pool_dealloc(pool, block_4);
    // then
    void* block = memory_pool_alloc(pool);
    TEST_ASSERT_NOT_NULL(block);
    // and
    void* fake_block = (void*) 0xDEADBEEF;
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, attempt(memory_pool_dealloc(pool, fake_block)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_memory_arena_alloc);
    RUN_TEST(test_memory_arena_overflow);
    RUN_TEST(test_memory_arena_reset);
    RUN_TEST(test_memory_pool_alloc);
    RUN_TEST(test_memory_pool_overflow);
    RUN_TEST(test_memory_pool_dealloc);
    return UNITY_END();
}
