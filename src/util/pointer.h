#ifndef LIBCDSA_POINTER_H
#define LIBCDSA_POINTER_H

#include "core/string.h"
#include "core/types.h"

/**
 * @brief Calculate a hash value of a pointer by its memory address.
 *
 * @param pointer the pointer
 *
 * @return the hashed value
 */
static inline uint64 pointer_hash(const void* pointer) {
    return (uintptr) pointer * 0x9e3779b97f4a7c15ULL;
}

/**
 * @brief Check if two pointers are equal.
 *
 * @param pointer_a first pointer
 * @param pointer_b second pointer
 *
 * @return true if a == b, false otherwise
 */
static inline bool pointer_equals(const void* pointer_a, const void* pointer_b) {
    return pointer_a == pointer_b;
}

/**
 * @brief Compare two pointers by its memory addresses.
 *
 * @param pointer_a first pointer
 * @param pointer_b second pointer
 *
 * @return negative if a < b, 0 if a == b, positive if a > b
 */
static inline int pointer_compare(const void* pointer_a, const void* pointer_b) {
    return ((uintptr) pointer_a > (uintptr) pointer_b) - ((uintptr) pointer_a < (uintptr) pointer_b);
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
 * @param pointer pointer to be ignored
 */
static inline void noop_destruct(void* pointer) {
    (void) pointer;
}

#endif
