#ifndef LIBCDSA_LOGGING_H
#define LIBCDSA_LOGGING_H

/**
 * @file logging.h
 * @brief Simple logging utility.
 */

/**
 * @brief Logging severity levels.
 */
typedef enum {
    LOG_ALL,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_OFF
} LogLevel;

/**
 * @brief Logger output format options.
 */
typedef struct {
    bool timestamp;
    bool level;
    bool module;
    bool location;
    bool colors;
    bool padding;
} LoggerOptions;

/**
 * @brief Name of the current logging module.
 *
 * Defaults to an empty string if no module name is specified, in which case
 * "default" is used as the module name.
 *
 * Set to "disabled" to disable logging for the current module.
 */
#ifndef LOG_MODULE
#define LOG_MODULE ""
#endif

/**
 * @brief Initializes the logger.
 *
 * @param file_name the name of the file to write log messages to, or nullptr to use the `stdout`
 *
 * @return true on successful initialization, false otherwise
 */
bool logger_initialize(const char* file_name);

/**
 * @brief Finalizes the logger and releases its resources.
 */
void logger_finalize(void);

/**
 * @brief Returns the current logging severity level.
 *
 * @return the current logging severity level
 */
LogLevel logger_level(void);

/**
 * @brief Changes the current logging severity level.
 *
 * @param level the new logging severity level
 */
void logger_change_level(LogLevel level);

/**
 * @brief Pointer to the logger's formatting options.
 *
 * Modify its members to customize log message formatting.
 * All options are enabled by default, except `colors` and `padding`,
 * which are disabled by default when logging to a file.
 */
extern LoggerOptions* const logger_options;

/**
 * @brief Logs a trace-level message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_trace(message, ...) \
    logger_log(LOG_TRACE, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs a debug-level message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_debug(message, ...) \
    logger_log(LOG_DEBUG, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs an informational message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_info(message, ...) \
    logger_log(LOG_INFO, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs a warning message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_warn(message, ...) \
    logger_log(LOG_WARN, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs an error message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_error(message, ...) \
    logger_log(LOG_ERROR, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs a fatal message.
 *
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 */
#define logger_fatal(message, ...) \
    logger_log(LOG_FATAL, LOG_MODULE, __FILE__, __LINE__, __func__, message __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Logs a message at the specified severity level.
 *
 * @param level the logging severity level
 * @param module the module name
 * @param file the source file
 * @param line the source line
 * @param function the source function
 * @param message the message format string
 * @param ... optional format arguments
 *
 * @return true if the log was successfully written, false otherwise
 *
 * @note This function is intended for internal use. Use the appropriate `logger_` function instead.
 */
bool logger_log(LogLevel level, const char* module, const char* file, int line, const char* function, const char* message, ...);

/**
 * @brief Flush buffered log output to the output stream.
 *
 * @return true on success, false otherwise
 *
 * @note Buffered log output is automatically flushed by `logger_error()` and `logger_fatal()` by default.
 */
bool logger_flush();

#endif
