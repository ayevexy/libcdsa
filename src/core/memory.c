#include "memory.h"

#include "errors.h"
#include <stdlib.h>
#include <string.h>

static void* malloc_callback(bytes size, void*) {
    return malloc(size);
}

static void* realloc_callback(void* pointer, bytes size, void*) {
    return realloc(pointer, size);
}

static void free_callback(void* pointer, void*) {
    free(pointer);
}

static void reset_unsupported_callback(void*) {
    set_error(UNSUPPORTED_OPERATION_ERROR, "The global memory allocator does not support the reset operation");
}

Allocator global_memory_allocator = {
    .storage = &global_memory_allocator, // I contain myself
    .alloc = malloc_callback,
    .realloc = realloc_callback,
    .dealloc = free_callback,
    .reset = reset_unsupported_callback
};

void* (new)(bytes size, const void* source) {
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

void* memory_alloc(bytes size) {
    void* pointer = allocator_alloc(&global_memory_allocator, size);
    if (!pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    return pointer;
}

void* memory_try_alloc(bytes size) {
    return allocator_alloc(&global_memory_allocator, size);
}

void* memory_realloc(void* pointer, bytes size) {
    void* new_pointer = allocator_realloc(&global_memory_allocator, pointer, size);
    if (!new_pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to reallocate %zu bytes", size);
        return nullptr;
    }
    return new_pointer;
}

void* memory_try_realloc(void* pointer, bytes size) {
    return allocator_realloc(&global_memory_allocator, pointer, size);
}

void memory_dealloc(void* pointer) {
    allocator_dealloc(&global_memory_allocator, pointer);
}

void* memory_copy(const void* source, void* destination, bytes size) {
    return memcpy(destination, source, size);
}

void* memory_move(const void* source, void* destination, bytes size) {
    return memmove(destination, source, size);
}

void* memory_set(void* pointer, int value, bytes size) {
    return memset(pointer, value, size);
}

int memory_compare(const void* first, const void* second, bytes size) {
    return memcmp(first, second, size);
}

const void* memory_find(const void* pointer, byte value, bytes size) {
    return memchr(pointer, value, size);
}