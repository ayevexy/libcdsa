#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

constexpr int MAX_MESSAGE_LENGTH = 256;

thread_local static Error global_error = NO_ERROR;
thread_local static char global_error_message[MAX_MESSAGE_LENGTH] = "";
thread_local static int error_scope = 0;
thread_local static bool abort_on_error = true;

const char* error_to_string(Error error) {
    static const char* error_strings[] = {
        "NO_ERROR",
        "NULL_POINTER_ERROR",
        "INDEX_OUT_OF_BOUNDS_ERROR",
        "NO_SUCH_ELEMENT_ERROR",
        "ILLEGAL_ARGUMENT_ERROR",
        "ILLEGAL_STATE_ERROR",
        "UNSUPPORTED_OPERATION_ERROR",
        "CONCURRENT_MODIFICATION_ERROR",
        "MEMORY_ALLOCATION_ERROR"
    };
    return error > 0 && error < ERROR_COUNT ? error_strings[error] : "UNKNOWN_ERROR";
}

const char* error_message(void) {
    return global_error_message;
}

const char* plain_error_message(void) {
    for (int i = 0; global_error_message[i] != '\0'; i++) {
        if (global_error_message[i] == '-') {
            return global_error_message + i + 2;
        }
    }
    return global_error_message;
}

void isolate_error(void) {
    global_error = NO_ERROR;
    global_error_message[0] = '\0';

    error_scope++;
    abort_on_error = false;
}

Error capture_error(void) {
    const Error error = global_error;
    global_error = NO_ERROR;

    assert(error_scope > 0 && "error_scope can't be negative");
    error_scope--;
    abort_on_error = true;

    return error;
}

void set_plain_error(Error error, const char* error_message_format, ...) {
    assert(error != NO_ERROR && "can't raise NO_ERROR");

    _Thread_local static char global_error_message_copy[MAX_MESSAGE_LENGTH];
    global_error = error;

    va_list parameters = {};
    va_start(parameters, error_message_format);

    const int length = vsnprintf(global_error_message_copy,MAX_MESSAGE_LENGTH, error_message_format, parameters);
    va_end(parameters);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");
    strcpy(global_error_message, global_error_message_copy);

    if (global_error && abort_on_error && error_scope == 0) {
        fprintf(stderr, "%s\n", global_error_message);
        exit(EXIT_FAILURE);
    }
}