#ifndef LIBCDSA_CALLBACKS_H
#define LIBCDSA_CALLBACKS_H

/**
 * @file callbacks.h
 * @brief Generic callbacks for pointers, primitive types and strings.
 */

#include "core/types.h"
#include "core/string.h"

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
 * @brief Compare two pointers by their memory addresses.
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
 * @brief No-operation destruct function.
 *
 * @param pointer the pointer to ignore
 */
static inline void noop_destruct(void* pointer) {
    (void) pointer;
}

/* Primitive types */

#define DECLARE_PRIMITIVE_CALLBACKS(T, suffix, format)                                          \
                                                                                                \
    /** @brief Calculate a hash value of a primitive value. */                                  \
    static inline uint64 suffix##_hash_generic(const void* value) {                             \
        return (uint64) *(const T*) value;                                                      \
    }                                                                                           \
                                                                                                \
    /** @brief Check if two primitive values are equal. */                                      \
    static inline bool suffix##_equals_generic(const void* value_a, const void* value_b) {      \
        return *(const T*) value_a == *(const T*) value_b;                                      \
    }                                                                                           \
                                                                                                \
    /** @brief Compare two primitive values. */                                                 \
    static inline int suffix##_compare_generic(const void* value_a, const void* value_b) {      \
        T a = *(const T*) value_a;                                                              \
        T b = *(const T*) value_b;                                                              \
        return (a > b) - (a < b);                                                               \
    }                                                                                           \
                                                                                                \
    /** @brief Convert a primitive value to string. */                                          \
    static inline String suffix##_to_string_generic(const void* value) {                        \
        return string_format(format, *(const T*) value);                                        \
    }

DECLARE_PRIMITIVE_CALLBACKS(bool,    bool,    "%d")

DECLARE_PRIMITIVE_CALLBACKS(char,    char,    "%c")
DECLARE_PRIMITIVE_CALLBACKS(schar,   schar,   "%hhd")
DECLARE_PRIMITIVE_CALLBACKS(uchar,   uchar,   "%hhu")
DECLARE_PRIMITIVE_CALLBACKS(short,   short,   "%hd")
DECLARE_PRIMITIVE_CALLBACKS(ushort,  ushort,  "%hu")

DECLARE_PRIMITIVE_CALLBACKS(int,     int,     "%d")
DECLARE_PRIMITIVE_CALLBACKS(uint,    uint,    "%u")
DECLARE_PRIMITIVE_CALLBACKS(long,    long,    "%ld")
DECLARE_PRIMITIVE_CALLBACKS(ulong,   ulong,   "%lu")
DECLARE_PRIMITIVE_CALLBACKS(llong,   llong,   "%lld")
DECLARE_PRIMITIVE_CALLBACKS(ullong,  ullong,  "%llu")

DECLARE_PRIMITIVE_CALLBACKS(float,   float,   "%g")
DECLARE_PRIMITIVE_CALLBACKS(double,  double,  "%g")
DECLARE_PRIMITIVE_CALLBACKS(ldouble, ldouble, "%Lg")

#undef DECLARE_PRIMITIVE_CALLBACKS

/**
 * @brief String hash function to be used in containers.
 *
 * @param string the string
 *
 * @return the hashed value
 */
static inline uint64 string_hash_generic(const void* string) {
    return string_hash((String) string);
}

/**
 * @brief String equality function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string
 *
 * @return true if equal, false otherwise
 */
static inline bool string_equals_generic(const void* string, const void* other_string) {
    return string_equals((String) string, (String) other_string);
}

/**
 * @brief String comparator function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string
 *
 * @return 0 if equal; negative if the first string is less; positive if greater
 */
static inline int string_compare_generic(const void* string, const void* other_string) {
    return string_compare((String) string, (String) other_string);
}

/**
 * @brief String destructor function to be used in containers.
 *
 * @param string the string
 */
static inline void string_destroy_generic(void* string) {
    string_destroy(&(String) { (String) string });
}

/**
 * @brief String to string function to be used in containers.
 *
 * @param string the string
 *
 * @return the string
 */
static inline String string_to_string_generic(const void* string) {
    return (String) string;
}

#endif
