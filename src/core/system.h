#ifndef LIBCDSA_SYSTEM_H
#define LIBCDSA_SYSTEM_H

#include "types.h"
#include "errors.h"
#include "array.h"
#include "string.h"
#include "file.h"

/**
 * @brief Returns the system input stream (defaults to stdin).
 *
 * @return the file input stream
 */
File* system_input();

/**
 * @brief Replaces the system input stream.
 *
 * @param input the new file input stream
 *
 * @exception NULL_POINTER_ERROR if input is null
 */
void system_change_input(File* input);

/**
 * @brief Returns the system output stream (defaults to stdout).
 *
 * @return the file output stream
 */
File* system_output();

/**
 * @brief Replaces the system output stream.
 *
 * @param output the new file output stream
 *
 * @exception NULL_POINTER_ERROR if output is null
 */
void system_change_output(File* output);

/**
 * @brief Returns the system error stream (defaults to stderr).
 *
 * @return the file error stream
 */
File* system_error();

/**
 * @brief Replaces the system error stream.
 *
 * @param error the new file error stream
 *
 * @exception NULL_POINTER_ERROR if error is null
 */
void system_change_error(File* error);

/**
 * @brief Reads a single character from the system input stream.
 *
 * @return a char
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
char system_read();

/**
 * @brief Reads a string line from the system input stream.
 *
 * @param ... the buffer size (default is 256 bytes)
 *
 * @return the string line
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the string fails
 */
#define system_read_line(...) system_read_line_(__VA_OPT__(__VA_ARGS__,) 256, __VA_ARGS__)

#define system_read_line_(size, ...) system_read_line((char[size]){}, size)

String (system_read_line)(char* buffer, bytes size);

/**
 * @brief Writes a string to the system output stream.
 *
 * @param string the string
 * @param ... additional arguments
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
#define system_write(string, ...) system_write(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write)(struct String string, ...);

/**
 * @brief Writes a string to the system output stream, then terminates the line.
 *
 * @param string the string
 * @param ... additional arguments
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
#define system_write_line(string, ...) system_write_line(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_line)(struct String string, ...);

/**
 * @brief Writes a string to the system error stream.
 *
 * @param string the string
 * @param ... additional arguments
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
#define system_write_error(string, ...) system_write_error(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_error)(struct String string, ...);

/**
 * @brief Writes a string to the system error stream, then terminates the line.
 *
 * @param string the string
 * @param ... additional arguments
 *
 * @exception FILE_INPUT_OUTPUT_ERROR if an input/output error occurs
 * @exception FILE_SYSTEM_ERROR if a file system error occurs
 */
#define system_write_error_line(string, ...) system_write_error_line(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_error_line)(struct String string, ...);

/**
 * @brief Retrieves an environment variable value as a string.
 *
 * @param name the environment variable name
 *
 * @return the variable value as a string, or nullptr if the variable isn't defined
 *
 * @xception NULL_POINTER_ERROR if name is null
 */
const char* system_get_environment_variable(const char* name);

#ifdef __linux__

/**
 * @brief Sets the string value of an environment variable.
 *
 * @param name the environment variable name
 * @param value the new environment variable value
 *
 * @exception NULL_POINTER_ERROR if name is null
 * @exception ILLEGAL_ARGUMENT_ERROR if name is empty or is the equal sign "="
 */
void system_set_environment_variable(const char* name, const char* value);

/**
 * @brief Removes an environment variable.
 *
 * @param name the environment variable name
 *
 * @exception NULL_POINTER_ERROR if name is null
 * @exception ILLEGAL_ARGUMENT_ERROR if name is empty or is the equal sign "="
 */
void system_remove_environment_variable(const char* name);

#endif

/**
 * @brief Retrieves the underlying platform information name.
 *
 * @return the platform name
 */
const char* system_platform_name();

/**
 * @brief Retrieves the underlying platform information version.
 *
 * @return the platform version
 */
const char* system_platform_version();

/**
 * @brief Retrieves the underlying platform information architecture.
 *
 * @return the platform architecture
 */
const char* system_platform_architecture();

#ifdef __linux__

/**
 * @brief Enumeration of possible process errors.
 */
typedef enum : Error {
    PROCESS_CREATION_ERROR = ERROR_BASE(PROCESS_ERROR_CATEGORY),
    PROCESS_EXECUTION_ERROR,
    PROCESS_NOT_FOUND_ERROR,
    PROCESS_ACCESS_DENIED_ERROR,
    PROCESS_INTERRUPTED_ERROR,
} ProcessError;

/**
 * @brief Creates an operating system process.
 *
 * If no arguments are provided, the function behaves like fork().
 * Otherwise, the first argument is treated as the executable and the
 * remaining arguments as its parameters.
 *
 * @param ... the executable followed by its parameters (optional)
 *
 * @return the process id, 0 in the child process, or -1 if the operation fails
 *
 * @exception PROCESS_CREATION_ERROR if the process cannot be created
 * @exception PROCESS_EXECUTION_ERROR if the executable cannot be executed
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to execute the executable is denied
 */
#define system_process_create(...) system_process_create(count_args(__VA_ARGS__) __VA_OPT__(,) __VA_ARGS__)

intptr (system_process_create)(int count, ...);

/**
 * @brief Waits until a process finishes its execution.
 *
 * @param process_id the process id
 *
 * @return 0..255 if the process exits normally
 *         -1 if waiting fails
 *         -N if the process is terminated by signal N
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process is not a child process
 * @exception PROCESS_INTERRUPTED_ERROR if waiting is interrupted by a signal
 */
