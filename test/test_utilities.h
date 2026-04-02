#ifndef LIBCDSA_TEST_UTILITIES_H
#define LIBCDSA_TEST_UTILITIES_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

static inline int int_pointer_value_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%d", *(int*) value);
}

static inline int char_pointer_value_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%c", *(char*) value);
}

static inline int compare_int_pointers_value(const void* a, const void* b) {
    return (*(int*) a > *(int*) b) - (*(int*) a < *(int*) b);
}

static inline int compare_char_pointers_value(const void* a, const void* b) {
    return (*(char*) a > *(char*) b) - (*(char*) a < *(char*) b);
}

typedef struct { char key; int value; } CharIntEntry;

static inline uint64_t char_hash(const void *character) {
    return *(const unsigned char *) character % 5;
}

#endif