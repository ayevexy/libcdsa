#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Generic function pointer types for functional-style operations.
 */

/** @brief Function to consume a value (e.g., for `for_each`) */
typedef void (*Consumer)(void*);

/** @brief Predicate function returning true or false for a given element */
typedef bool (*Predicate)(const void*);

/** @brief Operator function that transforms an element and returns the result */
typedef void* (*Operator)(void*);

/** @brief Comparator function returning negative, zero, or positive value */
typedef int (*Comparator)(const void*, const void*);

/**
 * @brief No Operation destruct function.
 *
 * @param element The element to be ignored.
 */
static inline void noop_destruct(void* element) {
    (void) element;
}

/**
 * @brief Check if two pointers are equal.
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @return true if a == b, false otherwise.
 */
static inline bool pointer_equals(const void* a, const void* b) {
    return a == b;
}

/**
 * @brief Compare two pointers.
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @return Negative if a < b, 0 if a == b, positive if a > b (compares addresses).
 */
static inline int compare_pointers(const void* a, const void* b) {
    return ((uintptr_t) a > (uintptr_t) b) - ((uintptr_t) a < (uintptr_t) b);
}

/**
 * @brief Convert a pointer to string (implementation-defined format).
 *
 * @param value Pointer to convert.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
static inline int pointer_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%p", value);
}

#endif