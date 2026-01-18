#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Enumeration of possible runtime errors.
 *
 * This enumeration defines all error types that can be raised and captured
 * by the error handling subsystem.
 *
 * `NO_ERROR` indicates the absence of an error.
 */
typedef enum {
    NO_ERROR = 0,
    NULL_POINTER_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR,
    NO_SUCH_ELEMENT_ERROR,
    ILLEGAL_ARGUMENT_ERROR,
    MEMORY_ALLOCATION_ERROR
} Error;

/**
 * @brief Human-readable error messages corresponding to each `Error` value.
 *
 * The index of each string matches the corresponding `Error` enum value.
 */
extern const char* error_strings[];

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
 * Unlike `error_message`, this returns only the base error description
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
 * @return the captured `Error`
 */
#define attempt(expression) (isolate_error(), (expression), capture_error())

/**
 * @brief Clears the current error state.
 *
 * After calling this function, the error state is reset to `NO_ERROR`.
 */
void isolate_error(void);

/**
 * @brief Captures and returns the current error.
 *
 * @return the current `Error`
 */
Error capture_error(void);

/**
 * @brief Raises an error with a formatted message and returns from the caller.
 *
 * This macro reports an error using the provided message and immediately
 * returns from the calling function.
 *
 * If `message` is an empty string, a default message format is used.
 *
 * @param error the error code to raise
 * @param message additional context message (maybe empty)
 * @param ... optional format arguments
 *
 * @note This macro performs a `return` statement.
 */
#define raise_error(error, message, ...)                                            \
    raise_plain_error(error,                                                        \
        message[0] == '\0' ? "Error at %s(): %s" : "Error at %s(): %s - "message,   \
        __func__,                                                                   \
        error_strings[error],                                                       \
        ##__VA_ARGS__                                                               \
    ); return

/**
 * @brief Raises an error with a formatted message.
 *
 * This function records the given error and formats the provided message
 * using `printf`-style formatting.
 *
 * @param error the error code to raise
 * @param error_message_format format string
 * @param ... format arguments
 */
void raise_plain_error(Error error, const char* error_message_format, ...);

#endif