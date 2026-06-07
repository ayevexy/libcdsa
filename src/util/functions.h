#ifndef LIBCDSA_FUNCTIONS_H
#define LIBCDSA_FUNCTIONS_H

#include "string.h"
#include <stdint.h>

/**
 * @brief Generic function pointer types for functional-style operations.
 */

/** @brief Function to consume a value (e.g., for `for_each`) */
typedef void (*Consumer)(void*);

/** @brief Function to consume two values */
typedef void (*BiConsumer)(void*, void*);

/** @brief Predicate function returning true or false for a given element */
typedef bool (*Predicate)(const void*);

/** @brief Operator function that transforms an element and returns the result */
typedef void* (*Operator)(void*);

/** @brief Operator function that transforms two elements and returns the result */
typedef void* (*BiOperator)(void*, void*);

/** @brief Comparator function returning negative, zero, or positive value */
typedef int (*Comparator)(const void*, const void*);

/**
 * @brief Calculate a hash value of a pointer by its memory address.
 *
 * @param pointer the pointer
 *
 * @return the hashed value
 */
static inline uint64_t pointer_hash(const void* pointer) {
    return (uintptr_t) pointer * 0x9e3779b97f4a7c15ULL;
}

/**
 * @brief Check if two pointers are equal.
 *
 * @param a first pointer
 * @param b second pointer
 *
 * @return true if a == b, false otherwise
 */
static inline bool pointer_equals(const void* a, const void* b) {
    return a == b;
}

/**
 * @brief Compare two pointers by its memory addresses.
 *
 * @param a first pointer
 * @param b second pointer
 *
 * @return negative if a < b, 0 if a == b, positive if a > b
 */
static inline int pointer_compare(const void* a, const void* b) {
    return ((uintptr_t) a > (uintptr_t) b) - ((uintptr_t) a < (uintptr_t) b);
}

/**
 * @brief Convert a pointer to string (implementation-defined format).
 *
 * @param pointer pointer to convert
 *
 * @return a newly allocated string
 */
static inline String pointer_to_string(const void* pointer) {
    return string_format("%p", pointer);
}

/**
 * @brief No Operation destruct function.
 *
 * @param element the element to be ignored
 */
static inline void noop_destruct(void* element) {
    (void) element;
}

#endif