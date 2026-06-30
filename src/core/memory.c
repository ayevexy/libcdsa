#include "memory.h"

#include "errors.h"
#include <stdlib.h>
#include <string.h>

static void* malloc_callback(size_t size, void*) {
    return malloc(size);
}

static void* realloc_callback(void* pointer, size_t size, void*) {
    return realloc(pointer, size);
}

static void free_callback(void* pointer, void*) {
    free(pointer);
}

static void unsupported_operation(void*) {
    set_error(UNSUPPORTED_OPERATION_ERROR, "The global allocator does not support the reset operation");
}

Allocator global_memory_allocator = {
    .storage = &global_memory_allocator, // I contain myself
    .alloc = malloc_callback,
    .realloc = realloc_callback,
    .dealloc = free_callback,
    .reset = unsupported_operation
};

void* (new)(size_t size, const void* source) {
    void* pointer = allocator_alloc(&global_memory_allocator, size);
    if (!pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    if (source) {
        memcpy(pointer, source, size);
    }
    return pointer;
}

void (delete)(void* pointer) {
    allocator_dealloc(&global_memory_allocator, pointer);
}

void* memory_alloc(size_t size) {
    void* pointer = allocator_alloc(&global_memory_allocator, size);
    if (!pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    return pointer;
}

void* memory_try_alloc(size_t size) {
    return allocator_alloc(&global_memory_allocator, size);
}

void* memory_realloc(void* pointer, size_t size) {
    void* new_pointer = allocator_realloc(&global_memory_allocator, pointer, size);
    if (!new_pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to reallocate %zu bytes", size);
        return nullptr;
    }
    return new_pointer;
}

void* memory_try_realloc(void* pointer, size_t size) {
    return allocator_realloc(&global_memory_allocator, pointer, size);
}

void memory_dealloc(void* pointer) {
    allocator_dealloc(&global_memory_allocator, pointer);
}