#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define error_category_shift 8
#define error_category_mask 0xFF00
#define error_value_mask 0x00FF

constexpr int MAX_MESSAGE_LENGTH = 256;

typedef struct {
    Error error;
    char message[MAX_MESSAGE_LENGTH];
    int scope;
    bool abort;
} ErrorContext;

thread_local static ErrorContext error_context = { .abort = true };

static const char* runtime_error_to_string(uint8 error) {
    static const char* error_strings[] = {
        "RUNTIME_ERROR",
        "NULL_POINTER_ERROR",
        "ARITHMETIC_ERROR",
        "INDEX_OUT_OF_BOUNDS_ERROR",
        "NO_SUCH_ELEMENT_ERROR",
        "ILLEGAL_ARGUMENT_ERROR",
        "ILLEGAL_STATE_ERROR",
        "UNSUPPORTED_OPERATION_ERROR",
        "CONCURRENT_MODIFICATION_ERROR",
        "MEMORY_ALLOCATION_ERROR"
    };
    return error < sizeof(error_strings) / sizeof(error_strings[0])
        ? error_strings[error]
        : "UNKNOWN_ERROR";
}

extern const char* file_system_error_to_string(uint8);

extern const char* process_error_to_string(uint8);

extern const char* thread_error_to_string(uint8);

extern const char* sequence_error_to_string(uint8);

const char* error_to_string(Error error) {
    if (error == NO_ERROR) {
        return "NO_ERROR";
    }
    const uint8 category = (error & error_category_mask) >> error_category_shift;
    const uint8 value = error & error_value_mask;
    switch (category) {
        case RUNTIME_ERROR_CATEGORY:      return runtime_error_to_string(value);
        case FILE_SYSTEM_ERROR_CATEGORY:  return file_system_error_to_string(value);
        case PROCESS_ERROR_CATEGORY:      return process_error_to_string(value);
        case THREAD_ERROR_CATEGORY:       return thread_error_to_string(value);
        case SEQUENCE_ERROR_CATEGORY:     return sequence_error_to_string(value);
        default:                          return "UNKNOWN_ERROR";
    }
}

const char* error_message(void) {
    return error_context.message;
}

const char* plain_error_message(void) {
    for (int i = 0; error_context.message[i] != '\0'; i++) {
        if (error_context.message[i] == ':') {
            return error_context.message + i + 2;
        }
    }
    return error_context.message;
}

void isolate_error(void) {
    error_context.error = NO_ERROR;
    error_context.message[0] = '\0';

    error_context.scope++;
    error_context.abort = false;
}

Error capture_error(void) {
    const Error error = error_context.error;
    error_context.error = NO_ERROR;

    assert(error_context.scope > 0 && "error_context.scope can't be negative");
    error_context.scope--;
    error_context.abort = true;

    return error;
}

void set_plain_error(Error error, const char* error_message_format, ...) {
    assert(error != NO_ERROR && "can't raise NO_ERROR");

    thread_local static char message_copy[MAX_MESSAGE_LENGTH];
    error_context.error = error;

    va_list parameters = {};
    va_start(parameters, error_message_format);

    const int length = vsnprintf(message_copy, MAX_MESSAGE_LENGTH, error_message_format, parameters);
    va_end(parameters);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");
    strcpy(error_context.message, message_copy);

    if (error_context.error && error_context.abort && error_context.scope == 0) {
        fprintf(stderr, "%s\n", error_context.message);
        exit(EXIT_FAILURE);
    }
}