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
    if (require_non_null(string, string->data)) return;

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
    if (require_non_null(string.data)) return '\0';
    if (index < 0 || index >= string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, string.length);
        return '\0';
    }
    return string.data[index];
}

bool string_is_empty(String string) {
    if (require_non_null(string.data)) return false;
    return string.length == 0;
}

bool string_is_blank(String string) {
    if (require_non_null(string.data)) return false;
    for (int i = 0; i < string.length; i++) {
        if (!isspace((unsigned char) string.data[i]))
            return false;
    }
    return true;
}

int string_compare(String string, String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncmp(string.data, other_string.data, string.length);
}

int string_compare_ignore_case(String string, String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncasecmp(string.data, other_string.data, string.length);
}

bool string_equals(String string, String other_string) {
    return string_compare(string, other_string) == 0;
}

bool string_equals_ignore_case(String string, String other_string) {
    return string_compare_ignore_case(string, other_string) == 0;
}

int string_index_of_char(String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int string_last_index_of_char(String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = string.length - 1; i >= 0; i--) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int string_index_of_substring(String string, String substring) {
    if (require_non_null(string.data, substring.data)) return 0;

    if (substring.length == 0) return 0;
    if (substring.length > string.length) return -1;

    for (int i = 0; i <= string.length - substring.length; i++) {
        if (memcmp(string.data + i, substring.data, (size_t) substring.length) == 0) {
            return i;
        }
    }
    return -1;
}

int string_last_index_of_substring(String string, String substring) {
    if (require_non_null(string.data, substring.data)) return 0;

    if (substring.length == 0) return 0;
    if (substring.length > string.length) return -1;

    for (int i = string.length - substring.length; i >= 0; i--) {
        if (memcmp(string.data + i, substring.data, (size_t) substring.length) == 0) {
            return i;
        }
    }
    return -1;
}

bool string_contains(String string, String substring) {
    return string_index_of_substring(string, substring) >= 0;
}

bool string_starts_with(String string, String prefix) {
    if (require_non_null(string.data, prefix.data)) return false;

    return string.length >= prefix.length && memcmp(string.data, prefix.data, (size_t) prefix.length) == 0;
}

bool string_ends_with(String string, String suffix) {
    if (require_non_null(string.data, suffix.data)) return false;

    return string.length >= suffix.length
        && memcmp(string.data + string.length - suffix.length, suffix.data, (size_t) suffix.length) == 0;
}

StringView string_trim(String string) {
    return string_trim_start(string_trim_end(string));
}

StringView string_trim_start(String string) {
    if (require_non_null(string.data)) return string_null();

    int start = 0;
    while (start < string.length && isspace(string.data[start])) {
        start++;
    }
    return (StringView) { .data = string.data + start, .length = string.length - start };
}

StringView string_trim_end(String string) {
    if (require_non_null(string.data)) return string_null();

    int end = string.length;
    while (end > 0 && isspace(string.data[end - 1])) {
        end--;
    }
    return (StringView) { .data = string.data, .length = end };

}

StringView string_substring(String string, int start, int length) {
    if (require_non_null(string.data)) return string_null();

    if (start < 0 || length < 0 || start > string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, length = %d", start, length);
        return string_null();
    }
    const int max_length = string.length - start;
    const int new_length = length > max_length ? max_length : length;

    return (StringView) { .data = string.data + start, .length = new_length };
}

StringView string_clone(String string) {
    if (require_non_null(string.data)) return string_null();

    return (StringView) { .data = string.data, .length = string.length };
}

StringOwned string_concat(String string, String other_string) {
    if (require_non_null(string.data, other_string.data)) return string_null();

    const int length = string.length + other_string.length;

    char* data = strings_memory_alloc(length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    memcpy(data, string.data, string.length);
    memcpy(data + string.length, other_string.data, other_string.length + NULL_TERMINATOR);
    return (StringOwned) { .data = data, .length = length };
}

StringOwned string_replace_char(String string, char character, char replacement) {
    if (require_non_null(string.data)) return string_null();

    char* data = strings_memory_alloc(string.length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    for (int i = 0; i < string.length + NULL_TERMINATOR; i++) {
        if (string.data[i] == character) {
            data[i] = replacement;
        } else {
            data[i] = string.data[i];
        }
    }
    return (StringOwned) { .data = data, .length = string.length };
}

StringOwned string_replace_substring(String string, String target, String replacement) {
    if (require_non_null(string.data, target.data, replacement.data)) return string_null();
    if (target.length == 0) return string_null();

    int count = 0;
    for (int i = 0; i <= string.length - target.length; i++) {
        if (memcmp(string.data + i, target.data, target.length) == 0) {
            count++;
            i += target.length - 1;
        }
    }
    const int new_length = string.length + count * (replacement.length - target.length);

    char* data = strings_memory_alloc(new_length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    int input_index = 0;
    int output_index = 0;

    while (input_index < string.length) {
        if (input_index <= string.length - target.length && memcmp(string.data + input_index, target.data, target.length) == 0) {
            memcpy(data + output_index, replacement.data, replacement.length);
            output_index += replacement.length;
            input_index += target.length;
        } else {
            data[output_index++] = string.data[input_index++];
        }
    }
    data[output_index] = '\0';
    return (StringOwned) { .data = data, .length = new_length };
}

StringOwned string_repeat(String string, int times) {
    if (require_non_null(string.data)) return string_null();

    if (times < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'times' parameter can't be negative");
        return string_null();
    }
    const int length = string.length * times;

    char* data = strings_memory_alloc(length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    for (int i = 0, offset = 0; i < times; i++) {
        memcpy(data + offset, string.data, string.length);
        offset += string.length;
    }
    data[length] = '\0';
    return (StringOwned) { .data = data, .length = length };
}

StringOwned string_join(String separator, ...) {
    if (require_non_null(separator.data)) return string_null();

    va_list parameters = {};
    int total_length = 0;

    va_start(parameters, separator);
    while (true) {
        const String string = va_arg(parameters, String);
        if (!string.data) break;
        total_length += string.length + separator.length;
    }
    total_length -= separator.length;
    va_end(parameters);

    char* data = strings_memory_alloc(total_length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }

    va_start(parameters, separator);
    int offset = 0;
    while (true) {
        const String string = va_arg(parameters, String);
        if (!string.data) break;

        memcpy(data + offset, string.data, string.length);

        if (offset + string.length + separator.length != total_length) {
            memcpy(data + offset + string.length, separator.data, separator.length);
        }
        offset += string.length + separator.length;
    }
    va_end(parameters);

    data[total_length] = '\0';
    return (StringOwned) { .data = data, .length = total_length };
}

StringView* string_split(String string, char delimiter) {
    if (require_non_null(string.data)) return nullptr;
    int count = 1;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == delimiter || i == string.length - 1) {
            count++;
        }
    }
    StringView* strings = strings_memory_alloc(count * sizeof(StringView));
    if (!strings) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'strings'");
        return nullptr;
    }
    int index = 0;
    for (int i = 0, start = 0; i <= string.length; i++) {
        if (string.data[i] == delimiter || i == string.length) {
            strings[index++] = (StringView) {
                .data = string.data + start,
                .length = i - start
            };
            start = i + 1;
        }
    }
    strings[index] = string_null();
    return strings;
}

StringView* string_lines(String string) {
    return string_split(string, '\n');
}

StringOwned string_to_uppercase(String string) {
    if (require_non_null(string.data)) return string_null();

    char* data = strings_memory_alloc(string.length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    for (int i = 0; i < string.length + NULL_TERMINATOR; i++) {
        data[i] = toupper(string.data[i]);
    }
    return (StringOwned) { .data = data, .length = string.length };
}

StringOwned string_to_lowercase(String string) {
    if (require_non_null(string.data)) return string_null();

    char* data = strings_memory_alloc(string.length + NULL_TERMINATOR);
    if (!data) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return string_null();
    }
    for (int i = 0; i < string.length + NULL_TERMINATOR; i++) {
        data[i] = tolower(string.data[i]);
    }
    return (StringOwned) { .data = data, .length = string.length };
}