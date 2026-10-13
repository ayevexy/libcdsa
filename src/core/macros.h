#ifndef LIBCDSA_MACROS_H
#define LIBCDSA_MACROS_H

/**
 * @file macros.h
 * @brief Utility macros.
 */

/**
 * @brief Concatenates two tokens.
 */
#define concat(a, b) concat_(a, b)

#define concat_(a, b) a##b

/**
 * @brief Expands a macro argument before stringifying it.
 */
#define stringify(value) stringify_(value)

#define stringify_(value) #value

/**
 * @brief Counts the number of arguments.
 *
 * @note supports up to 15 arguments.
 */
#define count_args(...) count_args_(0 __VA_OPT__(,) __VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define count_args_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, NAME, ...) NAME

#endif
