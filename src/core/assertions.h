#ifndef LIBCDSA_ASSERTIONS_H
#define LIBCDSA_ASSERTIONS_H

/**
 * @file assertions.h
 * @brief Custom assertions.
 */

/**
 * @brief Checks a condition and aborts execution if it evaluates to false.
 *
 * @param expression expression to evaluate
 * @param ... optional message and format arguments
 */
#define assert_condition(expression, ...) \
    assert_internal(expression, "assertion failed: "#expression __VA_OPT__(" "__VA_ARGS__))

/**
 * @brief Checks an invariant and aborts execution if it is violated.
 *
 * @param expression expression to evaluate
 * @param ... optional message and format arguments
 */
#define assert_invariant(expression, ...) \
    assert_internal(expression, "invariant failed: "#expression __VA_OPT__(" "__VA_ARGS__))

/**
 * @brief Checks a function precondition and aborts execution if it is not satisfied.
 *
 * @param expression expression to evaluate
 * @param ... optional message and format arguments
 */
#define assert_precondition(expression, ...) \
    assert_internal(expression, "precondition failed: "#expression __VA_OPT__(" "__VA_ARGS__))

/**
 * @brief Checks a function postcondition and aborts execution if it is not satisfied.
 *
 * @param expression expression to evaluate
 * @param ... optional message and format arguments
 */
#define assert_postcondition(expression, ...) \
    assert_internal(expression, "postcondition failed: "#expression __VA_OPT__(" "__VA_ARGS__))

/**
 * @brief Marks an unreachable section of code and aborts execution if reached.
 */
#define assert_unreachable() \
    assert_internal(false, "unreachable code reached")

/*--------------------------------------------------------------------------------------------------------*/

#ifndef NDEBUG

#define assert_internal(expression, message, ...) \
    ((expression) ? (void) 0 : assertion_failed(__FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__))

#else

#define assert_internal(expression, message, ...) ((void) 0)

#endif

[[noreturn]]
void assertion_failed(const char* source, int line, const char* location, const char* message, ...);

#endif