int system_process_wait(intptr process_id);

/**
 * @brief Waits for a process to finish its execution until a timeout expires.
 *
 * @param process_id the process id
 * @param timeout the maximum waiting time in milliseconds
 * @param timed_out a variable to store whether the timeout expired before the process finished
 *
 * @return 0..255 if the process exits normally
 *         -1 if waiting fails or the timeout expires
 *         -N if the process is terminated by signal N
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process is not a child process
 */
int system_process_wait_timeout(intptr process_id, uint64 timeout, bool* timed_out);

/**
 * @brief Checks whether a process exists.
 *
 * @param process_id the process id
 *
 * @return true if the process exists, false otherwise
 */
bool system_process_is_alive(intptr process_id);

/**
 * @brief Sends a signal to a process.
 *
 * @param process_id the process id
 * @param signum the signal number
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process does not exist
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to signal the process is denied
 * @exception ILLEGAL_ARGUMENT_ERROR if the signal number is invalid
 */
void system_process_signal(intptr process_id, int signum);

/**
 * @brief Suspends the execution of a process.
 *
 * @param process_id the process id
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process does not exist
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to signal the process is denied
 */
void system_process_suspend(intptr process_id);

/**
 * @brief Resumes the execution of a process.
 *
 * @param process_id the process id
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process does not exist
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to signal the process is denied
 */
void system_process_resume(intptr process_id);

/**
 * @brief Requests the termination of a process.
 *
 * @param process_id the process id
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process does not exist
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to signal the process is denied
 */
void system_process_terminate(intptr process_id);

/**
 * @brief Forcibly terminates a process.
 *
 * @param process_id the process id
 *
 * @exception PROCESS_NOT_FOUND_ERROR if the process does not exist
 * @exception PROCESS_ACCESS_DENIED_ERROR if permission to signal the process is denied
 */
void system_process_kill(intptr process_id);

/**
 * @brief Enumeration of possible thread errors.
 */
typedef enum : Error {
    THREAD_CREATION_ERROR = ERROR_BASE(THREAD_ERROR_CATEGORY),
    THREAD_NOT_FOUND_ERROR,
    THREAD_ILLEGAL_STATE_ERROR,
    THREAD_DEADLOCK_ERROR
} ThreadError;

/**
 * @brief Creates a thread.
 *
 * @param routine the routine executed by the thread
 * @param ... the routine argument (optional)
 *
 * @return the thread id, or -1 if creation fails
 *
 * @exception NULL_POINTER_ERROR if routine is null
 * @exception THREAD_CREATION_ERROR if the thread cannot be created
 */
#define system_thread_create(function, ...) system_thread_create_(function, __VA_OPT__(__VA_ARGS__,) nullptr)

#define system_thread_create_(function, argument, ...) system_thread_create(function, argument)

intptr (system_thread_create)(void* (*routine)(void*), void* argument);

/**
 * @brief Waits for a thread to terminate.
 *
 * @param thread_id the thread id
 *
 * @return the thread result, or nullptr if the operation fails
 *
 * @exception THREAD_NOT_FOUND_ERROR if the thread does not exist
 * @exception THREAD_ILLEGAL_STATE_ERROR if the thread is not joinable or is already being joined
 * @exception THREAD_DEADLOCK_ERROR if joining the thread would cause a deadlock
 */
void* system_thread_join(intptr thread_id);

/**
 * @brief Detaches a thread.
 *
 * @param thread_id the thread id
 *
 * @exception THREAD_NOT_FOUND_ERROR if the thread does not exist
 * @exception THREAD_ILLEGAL_STATE_ERROR if the thread is not joinable
 */
void system_thread_detach(intptr thread_id);

/**
 * @brief Requests cancellation of a thread.
 *
 * @param thread_id the thread id
 *
 * @exception THREAD_NOT_FOUND_ERROR if the thread does not exist
 */
void system_thread_interrupt(intptr thread_id);

/**
 * @brief Returns the current thread id.
 *
 * @return the current thread id
 */
intptr system_thread_current();

/**
 * @brief Terminates the current thread.
 *
 * @param result the thread result
 */
_Noreturn void system_thread_exit(void* result);

/**
 * @brief Suspends the current thread.
 *
 * @param milliseconds the duration in milliseconds
 */
void system_thread_sleep(uint64 milliseconds);

#endif

/**
 * @brief Executes a command line through the system shell.
 *
 * @param command the command
 *
 * @return 0..255 if it exits normally
 *         -1 if operation fails
 *         -N if the process is terminated by signal N
 *
 * @exception NULL_POINTER_ERROR if command is null
 * @exception RUNTIME_ERROR if the operation fails for some reason
 *
 * @note On Linux, the return value is POSIX-specific; on other platforms,
 *       the underlying system() status is returned.
 */
int system_execute(const char* command);

/**
 * @brief Returns the current system time.
 *
 * @return the number of milliseconds elapsed since the Unix epoch.
 */
uint64 system_current_time(void);

/**
 * @brief Returns a monotonic system time value.
 *
 * @return a time value in nanoseconds suitable for measuring elapsed time.
 */
uint64 system_elapsed_time(void);

/**
 * @brief Terminate the program execution with a status code.
 *
 * @param status the status code
 */
void system_exit(int status);

#endif
