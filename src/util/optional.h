#ifndef LIBCDSA_OPTIONAL_H
#define LIBCDSA_OPTIONAL_H

#include "core/errors.h"
#include "functions.h"

/**
 * @file optional.h
 * @brief Optional value container.
 */

/**
 * @brief A container object which may or may not contain a value.
 */
typedef struct {
    void* const value;
    const bool present;
} Optional;

/**
 * @brief Creates an empty optional.
 *
 * @return an empty optional
 */
static inline Optional optional_empty(void) {
    return (Optional) { .present = false };
}

/**
 * @brief Creates an optional containing the specified value.
 *
 * @param value the value
 *
 * @return an optional containing the value
 */
static inline Optional optional_of(void* value) {
    return (Optional) { .value = value, .present = true };
}

/**
 * @brief Creates an optional containing the specified value if non-null,
 * or an empty optional otherwise.
 *
 * @param value the value
 *
 * @return an optional containing the value, or an empty optional
 */
static inline Optional optional_of_nullable(void* value) {
    return value ? optional_of(value) : optional_empty();
}

/**
 * @brief Checks whether the optional contains a value.
 *
 * @param optional the optional
 *
 * @return true if the optional contains a value
 */
static inline bool optional_is_present(Optional optional) {
    return optional.present;
}

/**
 * @brief Checks whether the optional is empty.
 *
 * @param optional the optional
 *
 * @return true if the optional is empty
 */
static inline bool optional_is_empty(Optional optional) {
    return !optional.present;
}

/**
 * @brief Retrieves the contained value.
 *
 * @param optional the optional
 *
 * @return the contained value
 *
 * @exception NO_SUCH_ELEMENT_ERROR if optional is empty
 */
static inline void* optional_get(Optional optional) {
    if (optional_is_empty(optional)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "optional is empty");
        return nullptr;
    }
    return optional.value;
}

/**
 * @brief Retrieves the contained value, or a fallback if the optional is empty.
 *
 * @param optional the optional
 * @param fallback the fallback value
 *
 * @return the contained value or the fallback
 */
static inline void* optional_or_else(Optional optional, void* fallback) {
    return optional.present ? optional.value : fallback;
}

/**
 * @brief Performs the specified action if the optional contains a value.
 *
 * @param optional the optional
 * @param action the action to perform
 */
static inline void optional_if_present(Optional optional, Consumer action) {
    if (optional.present) {
        action(optional.value);
    }
}

/**
 * @brief Applies the specified function to the contained value.
 *
 * @param optional the optional
 * @param mapper the function to apply to the contained value
 *
 * @return an optional containing the result, or an empty optional
 */
static inline Optional optional_transform(Optional optional, Operator mapper) {
    return optional.present
        ? optional_of(mapper(optional.value))
        : optional_empty();
}

/**
 * @brief Returns the optional if it contains a value matching the predicate,
 * or an empty optional otherwise.
 *
 * @param optional the optional
 * @param predicate the predicate to test the contained value
 *
 * @return the original optional if the predicate matches, or an empty optional
 */
static inline Optional optional_keep_if(Optional optional, Predicate predicate) {
    return optional.present && predicate(optional.value)
        ? optional
        : optional_empty();
}

#endif