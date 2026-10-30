#include "string.h"

#include "errors.h"
#include "constraints.h"

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

Allocator* string_memory_allocator = &global_memory_allocator;

static void* string_memory_try_alloc(bytes size) {
    if (require_non_null(string_memory_allocator)) return nullptr;

    return allocator_alloc(string_memory_allocator, size);
}

static void string_memory_dealloc(void* pointer) {
    if (require_non_null(string_memory_allocator)) return;

    allocator_dealloc(string_memory_allocator, pointer);
}

static constexpr int NULL_TERMINATOR = 1;

struct String {
    int length;
    bool owned;
    const char* data;
    char buffer[];
};

StringView string_view_from_string(String string) {
    return (StringView) { string ? string->length : 0, string ? string->data : nullptr };
}

StringView string_view_from_raw_string(const char* raw_string) {
    return (StringView) { strlen(raw_string), (char*) raw_string };
}

char string_view_ignore_char(char c) {
    return c;
}

static String string_create(int length) {
    const String string = string_memory_try_alloc(sizeof(struct String) + length + NULL_TERMINATOR);

    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new string'");
        return nullptr;
    }

    string->length = length;
    string->owned = true;
    string->data = string->buffer;
    string->buffer[length] = '\0';

    return string;
}

String (string_new)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    const String new_string = string_create(string.length);

    if (!new_string) return nullptr;

    memory_copy(string.data, new_string->buffer, string.length);

    return new_string;
}

String (string_ref)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    const String new_string = string_create(string.length);

    if (!new_string) return nullptr;

    new_string->data = string.data;

    return new_string;
}

String (string_local)(StringView string, StringStorage* storage) {
    if (string.length > 32) {
        set_error(MEMORY_ALLOCATION_ERROR, "string exceeds local storage capacity");
        return nullptr;
    }

    const String new_string = (String) storage;

    new_string->length = string.length;
    new_string->owned = false;
    new_string->data = new_string->buffer;

    memory_copy(string.data, new_string->buffer, string.length);
    new_string->buffer[new_string->length] = '\0';

    return new_string;
}

String (string_static)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    constexpr int STRING_POOL_CAPACITY = 256;
    static alignas(struct String) uchar string_pool[STRING_POOL_CAPACITY * sizeof(struct String)];
    static int string_pool_size = 0;

    if (string_pool_size >= STRING_POOL_CAPACITY) {
        set_error(MEMORY_ALLOCATION_ERROR, "string pool exhausted");
        return nullptr;
    }

    const String new_string = (String) &string_pool[string_pool_size * sizeof(struct String)];

    new_string->length = string.length;
    new_string->owned = false;
    new_string->data = string.data;

    string_pool_size++;
    return new_string;
}

void (string_destroy)(String* string_pointer) {
    if (require_non_null(string_pointer, *string_pointer)) return;
    String string = *string_pointer;

    if (!string->owned) {
        set_error(ILLEGAL_STATE_ERROR, "cannot destroy a non-owned string");
        return;
    }
    string_memory_dealloc(string);
    *string_pointer = nullptr;
}

void string_destroy_all(void) {
    if (require_non_null(string_memory_allocator)) return;

    allocator_reset(string_memory_allocator);
}

String (string_format)(StringView format, ...) {
    if (require_non_null(format.data)) return nullptr;

    va_list parameters = {};
    va_start(parameters, format);
    const int length = vsnprintf(nullptr, 0, format.data, parameters);
    va_end(parameters);

    const String string = string_create(length);

    if (!string) return nullptr;

    va_start(parameters, format);
    vsnprintf(string->buffer, length + NULL_TERMINATOR, format.data, parameters);
    va_end(parameters);

    return string;
}

uint64 (string_hash)(StringView string) {
    if (require_non_null(string.data)) return -1;

    const char* data = string.data;
    uint64 hash = 5381;
    for (int c = *data; c != '\0'; c = *++data) {
        hash = hash * 33 + c;
    }
    return hash;
}

char (string_char_at)(StringView string, int index) {
    if (require_non_null(string.data)) return '\0';
    if (index < 0 || index >= string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, string.length);
        return '\0';
    }
    return string.data[index];
}

int (string_length)(StringView string) {
    return string.length;
}

const char* (string_data)(StringView string) {
    return string.data;
}

bool (string_is_empty)(StringView string) {
    if (require_non_null(string.data)) return false;
    return string.length == 0;
}

