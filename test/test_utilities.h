#ifndef LIBCDSA_TEST_UTILITIES_H
#define LIBCDSA_TEST_UTILITIES_H

#include "core/string.h"
#include "core/types.h"

#define SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TEST_ASSERT_ARRAY_EQUALS(array_a, array_b)              \
    for (int i = 0; i < SIZE(array_a); i++) {                   \
        TEST_ASSERT_EQUAL(array_a[i], *(int*) array_b[i]);      \
    }

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