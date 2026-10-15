#ifndef LIBCDSA_TEST_UTILITIES_H
#define LIBCDSA_TEST_UTILITIES_H

#include "core/types.h"
#include "core/string.h"

#define COUNT_ARGS(...) \
    COUNT_ARGS_(0 __VA_OPT__(,) __VA_ARGS__, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define COUNT_ARGS_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, NAME, ...) NAME

#define TEST_ASSERT_POINTER_ARRAY_ELEMENTS_EQUALS(array, T, ...)    \
    do {                                                            \
        auto array_ = (T[]){__VA_ARGS__};                           \
        int length = COUNT_ARGS(__VA_ARGS__);                       \
        for (int i = 0; i < length; i++) {                          \
            TEST_ASSERT_EQUAL(*(int*) array[i], array_[i]);         \
        }                                                           \
    } while (false)

static inline bool int_equals_generic(const void* a, const void* b) {
    return *(int*) a == *(int*) b;
}

static inline bool char_equals_generic(const void* a, const void* b) {
    return *(char*) a == *(char*) b;
}

static inline int int_compare_generic(const void* a, const void* b) {
    return (*(int*) a > *(int*) b) - (*(int*) a < *(int*) b);
}

static inline int int_compare_reversed_generic(const void* a, const void* b) {
    return int_compare_generic(b, a);
}

static inline int char_compare_generic(const void* a, const void* b) {
    return (*(char*) a > *(char*) b) - (*(char*) a < *(char*) b);
}

static inline String int_to_string_generic(const void* value) {
    return string_format("%d", *(int*) value);
}

static inline String char_to_string_generic(const void* value) {
    return string_format("%c", *(char*) value);
}

typedef struct { char key; int value; } CharIntEntry;

static inline uint64 char_hash_generic(const void *character) {
    return *(const uchar*) character % 5;
}

#endif