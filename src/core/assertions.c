#include "assertions.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void assertion_failed(const char* source, int line, const char* location, const char* message, ...) {
    for (const char* path = source; *path; path++) {
        if (*path == '/' || *path == '\\') {
            source = path + 1;
        }
    }

    fprintf(stderr, "ASSERTION_ERROR: ");

    va_list parameters = {};
    va_start(parameters, message);

    vfprintf(stderr, message, parameters);
    va_end(parameters);

    fprintf(stderr, "\n\tat: %s:%d %s()\n", source, line, location);

    fflush(stderr);
    abort();
}