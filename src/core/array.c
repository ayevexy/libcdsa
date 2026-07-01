#include "array.h"

#include "memory.h"
#include "errors.h"
#include <string.h>

typedef struct {
    int length;
    bytes element_size;
    alignas(max_align_t) uchar data[];
} ArrayStorage;

static ArrayStorage* array_storage(const void* array) {
    return ((ArrayStorage*)((char*)(array) - offsetof(ArrayStorage, data)));
}

void* array_create(int length, bytes element_size, const void* source) {
    const bytes size = sizeof(ArrayStorage) + length * element_size;
    ArrayStorage* array = memory_try_alloc(size);
    if (!array) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    array->length = length;
    array->element_size = element_size;
    if (source) {
        memcpy(array->data, source, length * element_size);
    } else {
        memset(array->data, 0, length * element_size);
    }
    return array->data;
}

bool array_check_bounds(const void* array, int index) {
    if (index < 0 || index >= array_length(array)) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_storage(array)->length);
        return false;
    }
    return true;
}

int (array_length)(const void* array) {
    return array_storage(array)->length;
}

void (array_destroy)(void* array) {
    memory_dealloc(array_storage(array));
}