bool (string_is_blank)(StringView string) {
    if (require_non_null(string.data)) return false;
    for (int i = 0; i < string.length; i++) {
        if (!isspace((uchar) string.data[i]))
            return false;
    }
    return true;
}

int (string_compare)(StringView string, StringView other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncmp(string.data, other_string.data, string.length);
}

int (string_compare_ignore_case)(StringView string, StringView other_string) {
    if (require_non_null(string.data, other_string.data)) return 0;

    if(string.length > other_string.length) return 1;
    if(string.length < other_string.length) return -1;

    return strncasecmp(string.data, other_string.data, string.length);
}

bool (string_equals)(StringView string, StringView other_string) {
    return (string_compare)(string, other_string) == 0;
}

bool (string_equals_ignore_case)(StringView string, StringView other_string) {
    return (string_compare_ignore_case)(string, other_string) == 0;
}

int (string_index_of_char)(StringView string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int (string_last_index_of_char)(StringView string, char character) {
    if (require_non_null(string.data)) return 0;

    for (int i = string.length - 1; i >= 0; i--) {
        if (string.data[i] == character) return i;
    }
    return -1;
}

int (string_index_of_substring)(StringView string, StringView substring) {
    if (require_non_null(string.data, substring.data)) return 0;

    if (substring.length == 0) return 0;
    if (substring.length > string.length) return -1;

    for (int i = 0; i <= string.length - substring.length; i++) {
        if (memcmp(string.data + i, substring.data, (bytes) substring.length) == 0) {
            return i;
        }
    }
    return -1;
}

int (string_last_index_of_substring)(StringView string, StringView substring) {
    if (require_non_null(string.data, substring.data)) return 0;

    if (substring.length == 0) return 0;
    if (substring.length > string.length) return -1;

    for (int i = string.length - substring.length; i >= 0; i--) {
        if (memcmp(string.data + i, substring.data, (bytes) substring.length) == 0) {
            return i;
        }
    }
    return -1;
}

bool (string_contains)(StringView string, StringView substring) {
    return string_index_of_substring(string, substring) >= 0;
}

bool (string_starts_with)(StringView string, StringView prefix) {
    if (require_non_null(string.data, prefix.data)) return false;

    return string.length >= prefix.length && memcmp(string.data, prefix.data, (bytes) prefix.length) == 0;
}

bool (string_ends_with)(StringView string, StringView suffix) {
    if (require_non_null(string.data, suffix.data)) return false;

    return string.length >= suffix.length
        && memcmp(string.data + string.length - suffix.length, suffix.data, (bytes) suffix.length) == 0;
}

String (string_trim)(StringView string) {
    String half = (string_trim_end)(string);
    String full = string_trim_start(half);
    string_memory_dealloc(half);
    return full;
}

String (string_trim_start)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    int start = 0;
    while (start < string.length && isspace(string.data[start])) {
        start++;
    }
    const int new_length = string.length - start;

    const String new_string = string_create(new_length);

    if (!new_string) return nullptr;

    memory_copy(string.data + start, new_string->buffer, string.length - start);

    return new_string;
}

String (string_trim_end)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    int end = string.length;
    while (end > 0 && isspace(string.data[end - 1])) {
        end--;
    }
    const int new_length = end;

    const String new_string = string_create(new_length);

    if (!new_string) return nullptr;

    memory_copy(string.data, new_string->buffer, end);

    return new_string;
}

String (string_substring)(StringView string, int start, int length) {
    if (require_non_null(string.data)) return nullptr;

    if (start < 0 || length < 0 || start > string.length) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, length = %d", start, length);
        return nullptr;
    }
    const int max_length = string.length - start;
    const int new_length = length > max_length ? max_length : length;

    const String new_string = string_create(new_length);

    if (!new_string) return nullptr;

    memory_copy(string.data + start, new_string->buffer, new_length);

    return new_string;
}

String (string_concat)(StringView string, StringView other_string) {
    if (require_non_null(string.data, other_string.data)) return nullptr;

    const int length = string.length + other_string.length;

    const String new_string = string_create(length);

    if (!new_string) return nullptr;

    memory_copy(string.data, new_string->buffer, string.length);
    memory_copy(other_string.data, new_string->buffer + string.length, other_string.length);

    return new_string;
}

String (string_replace_char)(StringView string, char character, char replacement) {
    if (require_non_null(string.data)) return nullptr;

    const String new_string = string_create(string.length);

    if (!new_string) return nullptr;

    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == character) {
            new_string->buffer[i] = replacement;
        } else {
            new_string->buffer[i] = string.data[i];
        }
    }
    return new_string;
}

