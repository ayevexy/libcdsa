#ifndef LIBCDSA_ERRORS_H
#define LIBCDSA_ERRORS_H

/**
 * @brief Represents an error.
 */
typedef struct Error {
    const char* name;
} * Error;

/**
 * @brief Creates an error with the specified name.
 *
 * @param name the error name
 */
#define ERROR(name) (&(struct Error) { (name) })

/** Constant expression defining the absence of an error. */
constexpr Error NO_ERROR = nullptr;

/**
 * @brief Common runtime error constants.
 */
extern const Error NULL_POINTER_ERROR;
extern const Error ARITHMETIC_ERROR;
extern const Error INDEX_OUT_OF_BOUNDS_ERROR;
extern const Error NO_SUCH_ELEMENT_ERROR;
extern const Error ILLEGAL_ARGUMENT_ERROR;
extern const Error ILLEGAL_STATE_ERROR;
extern const Error UNSUPPORTED_OPERATION_ERROR;
extern const Error CONCURRENT_MODIFICATION_ERROR;
extern const Error MEMORY_ALLOCATION_ERROR;

/**
 * @brief Retrieves the error message of the last captured error.
 *
 * @return error message
 */
const char* error_message(void);

/**
 * @brief Retrieves the error description of the last captured error.
 *
 * @return error description
 */
const char* error_description(void);

/**
 * @brief Prints the last captured error and its location.
 */
void error_print(void);

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
 * @brief Sets an error.
 *
 * @param error the error code to raise
 * @param message optional additional context message
 * @param ... optional format arguments
 */
#define set_error(error, ...) set_error_(error, __VA_OPT__(__VA_ARGS__,) "")

#define set_error_(error, message, ...) set_error(error, __func__, message __VA_OPT__(, ) __VA_ARGS__)

void (set_error)(Error error, const char* location, const char* message, ...);

#endif