#include "string.h"

#include "errors.h"
#include "memory.h"
#include "util/constraints.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

void* (*string_memory_alloc)(size_t) = memory_try_alloc;

void (*string_memory_dealloc)(void*) = memory_dealloc;

constexpr int NULL_TERMINATOR = 1;

String (string_new)(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->buffer;
    memcpy(new_string->buffer, string.data, string.length);
    new_string->buffer[new_string->length] = '\0';
    return new_string;
}

void (string_destroy)(String* string_pointer) {
    if (require_non_null(string_pointer, *string_pointer)) return;
    String string = *string_pointer;
    string_memory_dealloc((struct String*) string);
    *string_pointer = nullptr;
}

String (string_format)(struct String format, ...) {
    if (require_non_null(format.data)) return nullptr;

    va_list parameters = {};
    va_start(parameters, format);
    const int length = vsnprintf(nullptr, 0, format.data, parameters);
    va_end(parameters);

    struct String* string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    string->length = length;
    string->data = string->buffer;
    va_start(parameters, format);
    vsnprintf(string->buffer, length + NULL_TERMINATOR, format.data, parameters);
    va_end(parameters);

    return string;
}

char (string_char_at)(struct String string, int index) {
    if (require_non_null(string.data)) return '\0';
    if (index < 0 || index >= string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, string.length);
        return '\0';
    }
    return string.data[index];
}

int (string_length)(struct String string) {
    return string.length;
}

const char* (string_data)(struct String string) {
    return string.data;
}

bool (string_is_empty)(struct String string) {
    if (require_non_null(string.data)) return false;
    return string.length == 0;
}

bool (string_is_blank)(struct String string) {
    if (require_non_null(string.data)) return false;
    for (int i = 0; i < string.length; i++) {
        if (!isspace((unsigned char) string.data[i]))
            return false;
    }
    return true;
}

int (string_compare)(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncmp(string.data, other_string.data, string.length);
}

int (string_compare_ignore_case)(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncasecmp(string.data, other_string.data, string.length);
}

bool (string_equals)(struct String string, struct String other_string) {
    return (string_compare)(string, other_string) == 0;
}

bool (string_equals_ignore_case)(struct String string, struct String other_string) {
    return (string_compare_ignore_case)(string, other_string) == 0;
}