String (string_replace_substring)(StringView string, StringView target, StringView replacement) {
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

    const String new_string = string_create(new_length);

    if (!new_string) return nullptr;

    int input_index = 0;
    int output_index = 0;

    while (input_index < string.length) {
        if (input_index <= string.length - target.length && memcmp(string.data + input_index, target.data, target.length) == 0) {
            memory_copy(replacement.data, new_string->buffer + output_index, replacement.length);
            output_index += replacement.length;
            input_index += target.length;
        } else {
            new_string->buffer[output_index++] = string.data[input_index++];
        }
    }
    return new_string;
}

String (string_repeat)(StringView string, int times) {
    if (require_non_null(string.data)) return nullptr;

    if (times < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'times' parameter can't be negative");
        return nullptr;
    }
    const int length = string.length * times;

    const String new_string = string_create(length);

    if (!new_string) return nullptr;

    for (int i = 0, offset = 0; i < times; i++) {
        memory_copy(string.data, new_string->buffer + offset, string.length);
        offset += string.length;
    }
    return new_string;
}

String (string_join)(StringView separator, ...) {
    if (require_non_null(separator.data)) return nullptr;

    va_list parameters = {};
    int total_length = 0;

    va_start(parameters, separator);
    while (true) {
        const StringView string = va_arg(parameters, StringView);

        if (!string.data) break;

        total_length += string.length + separator.length;
    }
    total_length -= separator.length;
    va_end(parameters);

    const String new_string = string_create(total_length);

    if (!new_string) return nullptr;

    va_start(parameters, separator);
    int offset = 0;
    while (true) {
        const StringView string = va_arg(parameters, StringView);

        if (!string.data) break;

        memory_copy(string.data, new_string->buffer + offset, string.length);

        if (offset + string.length + separator.length < total_length) {
            memory_copy(separator.data, new_string->buffer + offset + string.length, separator.length);
        }
        offset += string.length + separator.length;
    }
    va_end(parameters);

    return new_string;
}

Array(String) (string_split)(StringView string, char delimiter) {
    if (require_non_null(string.data)) return nullptr;
    int count = 1;
    for (int i = 0; i < string.length; i++) {
        if (string.data[i] == delimiter) {
            count++;
        }
    }
    Array(String) strings = array_new(count, String);
    if (!strings) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'strings'");
        return nullptr;
    }
    int index = 0;
    for (int i = 0, start = 0; i <= string.length; i++) {
        if (string.data[i] == delimiter || i == string.length) {
            const String new_string = string_create(i - start);

            if (!new_string) return nullptr;

            memory_copy(string.data + start, new_string->buffer, i - start);

            strings[index++] = new_string;
            start = i + 1;
        }
    }
    return strings;
}

Array(String) (string_lines)(StringView string) {
    return (string_split)(string, '\n');
}

String (string_to_uppercase)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    const String new_string = string_create(string.length);

    if (!new_string) return nullptr;

    for (int i = 0; i < string.length; i++) {
        new_string->buffer[i] = toupper(string.data[i]);
    }
    return new_string;
}

String (string_to_lowercase)(StringView string) {
    if (require_non_null(string.data)) return nullptr;

    const String new_string = string_create(string.length);

    if (!new_string) return nullptr;

    for (int i = 0; i < string.length; i++) {
        new_string->buffer[i] = tolower(string.data[i]);
    }
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

DEFINE_STRING_VALUE_OF(string_value_of_uint, uint, "%u")

DEFINE_STRING_VALUE_OF(string_value_of_long, long, "%ld")

DEFINE_STRING_VALUE_OF(string_value_of_ulong, ulong, "%lu")

DEFINE_STRING_VALUE_OF(string_value_of_long_long, long long, "%lld")

DEFINE_STRING_VALUE_OF(string_value_of_ulong_long, unsigned long long, "%llu")

DEFINE_STRING_VALUE_OF(string_value_of_float, float, "%f")

DEFINE_STRING_VALUE_OF(string_value_of_double, double, "%f")

DEFINE_STRING_VALUE_OF(string_value_of_long_double, long double, "%Lf")

#undef DEFINE_STRING_VALUE_OF

static long parse_long(const char* string, long min, long max) {
    if (require_non_null(string)) return 0;

    errno = 0;
    char *end;
    const long value = strtol(string, &end, 10);

    if (errno == ERANGE || end == string || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string);
        return 0;
    }
    if (value < min || value > max) {
        set_error(NUMBER_FORMAT_ERROR, "number is out of range '%s'", string);
        return 0;
    }
    return value;
}

