#ifndef LIBCDSA_OPTIONAL_H
#define LIBCDSA_OPTIONAL_H

#include "core/functions.h"

/**
 * @file optional.h
 * @brief Optional value container.
 */

/**
 * @brief A container object which may or may not contain a value.
 */
typedef struct Optional {
    void* const value;
    const bool present;
} Optional;

/**
 * @brief Creates an empty optional.
 *
 * @return an empty optional
 */
Optional optional_empty(void);

/**
 * @brief Creates an optional containing the specified value.
 *
 * @param value the value
 *
 * @return an optional containing the value
 */
Optional optional_of(void* value);

/**
 * @brief Creates an optional containing the specified value if non-null,
 * or an empty optional otherwise.
 *
 * @param value the value
 *
 * @return an optional containing the value, or an empty optional
 */
Optional optional_of_nullable(void* value);

/**
 * @brief Checks whether the optional contains a value.
 *
 * @param optional the optional
 *
 * @return true if the optional contains a value
 */
bool optional_is_present(Optional optional);

/**
 * @brief Checks whether the optional is empty.
 *
 * @param optional the optional
 *
 * @return true if the optional is empty
 */
bool optional_is_empty(Optional optional);

/**
 * @brief Retrieves the contained value.
 *
 * @param optional the optional
 *
 * @return the contained value
 *
 * @exception NO_SUCH_ELEMENT_ERROR if optional is empty
 */
void* optional_get(Optional optional);

/**
 * @brief Retrieves the contained value, or a fallback if the optional is empty.
 *
 * @param optional the optional
 * @param fallback the fallback value
 *
 * @return the contained value or the fallback
 */
void* optional_or_else(Optional optional, void* fallback);

/**
 * @brief Performs the specified action if the optional contains a value.
 *
 * @param optional the optional
 * @param action the action to perform
 */
void optional_if_present(Optional optional, Consumer action);

/**
 * @brief Applies the specified function to the contained value.
 *
 * @param optional the optional
 * @param mapper the function to apply to the contained value
 *
 * @return an optional containing the result, or an empty optional
 */
Optional optional_transform(Optional optional, Operator mapper);

/**
 * @brief Returns the optional if it contains a value matching the predicate,
 * or an empty optional otherwise.
 *
 * @param optional the optional
 * @param predicate the predicate to test the contained value
 *
 * @return the original optional if the predicate matches, or an empty optional
 */
Optional optional_keep_if(Optional optional, Predicate predicate);

#endif