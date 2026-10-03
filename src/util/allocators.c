#include "allocators.h"

#include "core/errors.h"
#include "util/constraints.h"

struct MemoryArena {
    Allocator self;
    bytes capacity;
    usize offset;
    alignas(max_align_t) byte buffer[];
};

static void* arena_alloc_callback(bytes size, void* arena) {
    return memory_arena_alloc(arena, size);
}

static void* arena_realloc_callback(void*, bytes, void*) {
    set_error(UNSUPPORTED_OPERATION_ERROR, "arenas do not support reallocation");
    return nullptr;
}

static void arena_dealloc_callback(void*, void*) {

}

static void arena_reset_callback(void* arena) {
    memory_arena_reset(arena);
}

MemoryArena* memory_arena_new(bytes capacity) {
    MemoryArena* arena = memory_try_alloc(sizeof(MemoryArena) + capacity);
    if (!arena) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'arena'");
        return nullptr;
    }
    arena->self = (Allocator) {
        .storage = arena,
        .alloc = arena_alloc_callback,
        .realloc = arena_realloc_callback,
        .dealloc = arena_dealloc_callback,
        .reset = arena_reset_callback
    };
    arena->capacity = capacity;
    arena->offset = 0;
    return arena;
}

void memory_arena_destroy(MemoryArena** arena_pointer) {
    if (require_non_null(arena_pointer, *arena_pointer)) return;
    MemoryArena* arena = *arena_pointer;

    memory_dealloc(arena);
    *arena_pointer = nullptr;
}

void* memory_arena_alloc(MemoryArena* arena, bytes size) {
    if (require_non_null(arena)) return nullptr;

    constexpr bytes alignment = alignof(max_align_t);
    const bytes padding = (-arena->offset) & (alignment - 1);
    const bytes remaining = arena->capacity - arena->offset;

    if (padding > remaining || size > remaining - padding) {
        set_error(MEMORY_ALLOCATION_ERROR, "the requested memory block exceeds the arena capacity");
        return nullptr;
    }

    const bytes offset = arena->offset + padding;
    void* block = arena->buffer + offset;
    arena->offset = offset + size;

    return block;
}

void memory_arena_reset(MemoryArena* arena) {
    if (require_non_null(arena)) return;

    arena->offset = 0;
}

Allocator* memory_arena_as_allocator(MemoryArena* arena) {
    if (require_non_null(arena)) return nullptr;

    return &arena->self;
}

struct MemoryPool {
    Allocator self;
    usize capacity;
    bytes block_size;
    void* free_list;
    alignas(max_align_t) byte buffer[];
};

static void* memory_pool_alloc_callback(bytes, void* pool) {
    return memory_pool_alloc(pool);
}

static void* memory_pool_realloc_callback(void*, bytes, void*) {
    set_error(UNSUPPORTED_OPERATION_ERROR, "memory pools do not support reallocation");
    return nullptr;
}

static void memory_pool_dealloc_callback(void* pointer, void* pool) {
    memory_pool_dealloc(pool, pointer);
}

static void memory_pool_reset_callback(void*) {
    set_error(UNSUPPORTED_OPERATION_ERROR, "memory pools do not support reset");
}

static bytes align_block_size(bytes block_size) {
    constexpr bytes alignment = alignof(max_align_t);

    if (block_size < sizeof(void*)) {
        block_size = sizeof(void*);
    }
    if (block_size > SIZE_MAX - (alignment - 1)) {
        return 0;
    }
    return (block_size + alignment - 1) & ~(alignment - 1);
}

static bool contains(const MemoryPool* pool, const void* pointer) {
    const uintptr start = (uintptr) pool->buffer;
    const uintptr end = start + pool->block_size * pool->capacity;
    const uintptr address = (uintptr) pointer;

    return address >= start && address < end
           && (address - start) % pool->block_size == 0;
}

MemoryPool* memory_pool_new(usize capacity, bytes block_size) {
    if (block_size == 0 || capacity == 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'capacity' and 'block_size' must be greater than zero");
        return nullptr;
    }

    block_size = align_block_size(block_size);

    if (block_size == 0 || block_size > SIZE_MAX / capacity) {
        set_error(MEMORY_ALLOCATION_ERROR, "the memory pool size exceeds the maximum allocation size");
        return nullptr;
    }

    const bytes buffer_size = capacity * block_size;

    if (buffer_size > SIZE_MAX - sizeof(MemoryPool)) {
        set_error(MEMORY_ALLOCATION_ERROR, "the memory pool size exceeds the maximum allocation size");
        return nullptr;
    }

    MemoryPool* pool = memory_try_alloc(sizeof(MemoryPool) + buffer_size);

    if (!pool) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'pool'");
        return nullptr;
    }

    pool->capacity = capacity;
    pool->block_size = block_size;
    pool->free_list = pool->buffer;

    for (usize i = 0; i < capacity - 1; i++) {
        uchar* current = pool->buffer + i * block_size;
        uchar* next = current + block_size;
        *(void**) current = next;
    }
    uchar* last = pool->buffer + (capacity - 1) * block_size;
    *(void**) last = nullptr;

    pool->self = (Allocator) {
        .storage = pool,
        .alloc = memory_pool_alloc_callback,
        .realloc = memory_pool_realloc_callback,
        .dealloc = memory_pool_dealloc_callback,
        .reset = memory_pool_reset_callback
    };
    return pool;
}

void memory_pool_destroy(MemoryPool** pool_pointer) {
    if (require_non_null(pool_pointer, *pool_pointer)) return;
    MemoryPool* pool = *pool_pointer;

    memory_dealloc(pool);
    *pool_pointer = nullptr;
}

void* memory_pool_alloc(MemoryPool* pool) {
    if (require_non_null(pool)) return nullptr;

    if (!pool->free_list) {
        set_error(MEMORY_ALLOCATION_ERROR, "the memory pool is exhausted");
        return nullptr;
    }

    void* block = pool->free_list;
    pool->free_list = *(void**) block;

    return block;
}

void memory_pool_dealloc(MemoryPool* pool, void* pointer) {
    if (require_non_null(pool, pointer)) return;

    if (!contains(pool, pointer)) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "the pointer does not belong to the memory pool");
        return;
    }

    *(void**) pointer = pool->free_list;
    pool->free_list = pointer;
}

Allocator* memory_pool_as_allocator(MemoryPool* pool) {
    if (require_non_null(pool)) return nullptr;

    return &pool->self;
}