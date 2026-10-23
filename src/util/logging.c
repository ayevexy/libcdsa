#include "logging.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#ifdef __linux__
#include <pthread.h>
#endif

#define TEXT_COLOR    "\033[90m"       // Bright Black
#define TRACE_COLOR   "\033[1;90m"     // Bold Bright Black
#define DEBUG_COLOR   "\033[1;36m"     // Bold Cyan
#define INFO_COLOR    "\033[1;32m"     // Bold Green
#define WARN_COLOR    "\033[1;33m"     // Bold Yellow
#define ERROR_COLOR   "\033[1;31m"     // Bold Red
#define FATAL_COLOR   "\033[1;37;41m"  // Bold White on Red Background
#define SOURCE_COLOR  "\033[1;34m"     // Bold Blue
#define END_COLOR     "\033[0m"        // Reset

static constexpr int BUFFER_SIZE = 512;

typedef struct {
    bool initialized;
    char buffer[BUFFER_SIZE];
    int offset;
#ifdef __linux__
    pthread_mutex_t mutex;
#endif
    FILE* file;
    LogLevel level;
    LoggerOptions options;
} Logger;

static Logger logger = {
    .initialized = false,
    .buffer = "",
    .offset = 0,
    .file = nullptr,
    .level = LOG_ALL,
    .options = {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true,
        .colors = true,
        .padding = true
    }
};

bool logger_initialize(const char* file_name) {
    if (logger.initialized) {
        return false;
    }
    if (file_name) {
        logger.file = fopen(file_name, "a");
        if (!logger.file) {
            return false;
        }
        logger.options.colors = false;
        logger.options.padding = false;
    } else {
        logger.file = stdout;
    }
#ifdef __linux__
    pthread_mutex_init(&logger.mutex, nullptr);
#endif
    logger.initialized = true;
    return true;
}

void logger_finalize(void) {
    if (!logger.initialized) {
        return;
    }
    if (logger.file && logger.file != stdout) {
        fclose(logger.file);
        logger.file = nullptr;
    }
#ifdef __linux__
    pthread_mutex_destroy(&logger.mutex);
#endif
    logger.initialized = false;
}

LogLevel logger_level(void) {
    return logger.level;
}

void logger_change_level(LogLevel level) {
    logger.level = level;
}

LoggerOptions* const logger_options = &logger.options;

static bool logger_format_timestamp() {
    if (!logger.options.timestamp) {
        return true;
    }

    const time_t now = time(nullptr);
    struct tm* time_info = localtime(&now);

    if (!time_info) {
        logger.offset += snprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset,
            "%s[YYYY-MM-DD HH:MM:SS]%s ",
            logger.options.colors ? TEXT_COLOR : "",
            logger.options.colors ? END_COLOR : ""
        );
    } else {
        logger.offset += snprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset,
            "%s[%04d-%02d-%02d %02d:%02d:%02d]%s ",
            logger.options.colors ? TEXT_COLOR : "",
            time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec,
            logger.options.colors ? END_COLOR : ""
        );
    }

    return logger.offset >= 0 && logger.offset < BUFFER_SIZE;
}

static bool logger_format_level(LogLevel level) {
    static const char* log_level_names[] = {
        [LOG_TRACE] = "TRACE",
        [LOG_DEBUG] = "DEBUG",
        [LOG_INFO]  = "INFO",
        [LOG_WARN]  = "WARN",
        [LOG_ERROR] = "ERROR",
        [LOG_FATAL] = "FATAL"
    };

    static const char* log_level_colors[] = {
        [LOG_TRACE] = TRACE_COLOR,
        [LOG_DEBUG] = DEBUG_COLOR,
        [LOG_INFO]  = INFO_COLOR,
        [LOG_WARN]  = WARN_COLOR,
        [LOG_ERROR] = ERROR_COLOR,
        [LOG_FATAL] = FATAL_COLOR
    };

    if (!logger.options.level) {
        return true;
    }

    logger.offset += snprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset,
        "%s[%-*s]%s ",
        logger.options.colors ? log_level_colors[level] : "",
        logger.options.padding ? 5 : 0,
        log_level_names[level],
        logger.options.colors ? END_COLOR : ""
    );

    return logger.offset >= 0 && logger.offset < BUFFER_SIZE;
}

static bool logger_format_module(const char* module_name) {
    if (!logger.options.module) {
        return true;
    }

    logger.offset += snprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset,
        "%s[%*s]%s ",
        logger.options.colors ? TEXT_COLOR : "",
        logger.options.padding ? 20 : 0,
        module_name[0] != '\0' ? module_name : "default",
        logger.options.colors ? END_COLOR : ""
    );

    return logger.offset >= 0 && logger.offset < BUFFER_SIZE;
}

static bool logger_format_location(const char* file, int line, const char* function) {
    if (!logger.options.location) {
        return true;
    }

    constexpr int LOCATION_BUFFER_SIZE = 128;
    static char location_buffer[LOCATION_BUFFER_SIZE];

    for (const char* path = file; *path; path++) {
        if (*path == '/' || *path == '\\') {
            file = path + 1;
        }
    }

    const int count = snprintf(location_buffer, LOCATION_BUFFER_SIZE,
        "%s%s:%d%s %s%s()%s",
        logger.options.colors ? SOURCE_COLOR : "",
        file, line,
        logger.options.colors ? END_COLOR : "",
        logger.options.colors ? TEXT_COLOR : "",
        function,
        logger.options.colors ? END_COLOR : ""
    );

    if (count <= 0 || count >= LOCATION_BUFFER_SIZE) {
        return false;
    }

    logger.offset += snprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset,
        TEXT_COLOR "[" END_COLOR
        "%-*s"
        TEXT_COLOR "] " END_COLOR,
        logger.options.padding ? (logger.options.colors ? 64 : 44) : 0, location_buffer
    );

    return logger.offset >= 0 && logger.offset < BUFFER_SIZE;
}

static bool logger_format_message(const char* message, va_list parameters) {
    logger.offset += vsnprintf(logger.buffer + logger.offset, BUFFER_SIZE - logger.offset, message, parameters);

    return logger.offset >= 0 && logger.offset < BUFFER_SIZE;
}

static bool logger_log_internal(LogLevel level, const char* module, const char* file, int line, const char* function, const char* message, va_list parameters) {
    if (level < LOG_TRACE || level >= LOG_OFF || logger.level == LOG_OFF || level < logger.level) {
        return false;
    }

    if (strcmp(module, "disabled") == 0) {
        return false;
    }

    logger.buffer[0] = '\0';
    logger.offset = 0;

    return logger_format_timestamp()
        && logger_format_level(level)
        && logger_format_module(module)
        && logger_format_location(file, line, function)
        && logger_format_message(message, parameters)
        && fprintf(logger.file, "%s\n", logger.buffer) > 0
        && level >= LOG_ERROR ? fflush(logger.file) == 0 : true;
}

bool logger_log(LogLevel level, const char* module, const char* file, int line, const char* function, const char* message, ...) {
#ifdef __linux__
    pthread_mutex_lock(&logger.mutex);
#endif
    va_list parameters = {};
    va_start(parameters, message);

    const bool written = logger_log_internal(level, module, file, line, function, message, parameters);
    va_end(parameters);
#ifdef __linux__
    pthread_mutex_unlock(&logger.mutex);
#endif
    return written;
}

bool logger_flush() {
    return fflush(logger.file) == 0;
}