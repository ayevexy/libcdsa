#ifndef LIBCDSA_SYSTEM_H
#define LIBCDSA_SYSTEM_H

#include "types.h"
#include "string.h"

/** @brief The system input stream (initially nullptr, defaults to stdin) */
extern void* system_input;

/** @brief The system output stream (initially nullptr, defaults to stdout) */
extern void* system_output;

/** @brief The system error stream (initially nullptr, defaults to stderr) */
extern void* system_error;

/**
 * @brief Reads a single character from the system input stream.
 *
 * @return a char
 */
char system_read();

/**
 * @brief Reads a string line from the system input stream.
 *
 * @param ... the buffer size (default is 256 bytes)
 *
 * @return the string line
 */
#define system_read_line(...) system_read_line_(__VA_OPT__(__VA_ARGS__,) 256, __VA_ARGS__)

#define system_read_line_(size, ...) system_read_line((char[size]){}, size)

String (system_read_line)(char* buffer, bytes size);

/**
 * @brief Writes a string to the system output stream.
 *
 * @param string the string
 * @param ... additional arguments
 */
#define system_write(string, ...) system_write(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write)(struct String string, ...);

/**
 * @brief Writes a string to the system output stream, then terminates the line.
 *
 * @param string the string
 * @param ... additional arguments
 */
#define system_write_line(string, ...) system_write_line(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_line)(struct String string, ...);

/**
 * @brief Writes a string to the system error stream.
 *
 * @param string the string
 * @param ... additional arguments
 */
#define system_write_error(string, ...) system_write_error(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_error)(struct String string, ...);

/**
 * @brief Writes a string to the system error stream, then terminates the line.
 *
 * @param string the string
 * @param ... additional arguments
 */
#define system_write_error_line(string, ...) system_write_error_line(dispatch_string_type(string) __VA_OPT__(,) __VA_ARGS__)

void (system_write_error_line)(struct String string, ...);

/**
 * @brief Retrieves an environment variable value as a string.
 *
 * @param name the name
 *
 * @return a string
 */
const char* system_get_environment_variable(const char* name);

/**
 * @brief Sets the string value of an environment variable.
 *
 * @param name the name
 * @param value the value
 */
void system_set_environment_variable(const char* name, const char* value);

/**
 * @brief Removes an environment variable.
 *
 * @param name the name
 */
void system_remove_environment_variable(const char* name);

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

/**
 * @brief Executes a command line through the system shell.
 *
 * @param command the command
 */
int system_execute(const char* command);

/**
 * @brief Terminate the program execution with a status code.
 *
 * @param status the status code
 */
void system_exit(int status);

#endif
