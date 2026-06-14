#ifndef LIBCDSA_MEMORY_H
#define LIBCDSA_MEMORY_H

#include "errors.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocate and initialize an object of type `T` on the heap.
 *
 * If no initial value is provided, the object will be initialized with
 * the default values for the type.
 *
 * @param T the type of the object to allocate
 * @param ... optional literal value used to initialize the object
 *
 * @return a pointer to the newly allocated object, or nullptr on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define new(T, ...) ((T*) (new)(sizeof(T), &(T){__VA_ARGS__}))

static inline void* (new)(size_t size, const void* source) {
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

/**
 * @brief Deallocate a memory block and set its pointer to nullptr.
 *
 * Frees the memory previously allocated with `malloc` (or `new`)
 * and assigns `nullptr` to the pointer to avoid dangling references.
 *
 * @param pointer pointer to the memory block
 */
#define delete(pointer)     \
    do {                    \
        free(pointer);      \
        pointer = nullptr;  \
    } while (false)

static inline void (delete)(void* pointer) {
    free(pointer);
}

#endif