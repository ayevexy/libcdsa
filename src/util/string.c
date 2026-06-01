#include "string.h"

#include "errors.h"
#include "constraints.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void* (*string_memory_alloc)(size_t) = malloc;

void (*string_memory_dealloc)(void*) = free;

constexpr int NULL_TERMINATOR = 1;

uint64_t c_string_hash(const void* raw_string) {
    String string = (String) raw_string;
    uint64_t hash = 5381;
    for (int c = *string->data; c != '\0'; c = *++string->data) {
        hash = hash * 33 + c;
    }
    return hash;
}

bool c_string_equals(const void* string, const void* other_string) {
    return _string_equals(*((String) string), *((String) other_string));
}

int c_string_compare(const void* string, const void* other_string) {
    return _string_compare(*((String) string), *((String) other_string));
}

void c_string_destroy(void* string) {
    string_memory_dealloc(string);
}

int c_string_to_string(const void* string, char* buffer, size_t size) {
    return snprintf(buffer, size, "%s", ((String) string)->data);
}

String _string_new(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    String new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->_data;
    memcpy(new_string->data, string.data, string.length);
    new_string->data[new_string->length] = '\0';
    return new_string;
}

void _string_destroy(String* string_pointer) {
    if (require_non_null(string_pointer, *string_pointer)) return;
    String string = *string_pointer;
    string_memory_dealloc(string);
    *string_pointer = nullptr;
}

String _string_format(struct String format, ...) {
    if (require_non_null(format.data)) return nullptr;

    va_list parameters = {};
    va_start(parameters, format);
    const int length = vsnprintf(nullptr, 0, format.data, parameters);
    va_end(parameters);

    String string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    string->length = length;
    string->data = string->_data;
    va_start(parameters, format);
    vsnprintf(string->data, length + NULL_TERMINATOR, format.data, parameters);
    va_end(parameters);

    return string;
}

char _string_char_at(struct String string, int index) {
    if (require_non_null(string.data)) return '\0';
    if (index < 0 || index >= string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, string.length);
        return '\0';
    }
    return string.data[index];
}

int _string_length(struct String string) {
    return string.length;
}

const char* _string_data(struct String string) {
    return string.data;
}

bool _string_is_empty(struct String string) {
    if (require_non_null(string.data)) return false;
    return string.length == 0;
}

bool _string_is_blank(struct String string) {
    if (require_non_null(string.data)) return false;
    for (int i = 0; i < string.length; i++) {
        if (!isspace((unsigned char) string.data[i]))
            return false;
    }
    return true;
}

int _string_compare(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncmp(string.data, other_string.data, string.length);
}

int _string_compare_ignore_case(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncasecmp(string.data, other_string.data, string.length);
}

bool _string_equals(struct String string, struct String other_string) {
    return _string_compare(string, other_string) == 0;
}

bool _string_equals_ignore_case(struct String string, struct String other_string) {
    return _string_compare_ignore_case(string, other_string) == 0;
}

