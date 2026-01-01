#include "error.h"

#include <stdio.h>
#include <stdarg.h>

#define LENGTH 256

_Thread_local Error global_error = NO_ERROR;

_Thread_local char global_error_message[LENGTH] = "\0";

void set_error(Error error_code, const char *format, ...) {
    global_error = error_code;

    va_list parameters = {};
    va_start(parameters, format);

    const int length = vsnprintf(global_error_message,LENGTH, format, parameters);
    va_end(parameters);

    if (length < 0 || length >= LENGTH) {
        return; // formatting error or truncated
    }
    snprintf(global_error_message + length, LENGTH - length,"\n");
}