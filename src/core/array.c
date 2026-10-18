#include "array.h"

#include "errors.h"
#include "memory.h"
#include "constraints.h"

typedef struct {
    int length;
    bytes element_size;
    alignas(max_align_t) uchar data[];
} Array;

static Array* array_self(const void* array) {
    return ((Array*)((char*)(array) - offsetof(Array, data)));
}

void* array_create(int length, bytes element_size, const void* source) {
    if (require_non_negative(length)) return nullptr;

    const bytes size = sizeof(Array) + length * element_size;

    Array* array = memory_try_alloc(size);

    if (!array) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }

    array->length = length;
    array->element_size = element_size;

    if (source) {
        memory_copy(source, array->data, length * element_size);
    } else {
        memory_set(array->data, 0, length * element_size);
    }

    return array->data;
}

bool array_check_bounds(const void* array, int index) {
    if (require_non_null(array)) return false;

    if (index < 0 || index >= array_length(array)) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_self(array)->length);
        return false;
    }
    return true;
}

int (array_length)(const void* array) {
    if (require_non_null(array)) return -1;

    return array_self(array)->length;
}

bool array_equals(const Array(void) array, const Array(void) other) {
    if (require_non_null(array, other)) return false;

    if (array_length(array) != array_length(other)){
        return false;
    }
    if (array_self(array)->element_size != array_self(other)->element_size) {
        return false;
    }
    const bytes size = array_length(array) * array_self(array)->element_size;

    return memory_compare(array, other, size) == 0;
}

Array(void) array_copy(const Array(void) array) {
    if (require_non_null(array)) return nullptr;

    return array_create(array_length(array), array_self(array)->element_size, array_self(array)->data);
}

void (array_destroy)(void* array) {
    if (require_non_null(array)) return;

    memory_dealloc(array_self(array));
}