int (string_index_of_char)(struct String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int (string_last_index_of_char)(struct String string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = string.length - 1; i >= 0; i--) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int (string_index_of_substring)(struct String string, struct String substring) {
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

int (string_last_index_of_substring)(struct String string, struct String substring) {
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

bool (string_contains)(struct String string, struct String substring) {
    return string_index_of_substring(string, substring) >= 0;
}

bool (string_starts_with)(struct String string, struct String prefix) {
    if (require_non_null(string.data, prefix.data)) return false;

    return string.length >= prefix.length && memcmp(string.data, prefix.data, (size_t) prefix.length) == 0;
}

bool (string_ends_with)(struct String string, struct String suffix) {
    if (require_non_null(string.data, suffix.data)) return false;

    return string.length >= suffix.length
        && memcmp(string.data + string.length - suffix.length, suffix.data, (size_t) suffix.length) == 0;
}

String (string_trim)(struct String string) {
    String half = (string_trim_end)(string);
    String full = string_trim_start(half);
    string_memory_dealloc((struct String*) half);
    return full;
}

String (string_trim_start)(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    int start = 0;
    while (start < string.length && isspace(string.data[start])) {
        start++;
    }
    const int new_length = string.length - start;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->buffer;
    memcpy(new_string->buffer, string.data + start, string.length - start);
    new_string->buffer[new_length] = '\0';
    return new_string;
}

String (string_trim_end)(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    int end = string.length;
    while (end > 0 && isspace(string.data[end - 1])) {
        end--;
    }
    const int new_length = end;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->buffer;
    memcpy(new_string->buffer, string.data, end);
    new_string->buffer[new_length] = '\0';
    return new_string;
}

String (string_substring)(struct String string, int start, int length) {
    if (require_non_null(string.data)) return nullptr;

    if (start < 0 || length < 0 || start > string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, length = %d", start, length);
        return nullptr;
    }
    const int max_length = string.length - start;
    const int new_length = length > max_length ? max_length : length;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->buffer;
    memcpy(new_string->buffer, string.data + start, new_length);
    new_string->buffer[new_length] = '\0';
    return new_string;
}

String (string_concat)(struct String string, struct String other_string) {
    if (require_non_null(string.data, other_string.data)) return nullptr;

    const int length = string.length + other_string.length;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = length;
    new_string->data = new_string->buffer;
    memcpy(new_string->buffer, string.data, string.length);
    memcpy(new_string->buffer + string.length, other_string.data, other_string.length);
    new_string->buffer[length] = '\0';
    return new_string;
}

String (string_replace_char)(struct String string, char character, char replacement) {
    if (require_non_null(string.data)) return nullptr;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->buffer;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) {
            new_string->buffer[i] = replacement;
        } else {
            new_string->buffer[i] = string.data[i];
        }
    }
    new_string->buffer[string.length] = '\0';
    return new_string;
}

String (string_replace_substring)(struct String string, struct String target, struct String replacement) {
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
    struct String* new_string = string_memory_alloc(sizeof(struct String) + new_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = new_length;
    new_string->data = new_string->buffer;

    int input_index = 0;
    int output_index = 0;

    while (input_index < string.length) {
        if (input_index <= string.length - target.length && memcmp(string.data + input_index, target.data, target.length) == 0) {
            memcpy(new_string->buffer + output_index, replacement.data, replacement.length);
            output_index += replacement.length;
            input_index += target.length;
        } else {
            new_string->buffer[output_index++] = string.data[input_index++];
        }
    }
    new_string->buffer[output_index] = '\0';
    return new_string;
}

String (string_repeat)(struct String string, int times) {
    if (require_non_null(string.data)) return nullptr;

    if (times < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'times' parameter can't be negative");
        return nullptr;
    }
    const int length = string.length * times;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = length;
    new_string->data = new_string->buffer;

    for (int i = 0, offset = 0; i < times; i++) {
        memcpy(new_string->buffer + offset, string.data, string.length);
        offset += string.length;
    }
    new_string->buffer[length] = '\0';
    return new_string;
}

String (string_join)(struct String separator, ...) {
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

    struct String* new_string = string_memory_alloc(sizeof(struct String) + total_length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = total_length;
    new_string->data = new_string->buffer;

    va_start(parameters, separator);
    int offset = 0;
    while (true) {
        const struct String string = va_arg(parameters, struct String);
        if (!string.data) break;

        memcpy(new_string->buffer + offset, string.data, string.length);

        if (offset + string.length + separator.length != total_length) {
            memcpy(new_string->buffer + offset + string.length, separator.data, separator.length);
        }
        offset += string.length + separator.length;
    }
    va_end(parameters);

    new_string->buffer[total_length] = '\0';
    return new_string;
}

Array(String) (string_split)(struct String string, char delimiter) {
    if (require_non_null(string.data)) return nullptr;
    int count = 1;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == delimiter) {
            count++;
        }
    }
    Array(String) strings = array_new(count, struct String);
    if (!strings) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'strings'");
        return nullptr;
    }
    int index = 0;
    for (int i = 0, start = 0; i <= string.length; i++) {
        if (string.data[i] == delimiter || i == string.length) {
            struct String* new_string = string_memory_alloc(sizeof(struct String) + i - start + NULL_TERMINATOR);
            if (!new_string) {
                set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
                return nullptr;
            }
            new_string->data = new_string->buffer;
            memcpy(new_string->buffer, string.data + start, i - start);
            new_string->length = i - start;
            new_string->buffer[new_string->length] = '\0';
            strings[index++] = new_string;
            start = i + 1;
        }
    }
    return strings;
}

Array(String) (string_lines)(struct String string) {
    return (string_split)(string, '\n');
}

String (string_to_uppercase)(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->buffer;

    for (int i = 0; i < string.length; i++) {
        new_string->buffer[i] = toupper(string.data[i]);
    }
    new_string->buffer[new_string->length] = '\0';
    return new_string;
}

String (string_to_lowercase)(struct String string) {
    if (require_non_null(string.data)) return nullptr;

    struct String* new_string = string_memory_alloc(sizeof(struct String) + string.length + NULL_TERMINATOR);
    if (!new_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }
    new_string->length = string.length;
    new_string->data = new_string->buffer;

    for (int i = 0; i < string.length; i++) {
        new_string->buffer[i] = tolower(string.data[i]);
    }
    new_string->buffer[new_string->length] = '\0';
    return new_string;
}

#define DEFINE_STRING_VALUE_OF(func_name, type, format)                             \
    String func_name(type value) {                                                  \
        const int length = snprintf(nullptr, 0, format, value) + NULL_TERMINATOR;   \
        char data[length];                                                          \
        snprintf(data, length, format, value);                                      \
        return string_new(data);                                                    \
    }

String string_value_of_bool(bool value) {
    return string_new(value ? "true" : "false");
}

DEFINE_STRING_VALUE_OF(string_value_of_char, char, "%c")

