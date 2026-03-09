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
#define require_non_null(...) dispatch_require_non_null(__VA_ARGS__,    \
    require_non_null_6,                                                 \
    require_non_null_5,                                                 \
    require_non_null_4,                                                 \
    require_non_null_3,                                                 \
    require_non_null_2,                                                 \
    require_non_null_1                                                  \
)(__VA_ARGS__)

#define dispatch_require_non_null(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define require_non_null_6(pointer, ...) (require_non_null_1(pointer) || require_non_null_5(__VA_ARGS__))

#define require_non_null_5(pointer, ...) (require_non_null_1(pointer) || require_non_null_4(__VA_ARGS__))

#define require_non_null_4(pointer, ...) (require_non_null_1(pointer) || require_non_null_3(__VA_ARGS__))

#define require_non_null_3(pointer, ...) (require_non_null_1(pointer) || require_non_null_2(__VA_ARGS__))

#define require_non_null_2(pointer, ...) (require_non_null_1(pointer) || require_non_null_1(__VA_ARGS__))

#define require_non_null_1(pointer) \
    ((pointer) == nullptr ? set_error(NULL_POINTER_ERROR, "'"#pointer"'"" argument must not be null"), true : false)

#endif