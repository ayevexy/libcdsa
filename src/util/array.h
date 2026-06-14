#ifndef LIBCDSA_ARRAY_H
#define LIBCDSA_ARRAY_H

#include "errors.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Defines a heap allocated array of type T.
 *
 * This array store its size behind the first element.
 *
 * @param T the type
 */
#define Array(T) T*

typedef struct {
    size_t length;
    max_align_t alignment;
    unsigned char data[];
} ArrayStorage;

/**
 * @brief Creates a new array with the specified length, type and initial elements.
 *
 * @param length the length
 * @param T the type
 * @param ... optional elements
 *
 * @return a newly created array
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define array_new(length, T, ...) array_new(length, sizeof(T), (T[]){__VA_ARGS__})

/**
 * @brief Creates a new array with the specified length, element size and initial data.
 *
 * @param length the length
 * @param element_size the element size
 * @param source the source to be copied (optional)
 *
 * @return a newly created array
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline void* (array_new)(int length, size_t element_size, const void* source) {
    const size_t size = sizeof(ArrayStorage) + length * element_size;
    ArrayStorage* array = malloc(size);
    if (!array) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    array->length = length;
    if (source) {
        memcpy(array->data, source, length * element_size);
    }
    return array->data;
}

/**
 * @brief Retrieves the length of the array.
 *
 * @param array the array
 *
 * @return the array length
 *
 * @warning this should only be called on arrays created by `array_new()`, otherwise it's undefined behavior
 */
#define array_length(array) (((ArrayStorage*)((char*)(array) - offsetof(ArrayStorage, data)))->length)

/**
 * @brief Destroys a previously created array.
 *
 * @param array pointer to the array
 *
 * @post array == nullptr
 *
 * @warning this should only be called on arrays created by `array_new()`, otherwise it's undefined behavior
 */
#define array_destroy(array)                                                                \
    do {                                                                                    \
        _Generic((array),                                                                   \
            const struct String***: destroy_string_array((const struct String** ) *array),  \
            default: (void) array                                                           \
        );                                                                                  \
        free((ArrayStorage*)((char*)(*array) - offsetof(ArrayStorage, data)));              \
        *array = nullptr;                                                                   \
    } while (false)

struct String;

extern void destroy_string_array(Array(const struct String*) strings);

#endif
