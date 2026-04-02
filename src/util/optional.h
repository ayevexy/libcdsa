#ifndef LIBCDSA_OPTIONAL_H
#define LIBCDSA_OPTIONAL_H

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
static inline Optional optional_empty() {
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

#endif