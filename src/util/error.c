#include "error.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LENGTH 256

_Thread_local static Error global_error = NO_ERROR;

_Thread_local static char global_error_message[LENGTH] = "\0";

const char* error_message(void) {
    return global_error_message;
}

Error get_error(void) {
    const Error error = global_error;
    global_error = NO_ERROR;
    return error;
}

void set_error(Error error, const char *error_message_format, ...) {
    _Thread_local static char global_error_message_copy[LENGTH];
    global_error = error;

    va_list parameters = {};
    va_start(parameters, error_message_format);

    const int length = vsnprintf(global_error_message_copy,LENGTH, error_message_format, parameters);
    va_end(parameters);

    if (length < 0 || length >= LENGTH) {
        return; // formatting error or truncated
    }
    snprintf(global_error_message_copy + length, LENGTH - length,"\n");
    strcpy(global_error_message, global_error_message_copy);
}