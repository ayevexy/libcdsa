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

extern const char* synchronization_error_to_string(uint8);

bool error_has_category(Error error, ErrorCategory category) {
    if (error == NO_ERROR) {
        return false;
    }
    return ((error & error_category_mask) >> error_category_shift) == category;
}

ErrorCategory error_category(Error error) {
    assert(error != NO_ERROR && "can't retrieve error category of NO_ERROR");

    return (error & error_category_mask) >> error_category_shift;
}

uint8 error_value(Error error) {
    assert(error != NO_ERROR && "can't retrieve error value of NO_ERROR");

    return error & error_value_mask;
}

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

const char* error_description(void) {
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

void (set_error)(Error error, const char* message, ...) {
    assert(error != NO_ERROR && "can't raise NO_ERROR");

    thread_local static char message_copy[MAX_MESSAGE_LENGTH];
    error_context.error = error;

    int length = snprintf(message_copy, MAX_MESSAGE_LENGTH, "%s: ", error_to_string(error));

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");

    va_list parameters = {};
    va_start(parameters, message);

    length += vsnprintf(message_copy + length, MAX_MESSAGE_LENGTH, message[0] != '\0' ? message : "no additional details available", parameters);
    va_end(parameters);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");
    strcpy(error_context.message, message_copy);

    if (error_context.error && error_context.abort && error_context.scope == 0) {
        fprintf(stderr, "%s\n", error_context.message);
        exit(EXIT_FAILURE);
    }
}