static ulong parse_ulong(const char* string, ulong max) {
    if (require_non_null(string)) return 0;

    if (string[0] == '-') {
        set_error(NUMBER_FORMAT_ERROR, "number is out of range '%s'", string);
        return 0;
    }

    errno = 0;
    char *end;
    const ulong value = strtoul(string, &end, 10);

    if (errno == ERANGE || end == string || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string);
        return 0;
    }
    if (value > max) {
        set_error(NUMBER_FORMAT_ERROR, "number is out of range '%s'", string);
        return 0;
    }
    return value;
}

short (string_to_short)(StringView string) {
    return parse_long(string.data, SHRT_MIN, SHRT_MAX);
}

ushort (string_to_ushort)(StringView string) {
    return parse_ulong(string.data, USHRT_MAX);
}

int (string_to_int)(StringView string) {
    return parse_long(string.data, INT_MIN, INT_MAX);
}

uint (string_to_uint)(StringView string) {
    return parse_ulong(string.data, UINT_MAX);
}

long (string_to_long)(StringView string) {
    return parse_long(string.data, LONG_MIN, LONG_MAX);
}

ulong (string_to_ulong)(StringView string) {
    return parse_ulong(string.data, ULONG_MAX);
}

long long (string_to_long_long)(StringView string) {
    if (require_non_null(string.data)) return 0;

    errno = 0;
    char *end;
    const long long value = strtoll(string.data, &end, 10);

    if (errno == ERANGE || end == string.data || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string.data);
        return 0;
    }
    return value;
}

unsigned long long (string_to_ulong_long)(StringView string) {
    if (require_non_null(string.data)) return 0;

    if (string.data[0] == '-') {
        set_error(NUMBER_FORMAT_ERROR, "number is out of range '%s'", string.data);
        return 0;
    }

    errno = 0;
    char *end;
    const unsigned long long value = strtoull(string.data, &end, 10);

    if (errno == ERANGE || end == string.data || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string.data);
        return 0;
    }
    return value;
}

float (string_to_float)(StringView string) {
    if (require_non_null(string.data)) return 0;

    errno = 0;
    char *end;
    const float value = strtof(string.data, &end);

    if (errno == ERANGE || end == string.data || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string.data);
        return 0.0f;
    }
    return value;
}

double (string_to_double)(StringView string) {
    if (require_non_null(string.data)) return 0;

    errno = 0;
    char *end;
    const double value = strtod(string.data, &end);

    if (errno == ERANGE || end == string.data || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string.data);
        return 0.0;
    }
    return value;
}

long double (string_to_long_double)(StringView string) {
    if (require_non_null(string.data)) return 0;

    errno = 0;
    char *end;
    const long double value = strtold(string.data, &end);

    if (errno == ERANGE || end == string.data || *end != '\0') {
        set_error(NUMBER_FORMAT_ERROR, "invalid number format '%s'", string.data);
        return 0.0L;
    }
    return value;
}

uint64 string_hash_generic(const void* string) {
    return string_hash((String) string);
}

bool string_equals_generic(const void* string, const void* other_string) {
    return string_equals((String) string, (String) other_string);
}

int string_compare_generic(const void* string, const void* other_string) {
    return string_compare((String) string, (String) other_string);
}

void string_destroy_generic(void* string) {
    string_memory_dealloc(string);
}

String string_to_string_generic(const void* string) {
    return (String) string;
}

void string_array_destroy(Array(String) strings) {
    for (int i = 0; i < array_length(strings); i++) {
        string_destroy(&strings[i]);
    }
    (array_destroy)(strings);
}

/* --------------------------------------------------------------------------------- */

static constexpr int INITIAL_CAPACITY = 10;
static constexpr int GROWN_FACTOR = 2;

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

void (string_builder_append)(StringBuilder* builder, StringView string) {
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
    memory_copy(string.data, builder->buffer + builder->length, string.length);
    builder->length += string.length;
    builder->buffer[builder->length] = '\0';
}

void (string_builder_insert)(StringBuilder* builder, int index, StringView string) {
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
    memory_move(builder->buffer + index, builder->buffer + index + string.length, builder->length);
    memory_move(string.data, builder->buffer + index, string.length);
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