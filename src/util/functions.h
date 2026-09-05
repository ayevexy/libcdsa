#ifndef LIBCDSA_FUNCTIONS_H
#define LIBCDSA_FUNCTIONS_H

/**
 * @brief Generic function pointer types for functional-style operations.
 */

/** @brief Function to consume a value (e.g., for `for_each`) */
typedef void (*Consumer)(void*);

/** @brief Function to consume two values */
typedef void (*BiConsumer)(void*, void*);

/** @brief Function to generate a value */
typedef void* (*Supplier)();

/** @brief Predicate function returning true or false for a given element */
typedef bool (*Predicate)(const void*);

/** @brief Operator function that transforms an element and returns the result */
typedef void* (*Operator)(void*);

/** @brief Operator function that transforms two elements and returns the result */
typedef void* (*BiOperator)(void*, void*);

/** @brief Comparator function returning negative, zero, or positive value */
typedef int (*Comparator)(const void*, const void*);

#endif