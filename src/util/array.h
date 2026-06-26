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
    int length;
    alignas(max_align_t) unsigned char data[];
} ArrayStorage;

/**
 * @brief Creates a new array with the specified length and type (filled with zeros).
 *
 * @param length the length
 * @param T the type
 *
 * @return a newly created array
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define array_new(length, T) array_create(length, sizeof(T), nullptr)

/**
 * @brief Creates a new array with the specified type and elements.
 *
 * @param T the type
 * @param ... elements
 *
 * @return a newly created array
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define array_of(T, ...) array_create(count_args(__VA_ARGS__), sizeof(T), (T[]){__VA_ARGS__})

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
static inline void* array_create(int length, size_t element_size, const void* source) {
    const size_t size = sizeof(ArrayStorage) + length * element_size;
    ArrayStorage* array = malloc(size);
    if (!array) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate %zu bytes", size);
        return nullptr;
    }
    array->length = length;
    if (source) {
        memcpy(array->data, source, length * element_size);
    } else {
        memset(array->data, 0, length * element_size);
    }
    return array->data;
}

/**
 * @brief Retrieves an element of the array by its index (checking bounds).
 *
 * @param array the array
 * @param index the index
 *
 * @return the element
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array.length
 */
#define array_get(array, index) \
    ((index < 0 || index >= array_length(array)) ? (set_error(INDEX_OUT_OF_BOUNDS_ERROR, ""), *(typeof(array)){0}) : array[index])

/**
 * @brief Replaces the element at the specified position of the array (checking bounds).
 *
 * @param array the array
 * @param index the index
 * @param value the new value
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array.length
 */
#define array_set(array, index, value) \
    ((index < 0 || index >= array_length(array)) ? set_error(INDEX_OUT_OF_BOUNDS_ERROR, "") : (array[index] = value))

/**
 * @brief Retrieves the length of the array.
 *
 * @param array the array
 *
 * @return the array length
 *
 * @warning this should only be called on arrays created by `array_new()`, otherwise it's undefined behavior
 */
#define array_length(array) \
    (((ArrayStorage*)((char*)(array) - offsetof(ArrayStorage, data)))->length)

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

/*------------------------------------------------------------------------------*/

#define count_args(...) dispatch_count_args(0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define dispatch_count_args(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

#endif