DEFINE_STRING_VALUE_OF(string_value_of_int, int, "%d")

DEFINE_STRING_VALUE_OF(string_value_of_uint, unsigned int, "%u")

DEFINE_STRING_VALUE_OF(string_value_of_long, long, "%ld")

DEFINE_STRING_VALUE_OF(string_value_of_ulong, unsigned long, "%lu")

DEFINE_STRING_VALUE_OF(string_value_of_long_long, long long, "%lld")

DEFINE_STRING_VALUE_OF(string_value_of_ulong_long, unsigned long long, "%llu")

DEFINE_STRING_VALUE_OF(string_value_of_float, float, "%f")

DEFINE_STRING_VALUE_OF(string_value_of_double, double, "%f")

DEFINE_STRING_VALUE_OF(string_value_of_long_double, long double, "%Lf")

#undef DEFINE_STRING_VALUE_OF

uint64_t string_hash_callback(const void* raw_string) {
    const char* string = ((String) raw_string)->data;
    uint64_t hash = 5381;
    for (int c = *string; c != '\0'; c = *++string) {
        hash = hash * 33 + c;
    }
    return hash;
}

bool string_equals_callback(const void* string, const void* other_string) {
    return (string_equals)(*((String) string), *((String) other_string));
}

int string_compare_callback(const void* string, const void* other_string) {
    return (string_compare)(*((String) string), *((String) other_string));
}

void string_destroy_callback(void* string) {
    string_memory_dealloc(string);
}

String string_to_string_callback(const void* string) {
    return string;
}

struct String string_self(String string) {
    return string ? *string : (struct String) {};
}

struct String (string_ref)(const char* raw_string) {
    return (struct String) { strlen(raw_string), (char*) raw_string };
}

char char_self(char c) {
    return c;
}

void string_array_destroy(Array(String) strings) {
    for (int i = 0; i < array_length(strings); i++) {
        string_destroy(&strings[i]);
    }
    (array_destroy)(strings);
}

/* --------------------------------------------------------------------------------- */

constexpr int INITIAL_CAPACITY = 10;
constexpr int GROWN_FACTOR = 2;

StringBuilder* string_builder_init(StringBuilder* builder) {
    if (require_non_null(builder)) return nullptr;

    char* buffer = memory_try_alloc(INITIAL_CAPACITY * sizeof(char) + NULL_TERMINATOR);
    if (!buffer) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to initialize 'string builder' buffer");
        return nullptr;
    }
    buffer[0] = '\0';
    builder->buffer = buffer;
    builder->length = 0;
    builder->capacity = INITIAL_CAPACITY;
    return builder;
}

void (string_builder_append)(StringBuilder* builder, struct String string) {
    if (require_non_null(builder)) return;

    if (builder->length + string.length >= builder->capacity) {
        char* new_buffer = memory_try_realloc(builder->buffer, builder->capacity * GROWN_FACTOR + NULL_TERMINATOR);
        if (!new_buffer) {
            set_error(MEMORY_ALLOCATION_ERROR, "failed to resize 'string builder' buffer");
            return;
        }
        builder->buffer = new_buffer;
        builder->capacity = builder->capacity * GROWN_FACTOR;
    }
    memcpy(builder->buffer + builder->length, string.data, string.length);
    builder->length += string.length;
    builder->buffer[builder->length] = '\0';
}

void (string_builder_insert)(StringBuilder* builder, int index, struct String string) {
    if (require_non_null(builder)) return;

    if (index < 0 || index > builder->length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, builder->length);
        return;
    }
    if (builder->length + string.length >= builder->capacity) {
        char* buffer = memory_try_realloc(builder->buffer, builder->capacity * GROWN_FACTOR + NULL_TERMINATOR);
        if (!buffer) {
            set_error(MEMORY_ALLOCATION_ERROR, "failed to resize 'string builder' buffer");
            return;
        }
        builder->buffer = buffer;
        builder->capacity = builder->capacity * GROWN_FACTOR;
    }
    memmove(builder->buffer + index + string.length, builder->buffer + index, builder->length);
    memmove(builder->buffer + index, string.data, string.length);
    builder->length += string.length;
    builder->buffer[builder->length] = '\0';
}

String string_builder_to_string(StringBuilder* builder) {
    if (require_non_null(builder)) return nullptr;

    return string_new(builder->buffer);
}

void string_builder_close(StringBuilder* builder) {
    if (require_non_null(builder)) return;
    memory_dealloc(builder->buffer);
    builder->buffer = nullptr;
    builder->length = 0;
    builder->capacity = 0;
}