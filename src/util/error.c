#include "error.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 256

_Thread_local static Error global_error = NO_ERROR;
_Thread_local static char global_error_message[LENGTH] = "";
_Thread_local static int error_scope = 0;
_Thread_local static bool abort_on_error = true;

const char* error_message(void) {
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
    assert(error_scope > 0);
    error_scope--;
    abort_on_error = true;
    return error;
}

void raise_error(Error error, const char *error_message_format, ...) {
    _Thread_local static char global_error_message_copy[LENGTH];
    global_error = error;

    va_list parameters = {};
    va_start(parameters, error_message_format);

    const int length = vsnprintf(global_error_message_copy,LENGTH, error_message_format, parameters);
    va_end(parameters);

    assert(length >= 0 && length < LENGTH);

    snprintf(global_error_message_copy + length, LENGTH - length,"\n");
    strcpy(global_error_message, global_error_message_copy);

    if (global_error && abort_on_error && error_scope == 0) {
        fprintf(stderr, "%s", global_error_message);
        exit(EXIT_FAILURE);
    }
}