int _string_index_of_char(struct String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int _string_last_index_of_char(struct String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = string.length - 1; i >= 0; i--) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int _string_index_of_substring(struct String string, struct String substring) {
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

int _string_last_index_of_substring(struct String string, struct String substring) {
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

bool _string_contains(struct String string, struct String substring) {
    return _string_index_of_substring(string, substring) >= 0;
}

bool _string_starts_with(struct String string, struct String prefix) {
    if (require_non_null(string.data, prefix.data)) return false;

    return string.length >= prefix.length && memcmp(string.data, prefix.data, (size_t) prefix.length) == 0;
}

bool _string_ends_with(struct String string, struct String suffix) {
    if (require_non_null(string.data, suffix.data)) return false;

    return string.length >= suffix.length
        && memcmp(string.data + string.length - suffix.length, suffix.data, (size_t) suffix.length) == 0;
}

String _string_trim(struct String string) {
    String half = _string_trim_end(string);
    String full = _string_trim_start(_string_ref(half));
    string_memory_dealloc(half);
    return full;
}

String _string_trim_start(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    int start = 0;
    while (start < string.length && isspace(string.data[start])) {
        start++;
    }
    const int new_length = string.length - start;

    String new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->_data;
    memcpy(new_string->data, string.data + start, string.length - start);
    new_string->data[new_length] = '\0';
    return new_string;
}

String _string_trim_end(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    int end = string.length;
    while (end > 0 && isspace(string.data[end - 1])) {
        end--;
    }
    const int new_length = end;

    String new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->_data;
    memcpy(new_string->data, string.data, end);
    new_string->data[new_length] = '\0';
    return new_string;
}

String _string_substring(struct String string, int start, int length) {
    if (require_non_null(string.data)) return nullptr;

    if (start < 0 || length < 0 || start > string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, length = %d", start, length);
        return nullptr;
    }
    const int max_length = string.length - start;
    const int new_length = length > max_length ? max_length : length;

    String new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->_data;
    memcpy(new_string->data, string.data + start, new_length);
    new_string->data[new_length] = '\0';
    return new_string;
}

String _string_concat(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return nullptr;

    const int length = string.length + other_string.length;

    String new_string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = length;
    new_string->data = new_string->_data;
    memcpy(new_string->data, string.data, string.length);
    memcpy(new_string->data + string.length, other_string.data, other_string.length);
    new_string->data[length] = '\0';
    return new_string;
}

String _string_replace_char(struct String string, char character, char replacement) {
    if (require_non_null(string.data)) return nullptr;

    String new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->_data;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) {
            new_string->data[i] = replacement;
        } else {
            new_string->data[i] = string.data[i];
        }
    }
    new_string->data[string.length] = '\0';
    return new_string;
}

String _string_replace_substring(struct String string, struct String target, struct String replacement) {
    if (require_non_null(string.data, target.data, replacement.data)) return nullptr;
    if (target.length == 0) return nullptr;

    int count = 0;
    for (int i = 0; i <= string.length - target.length; i++) {
        if (memcmp(string.data + i, target.data, target.length) == 0) {
            count++;
            i += target.length - 1;
        }
    }
    const int new_length = string.length + count * (replacement.length - target.length);
    String new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->_data;

    int input_index = 0;
    int output_index = 0;

    while (input_index < string.length) {
        if (input_index <= string.length - target.length && memcmp(string.data + input_index, target.data, target.length) == 0) {
            memcpy(new_string->data + output_index, replacement.data, replacement.length);
            output_index += replacement.length;
            input_index += target.length;
        } else {
            new_string->data[output_index++] = string.data[input_index++];
        }
    }
    new_string->data[output_index] = '\0';
    return new_string;
}

String _string_repeat(struct String string, int times) {
    if (require_non_null(string.data)) return nullptr;

    if (times < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'times' parameter can't be negative");
        return nullptr;
    }
    const int length = string.length * times;

    String new_string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = length;
    new_string->data = new_string->_data;

    for (int i = 0, offset = 0; i < times; i++) {
        memcpy(new_string->data + offset, string.data, string.length);
        offset += string.length;
    }
    new_string->data[length] = '\0';
    return new_string;
}

String _string_join(struct String separator, ...) {
    if (require_non_null(separator.data)) return nullptr;

    va_list parameters = {};
    int total_length = 0;

    va_start(parameters, separator);
    while (true) {
        const struct String string = va_arg(parameters, struct String);
        if (!string.data) break;
        total_length += string.length + separator.length;
    }
    total_length -= separator.length;
    va_end(parameters);

    String new_string = string_memory_alloc(sizeof(struct String) + total_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = total_length;
    new_string->data = new_string->_data;

    va_start(parameters, separator);
    int offset = 0;
    while (true) {
        const struct String string = va_arg(parameters, struct String);
        if (!string.data) break;

        memcpy(new_string->data + offset, string.data, string.length);

        if (offset + string.length + separator.length != total_length) {
            memcpy(new_string->data + offset + string.length, separator.data, separator.length);
        }
        offset += string.length + separator.length;
    }
    va_end(parameters);

    new_string->data[total_length] = '\0';
    return new_string;
}

Array(String) _string_split(struct String string, char delimiter) {
    if (require_non_null(string.data)) return nullptr;
    int count = 1;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == delimiter) {
            count++;
        }
    }
    Array(String) strings = _array_new(count, sizeof(struct String), nullptr);
    if (!strings) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'strings'");
        return nullptr;
    }
    int index = 0;
    for (int i = 0, start = 0; i <= string.length; i++) {
        if (string.data[i] == delimiter || i == string.length) {
            String new_string = string_memory_alloc(sizeof(struct String) + i - start + NULL_TERMINATOR);
            if (!new_string) {
                set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
                return nullptr;
            }
            new_string->data = new_string->_data;
            memcpy(new_string->data, string.data + start, i - start);
            new_string->length = i - start;
            new_string->data[new_string->length] = '\0';
            strings[index++] = new_string;
            start = i + 1;
        }
    }
    return strings;
}

Array(String) _string_lines(struct String string) {
    return _string_split(string, '\n');
}

String _string_to_uppercase(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    String new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->_data;

    for (int i = 0; i < string.length; i++) {
        new_string->data[i] = toupper(string.data[i]);
    }
    new_string->data[new_string->length] = '\0';
    return new_string;
}

String _string_to_lowercase(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    String new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->_data;

    for (int i = 0; i < string.length; i++) {
        new_string->data[i] = tolower(string.data[i]);
    }
    new_string->data[new_string->length] = '\0';
    return new_string;
}

#define DEFINE_STRING_VALUE_OF(func_name, type, format)                             \
    String func_name(type value) {                                                  \
        const int length = snprintf(nullptr, 0, format, value) + NULL_TERMINATOR;   \
        char data[length];                                                          \
        snprintf(data, length, format, value);                                      \
        return string_new(data);                                                    \
    }

String _string_value_of_bool(bool value) {
    return string_new(value ? "true" : "false");
}

DEFINE_STRING_VALUE_OF(_string_value_of_char, char, "%c")

DEFINE_STRING_VALUE_OF(_string_value_of_int, int, "%d")

DEFINE_STRING_VALUE_OF(_string_value_of_uint, unsigned int, "%u")

DEFINE_STRING_VALUE_OF(_string_value_of_long, long, "%ld")

DEFINE_STRING_VALUE_OF(_string_value_of_ulong, unsigned long, "%lu")

DEFINE_STRING_VALUE_OF(_string_value_of_long_long, long long, "%lld")

DEFINE_STRING_VALUE_OF(_string_value_of_ulong_long, unsigned long long, "%llu")

DEFINE_STRING_VALUE_OF(_string_value_of_float, float, "%f")

DEFINE_STRING_VALUE_OF(_string_value_of_double, double, "%f")

DEFINE_STRING_VALUE_OF(_string_value_of_long_double, long double, "%Lf")

#undef DEFINE_STRING_VALUE_OF

struct String _string_ref_of_string(String string) {
    return string ? *string : (struct String) {};
}

struct String _string_ref_of_raw_string(const char* raw_string) {
    return (struct String) { strlen(raw_string), (char*) raw_string };
}

char _char_self(char c) {
    return c;
}

void _cleanup(Array(struct String*) strings) {
    for (int i = 0; i < (int) array_length(strings); i++) {
        string_destroy(&strings[i]);
    }
}