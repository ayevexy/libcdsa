#include "string.h"

#include "errors.h"
#include "constraints.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void* (*strings_memory_alloc)(size_t) = malloc;

void (*strings_memory_dealloc)(void*) = free;

constexpr int NULL_TERMINATOR = 1;

StringOwned string_new(const char* raw_string) {
    if (require_non_null(raw_string)) return string_null();

    const int length = strlen(raw_string);
    char* data = strings_memory_alloc(length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    memcpy(data, raw_string, length + NULL_TERMINATOR);
    return (StringOwned) { .data = data, .length = length };
}

void string_destroy(StringOwned* string) {
    if (require_non_null(string)) return;

    strings_memory_dealloc((char*) string->data);
    string->data = nullptr;
}

StringView string_view(const char* raw_string) {
    if (require_non_null(raw_string)) return string_null();

    return (String) {
        .data = raw_string,
        .length = strlen(raw_string)
    };
}

StringOwned string_format(const char* format, ...) {
    if (require_non_null(format)) return string_null();

    va_list parameters = {};
    va_start(parameters, format);
    const int length = vsnprintf(nullptr, 0, format, parameters);
    va_end(parameters);

    char* data = strings_memory_alloc(length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    vsnprintf(data, length + NULL_TERMINATOR, format, parameters);

    return (StringOwned) { .data = data, .length = length };
}

char string_char_at(String string, int index) {
    if (index < 0 || index >= string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, string.length);
        return '\0';
    }
    return string.data[index];
}

bool string_is_empty(String string) {
    return string.length == 0;
}

bool string_is_blank(String string) {
    for (int i = 0; i < string.length; i++) {
        if (!isspace((unsigned char) string.data[i]))
            return false;
    }
    return true;
}