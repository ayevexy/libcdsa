#ifndef LIBCDSA_MACROS_H
#define LIBCDSA_MACROS_H

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
 * @note supports up to 10 arguments.
 */
#define count_args(...) count_args_(0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define count_args_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

#endif
