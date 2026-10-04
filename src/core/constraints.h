#ifndef LIBCDSA_CONSTRAINTS_H
#define LIBCDSA_CONSTRAINTS_H

/**
 * @file constraints.h
 * @brief Validation and constraint utilities.
 */

#include "errors.h"

/**
 * @brief Requires one or more pointers to be non-null.
 *
 * Sets NULL_POINTER_ERROR if any of the specified pointers is null.
 *
 * This macro accepts one to six pointer arguments.
 *
 * @param ... one or more pointers to validate
 *
 * @return true if the requirement is violated, false otherwise
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
    require((pointer) != nullptr, NULL_POINTER_ERROR, "'"#pointer"'"" must not be null")

/**
 * @brief Requires a number to be positive.
 *
 * Sets ILLEGAL_ARGUMENT_ERROR if the value is not greater than zero.
 *
 * @param value the number to validate
 *
 * @return true if the requirement is violated, false otherwise
 */
#define require_positive(value) \
    require((value) > 0, ILLEGAL_ARGUMENT_ERROR, "'" #value "' must be greater than zero")

/**
 * @brief Requires a number to be non-negative.
 *
 * Sets ILLEGAL_ARGUMENT_ERROR if the value is negative.
 *
 * @param value the number to validate
 *
 * @return true if the requirement is violated, false otherwise
 */
#define require_non_negative(value) \
    require((value) >= 0, ILLEGAL_ARGUMENT_ERROR, "'" #value "' must be non-negative")

/**
 * @brief Requires a number to be within an inclusive range.
 *
 * Sets ILLEGAL_ARGUMENT_ERROR if the value is outside the range.
 *
 * @param value the number to validate
 * @param min the minimum allowed value
 * @param max the maximum allowed value
 *
 * @return true if the requirement is violated, false otherwise
 */
#define require_in_range(value, min, max) \
    require((value) >= (min) && (value) <= (max), ILLEGAL_ARGUMENT_ERROR, "'" #value "' must be between '" #min "' and '" #max "'")

/**
 * @brief Requires an index to be within bounds.
 *
 * Sets INDEX_OUT_OF_BOUNDS_ERROR if the index is negative or greater
 * than or equal to the size.
 *
 * @param index the index to validate
 * @param size the collection size
 *
 * @return true if the requirement is violated, false otherwise
 */
#define require_in_bounds(index, size) \
    require((index) >= 0 && (index) < (size), INDEX_OUT_OF_BOUNDS_ERROR, "'" #index "' out of bounds for '" #size "'")

/**
 * @brief Internal implementation of the require_* constraint macros.
 *
 * @warning This macro is intended for internal use only. Prefer the
 * specialized require_* macros.
 *
 * Sets the specified error with the provided message if the condition
 * is false.
 *
 * @param condition the condition that must be true
 * @param error the error to set if the condition is false
 * @param ... the error message and optional arguments
 *
 * @return true if the requirement is violated, false otherwise.
 */
#define require(condition, error, ...) \
    (!(condition) ? set_error(error, __VA_ARGS__), true : false)

#endif