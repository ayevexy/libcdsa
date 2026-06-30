#ifndef LIBCDSA_ARRAY_H
#define LIBCDSA_ARRAY_H

#include <stddef.h>

/**
 * @brief Defines a heap allocated array of type T.
 *
 * This array store its size behind the first element.
 *
 * @param T the type
 */
#define Array(T) T*

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
 * @brief Retrieves an element of the array by its index (checking bounds).
 *
 * @param array the array
 * @param index the index
 *
 * @return the element
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array.length
 */
#define array_get(array, index) (array_check_bounds(array, index) ? array[index] : (typeof(*array)){0})

/**
 * @brief Replaces the element at the specified position of the array (checking bounds).
 *
 * @param array the array
 * @param index the index
 * @param ... the new value
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array.length
 */
#define array_set(array, index, ...) (array_check_bounds(array, index) ? (array[index] = __VA_ARGS__, (void) 0) : (void) 0)

/**
 * @brief Retrieves the length of the array.
 *
 * @param array the array
 *
 * @return the array length
 *
 * @warning this should only be called on arrays created by `array_new()` or `array_of()`, otherwise it's undefined behavior
 */
#define array_length(array) (array_length)(array)

/**
 * @brief Destroys a previously created array.
 *
 * @param array pointer to the array
 *
 * @post array == nullptr
 *
 * @warning this should only be called on arrays created by `array_new()` or `array_of()`, otherwise it's undefined behavior
 */
#define array_destroy(array) (_Generic((array),     \
    Array(String): string_array_destroy,            \
    default: (array_destroy)                        \
)(*array), *array = nullptr)

/*------------------------------------------------------------------------------------------------*/

#define count_args(...) dispatch_count_args(0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define dispatch_count_args(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

void* array_create(int, size_t, const void*);

bool array_check_bounds(const void*, int);

int (array_length)(const void*);

void (array_destroy)(void*);

typedef struct String const * String;

extern void string_array_destroy(Array(String));

#endif
