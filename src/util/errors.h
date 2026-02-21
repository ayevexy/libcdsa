#ifndef ERRORS_H
#define ERRORS_H

/**
 * @brief Enumeration of possible runtime errors.
 *
 * This enumeration defines all error types that can be raised and captured
 * by the error handling subsystem.
 *
 * NO_ERROR indicates the absence of an error.
 */
typedef enum {
    UNKNOWN_ERROR = -1,
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    ARITHMETIC_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    NO_SUCH_ELEMENT_ERROR,
    ILLEGAL_ARGUMENT_ERROR,
    ILLEGAL_STATE_ERROR,
    UNSUPPORTED_OPERATION_ERROR,
    CONCURRENT_MODIFICATION_ERROR,
    MEMORY_ALLOCATION_ERROR
} Error;

/** Constant expression defining the total numbers of errors. */
constexpr int ERROR_COUNT = 10;

/**
 * @brief Converts an error enumeration to its string representation.
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
    set_plain_error(error, "Error at %s(): %s - "message, __func__, error_to_string(error) __VA_OPT__(, ) __VA_ARGS__)

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