#ifndef LIBCDSA_ERRORS_H
#define LIBCDSA_ERRORS_H

#include "types.h"

/**
 * @brief Error code.
 *
 * Encodes an error category and error value.
 *
 * @note The upper 8 bits identify the error category and the lower 8 bits identify the specific error.
 */
typedef uint16 Error;

/** Constant expression defining the absence of an error. */
constexpr Error NO_ERROR = 0;

/**
 * @brief Enumeration of error categories.
 */
typedef enum : uint8 {
    RUNTIME_ERROR_CATEGORY = 1,
    FILE_SYSTEM_ERROR_CATEGORY,
    PROCESS_ERROR_CATEGORY,
    THREAD_ERROR_CATEGORY,
    SEQUENCE_ERROR_CATEGORY
} ErrorCategory;

/**
 * @brief Creates the base value for an error category.
 *
 * @param category the error category
 *
 * @return the category encoded in the upper 8 bits
 *
 * @note Intended for the first member of an error enum.
 */
#define ERROR_BASE(category) ((category) << 8)

/**
 * @brief Enumeration of common runtime errors.
 */
typedef enum : uint16 {
    RUNTIME_ERROR = ERROR_BASE(RUNTIME_ERROR_CATEGORY),
    NULL_POINTER_ERROR,
    ARITHMETIC_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    NO_SUCH_ELEMENT_ERROR,
    ILLEGAL_ARGUMENT_ERROR,
    ILLEGAL_STATE_ERROR,
    UNSUPPORTED_OPERATION_ERROR,
    CONCURRENT_MODIFICATION_ERROR,
    MEMORY_ALLOCATION_ERROR
} RuntimeError;

/**
 * @brief Converts an error to its string representation.
 *
 * @param error the error to be converted
 *
 * @return the string representation of the error
 */
const char* error_to_string(Error error);

/**
 * @brief Retrieves the formatted error message of the last captured error.
 *
 * This message may include contextual information such as function names
 * or additional details supplied when the error was raised.
 *
 * @return formatted error message
 */
const char* error_message(void);

/**
 * @brief Retrieves the plain error message of the last captured error.
 *
 * Unlike error_message, this returns only the base error description
 * without additional context.
 *
 * @return plain error message
 */
const char* plain_error_message(void);

/**
 * @brief Executes an expression while isolating and capturing any raised error.
 *
 * This macro clears the current error state, evaluates the given expression,
 * and captures any error raised during its execution.
 *
 * @param expression expression to be evaluated, must be a single function call (could assign to a variable)
 *
 * @return the captured Error
 */
#define attempt(expression) (isolate_error(), (expression), capture_error())

/**
 * @brief Declares a struct variable containing an arbitrary type value and an Error.
 *
 * @param T value type
 */
#define Result(T) struct { T value; Error error; }

/**
 * @brief Executes an expression while isolating and capturing any raised error
 * then returns the expression value alongside the captured error in a tuple.
 *
 * @param expression expression to be evaluated, must be a single function call
 *
 * @return the expression value and the captured error in a tuple
 */
#define try(expression) { (isolate_error(), (expression)), capture_error() }

/**
 * @brief Clears the current error state.
 *
 * After calling this function, the error state is reset to NO_ERROR.
 */
void isolate_error(void);

/**
 * @brief Captures and returns the current error.
 *
 * @return the current Error
 */
Error capture_error(void);

/**
 * @brief Sets an error with a formatted message.
 *
 * This macro reports an error using the provided message.
 *
 * @param error the error code to raise
 * @param message additional context message
 * @param ... optional format arguments
 */
#define set_error(error, message, ...) \
    set_plain_error(error, "%s: "message, error_to_string(error) __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Sets an error with a formatted message.
 *
 * This function records the given error and formats the provided message
 * using printf-style formatting.
 *
 * @param error the error code to raise
 * @param error_message_format format string
 * @param ... format arguments
 */
void set_plain_error(Error error, const char* error_message_format, ...);

#endif