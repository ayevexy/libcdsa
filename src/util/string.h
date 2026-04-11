#ifndef LIBCDSA_STRING_H
#define LIBCDSA_STRING_H

#include <stddef.h>

extern void* (*strings_memory_alloc)(size_t);

extern void (*strings_memory_dealloc)(void*);

typedef struct {
    const char* data;
    int length;
} String;

typedef String StringView;

typedef String StringOwned;

static inline String string_null() {
    return (String) { .data = nullptr, .length = 0 };
}

static inline String string_empty() {
    return (String) { .data = "", .length = 0 };
}

StringOwned string_new(const char* raw_string);

void string_destroy(StringOwned* string);

StringView string_view(const char* raw_string);

StringOwned string_format(const char* format, ...);

char string_char_at(String string, int index);

bool string_is_empty(String string);

bool string_is_blank(String string);

int string_compare(String string, String other_string);

int string_compare_ignore_case(String string, String other_string);

bool string_equals(String string, String other_string);

bool string_equals_ignore_case(String string, String other_string);

int string_index_of_char(String string, char character);

int string_last_index_of_char(String string, char character);

int string_index_of_substring(String string, String substring);

int string_last_index_of_substring(String string, String substring);

bool string_contains(String string, String substring);

bool string_starts_with(String string, String prefix);

bool string_ends_with(String string, String suffix);

StringView string_trim(String string);

StringView string_trim_start(String string);

StringView string_trim_end(String string);

StringView string_substring(String string, int start, int length);

StringView string_subsequence(String string, int start, int end);

StringView string_clone(String string);

StringOwned string_concat(String string, String other_string);

StringOwned string_indent(String string, int indent);

StringOwned string_replace_char(String string, char character, char replacement);

StringOwned string_replace_substring(String string, String substring, String replacement);

StringOwned string_repeat(String string, int times);

StringOwned string_join(String separator, ...);

StringView* string_split(String string, char delimiter);

StringView* string_lines(String string);

StringOwned string_to_uppercase(String string);

StringOwned string_to_lowercase(String string);

#endif