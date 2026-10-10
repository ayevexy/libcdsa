#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

constexpr int MAX_MESSAGE_LENGTH = 256;

typedef struct {
    Error error;
    char message[MAX_MESSAGE_LENGTH];
    int scope;
    bool abort;
} ErrorContext;

thread_local static ErrorContext error_context = { .abort = true };

const Error NULL_POINTER_ERROR            = ERROR("NULL_POINTER_ERROR", "a null pointer was provided");
const Error ARITHMETIC_ERROR              = ERROR("ARITHMETIC_ERROR", "an arithmetic operation could not be completed");
const Error NUMBER_FORMAT_ERROR           = ERROR("NUMBER_FORMAT_ERROR", "the string does not represent a valid number");
const Error INDEX_OUT_OF_BOUNDS_ERROR     = ERROR("INDEX_OUT_OF_BOUNDS_ERROR", "an index is outside the valid range");
const Error NO_SUCH_ELEMENT_ERROR         = ERROR("NO_SUCH_ELEMENT_ERROR", "the requested element does not exist");
const Error ILLEGAL_ARGUMENT_ERROR        = ERROR("ILLEGAL_ARGUMENT_ERROR", "an invalid argument was provided");
const Error ILLEGAL_STATE_ERROR           = ERROR("ILLEGAL_STATE_ERROR", "an operation was attempted in an invalid state");
const Error TYPE_MISMATCH_ERROR           = ERROR("TYPE_MISMATCH_ERROR", "the actual type does not match the expected type");
const Error UNSUPPORTED_OPERATION_ERROR   = ERROR("UNSUPPORTED_OPERATION_ERROR", "the requested operation is not supported");
const Error CONCURRENT_MODIFICATION_ERROR = ERROR("CONCURRENT_MODIFICATION_ERROR", "a collection was modified while it was being iterated");
const Error MEMORY_ALLOCATION_ERROR       = ERROR("MEMORY_ALLOCATION_ERROR", "a memory allocation operation failed");

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

void error_print(void) {
    const int location_offset = strlen(error_context.message) + 1;
    fprintf(stderr, "%s%s", error_context.message, error_context.message + location_offset);
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

void (set_error)(Error error, const char* location, const char* message, ...) {
    assert(error != NO_ERROR && "can't raise NO_ERROR");

    thread_local static char message_copy[MAX_MESSAGE_LENGTH];
    error_context.error = error;

    int length = snprintf(message_copy, MAX_MESSAGE_LENGTH, "%s: ", error->name);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");

    va_list parameters = {};
    va_start(parameters, message);

    message = message[0] != '\0' ? message : error->description ? error->description : "no additional details are available";

    length += vsnprintf(message_copy + length, MAX_MESSAGE_LENGTH, message, parameters);
    va_end(parameters);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");

    const int location_offset = length + 1;
    length += snprintf(message_copy + location_offset, MAX_MESSAGE_LENGTH, "\n\tat %s()\n", location);

    assert(length >= 0 && length < MAX_MESSAGE_LENGTH && "formatted string is too big");

    memcpy(error_context.message, message_copy, MAX_MESSAGE_LENGTH);

    if (error_context.error && error_context.abort && error_context.scope == 0) {
        fprintf(stderr, "%s%s", error_context.message, error_context.message + location_offset);
        exit(EXIT_FAILURE);
    }
}