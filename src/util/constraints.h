#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include <assert.h>

/**
 * @brief Asserts that one or more pointers are not null.
 *
 * This macro can accept 1 to 6 pointer arguments. Each pointer is
 * checked using `assert(pointer != nullptr)`.
 *
 * @param ... one or more pointers to check.
 *
 * @note Assertions are active only if NDEBUG is not defined.
 */
#define require_non_null(...) GET_MACRO(__VA_ARGS__,    \
    REQUIRE_NON_NULL_6,                                 \
    REQUIRE_NON_NULL_5,                                 \
    REQUIRE_NON_NULL_4,                                 \
    REQUIRE_NON_NULL_3,                                 \
    REQUIRE_NON_NULL_2,                                 \
    REQUIRE_NON_NULL_1                                  \
)(__VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define REQUIRE_NON_NULL_1(pointer) assert(pointer != nullptr)

#define REQUIRE_NON_NULL_2(pointer, ...) assert(pointer != nullptr); REQUIRE_NON_NULL_1(__VA_ARGS__)

#define REQUIRE_NON_NULL_3(pointer, ...) assert(pointer != nullptr); REQUIRE_NON_NULL_2(__VA_ARGS__)

#define REQUIRE_NON_NULL_4(pointer, ...) assert(pointer != nullptr); REQUIRE_NON_NULL_3(__VA_ARGS__)

#define REQUIRE_NON_NULL_5(pointer, ...) assert(pointer != nullptr); REQUIRE_NON_NULL_4(__VA_ARGS__)

#define REQUIRE_NON_NULL_6(pointer, ...) assert(pointer != nullptr); REQUIRE_NON_NULL_5(__VA_ARGS__)

#endif