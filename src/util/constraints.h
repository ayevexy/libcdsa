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
#define require_non_null(...) _dispatch_require_non_null(__VA_ARGS__,    \
    _require_non_null_6,                                                 \
    _require_non_null_5,                                                 \
    _require_non_null_4,                                                 \
    _require_non_null_3,                                                 \
    _require_non_null_2,                                                 \
    _require_non_null_1                                                  \
)(__VA_ARGS__)

#define _dispatch_require_non_null(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define _require_non_null_6(pointer, ...) (_require_non_null_1(pointer) || _require_non_null_5(__VA_ARGS__))

#define _require_non_null_5(pointer, ...) (_require_non_null_1(pointer) || _require_non_null_4(__VA_ARGS__))

#define _require_non_null_4(pointer, ...) (_require_non_null_1(pointer) || _require_non_null_3(__VA_ARGS__))

#define _require_non_null_3(pointer, ...) (_require_non_null_1(pointer) || _require_non_null_2(__VA_ARGS__))

#define _require_non_null_2(pointer, ...) (_require_non_null_1(pointer) || _require_non_null_1(__VA_ARGS__))

#define _require_non_null_1(pointer) \
    ((pointer) == nullptr ? set_error(NULL_POINTER_ERROR, "'"#pointer"'"" argument must not be null"), true : false)

#endif