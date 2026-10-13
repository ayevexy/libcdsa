#ifndef LIBCDSA_TEST_UTILITIES_H
#define LIBCDSA_TEST_UTILITIES_H

#include "core/types.h"
#include "core/string.h"

#define SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(array, T, ...)    \
    do {                                                            \
        auto array_ = (T[]){__VA_ARGS__};                           \
        int length = count_args(__VA_ARGS__);                       \
        for (int i = 0; i < length; i++) {                          \
            TEST_ASSERT_EQUAL(*(int*) array[i], array_[i]);         \
        }                                                           \
    } while (false)

static inline bool int_pointer_value_equals(const void* a, const void* b) {
    return *(int*) a == *(int*) b;
}

static inline bool char_pointer_value_equals(const void* a, const void* b) {
    return *(char*) a == *(char*) b;
}

static inline int int_pointer_value_compare(const void* a, const void* b) {
    return (*(int*) a > *(int*) b) - (*(int*) a < *(int*) b);
}

static inline int int_pointer_value_compare_reversed(const void* a, const void* b) {
    return int_pointer_value_compare(b, a);
}

static inline int char_pointer_value_compare(const void* a, const void* b) {
    return (*(char*) a > *(char*) b) - (*(char*) a < *(char*) b);
}

static inline String int_pointer_value_to_string(const void* value) {
    return string_format("%d", *(int*) value);
}

static inline String char_pointer_value_to_string(const void* value) {
    return string_format("%c", *(char*) value);
}

typedef struct { char key; int value; } CharIntEntry;

static inline uint64 char_hash(const void *character) {
    return *(const uchar*) character % 5;
}

#endif