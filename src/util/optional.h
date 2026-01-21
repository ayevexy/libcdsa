#ifndef OPTIONAL_H
#define OPTIONAL_H

/**
 * @brief A container object which may or may not contain a non-null value.
 *
 * `Optional` is a lightweight value-type structure used to explicitly represent
 * the presence or absence of a value without relying on `nullptr`.
 *
 * An `Optional` is immutable once created.
 */
typedef struct {
    void* const value;
    const bool present;
} Optional;

/**
 * @brief Creates an empty `Optional`.
 *
 * The returned `Optional` has no value present.
 *
 * @return an empty `Optional`
 */
#define optional_empty() (Optional) { .value = (void*) 0xDEADBEEF, .present = false }

/**
 * @brief Creates an `Optional` containing the specified value.
 *
 * @param _value pointer to the value to be wrapped
 *
 * @return an `Optional` containing `_value`
 *
 * @pre _value may be `nullptr`, but the `Optional` will be considered present
 */
#define optional_of(_value) (Optional) { .value = _value, .present = true }

#endif