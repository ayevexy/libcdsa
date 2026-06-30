#include "memory.h"

#include "errors.h"
#include <stdlib.h>
#include <string.h>

void* (new)(size_t size, const void* source) {
    void* pointer = malloc(size);
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
    free(pointer);
}

void* memory_alloc(size_t size) {
    void* pointer = malloc(size);
    if (!pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    return pointer;
}

void* memory_realloc(void* pointer, size_t size) {
    void* new_pointer = realloc(pointer, size);
    if (!new_pointer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to reallocate %zu bytes", size);
        return nullptr;
    }
    return new_pointer;
}

void memory_dealloc(void* pointer) {
    free(pointer);
}