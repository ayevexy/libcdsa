#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "errors.h"

/**
 * @brief Verifies that one or more pointers are not null
 * and set NULL_POINTER_ERROR with a predefined message format if true.
 *
* This macro can accept 1 to 6 pointer arguments.
 *
 * @param ... one or more pointers to check.
 */
#define set_error_on_null(...) dispatch_set_error_on_null(__VA_ARGS__,  \
    set_error_on_null_6,                                                \
    set_error_on_null_5,                                                \
    set_error_on_null_4,                                                \
    set_error_on_null_3,                                                \
    set_error_on_null_2,                                                \
    set_error_on_null_1,                                                \
)(__VA_ARGS__)

#define dispatch_set_error_on_null(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define set_error_on_null_6(pointer, ...) (set_error_on_null_1(pointer) || set_error_on_null_5(__VA_ARGS__))

#define set_error_on_null_5(pointer, ...) (set_error_on_null_1(pointer) || set_error_on_null_4(__VA_ARGS__))

#define set_error_on_null_4(pointer, ...) (set_error_on_null_1(pointer) || set_error_on_null_3(__VA_ARGS__))

#define set_error_on_null_3(pointer, ...) (set_error_on_null_1(pointer) || set_error_on_null_2(__VA_ARGS__))

#define set_error_on_null_2(pointer, ...) (set_error_on_null_1(pointer) || set_error_on_null_1(__VA_ARGS__))

#define set_error_on_null_1(pointer) \
    ((pointer) == nullptr ? set_error(NULL_POINTER_ERROR, "'"#pointer"'"" argument must not be null"), true : false)

#endif