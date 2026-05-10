#ifndef LIBCDSA_STRING_H
#define LIBCDSA_STRING_H

#include "array.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Internal memory allocation function used internally
 * to allocate String instances. Defaults to malloc.
 */
extern void* (*string_memory_alloc)(size_t);

/**
 * @brief Internal memory deallocation function used internally
 * to deallocate String instances. Defaults to free.
 */
extern void (*string_memory_dealloc)(void*);

/**
 * @brief String types abstraction over c-strings. The String types are conceptually immutable
 * and all operations that would modify a string return a new modified version instead.
 *
 * They are divided into two distinct types: StringView and String. StringView
 * are just references to other strings while String are a distinct objects with their
 * own internal buffer that must be freed manually.
 *
 * Memory ownership:
 * - StringView does NOT own memory and must not be freed.
 * - String owns its buffer and must be freed using string_destroy().
 * - Functions returning StringView reference existing memory.
 * - Functions returning String allocate new memory.
 */

/** @brief Owned String type, must be freed manually */
typedef struct {
    int length;
    char data[];
} String;

/** @brief Non-Owning String type that references some other string */
typedef struct {
    const char* data;
    int length;
} StringView;

/**
 * @brief Creates a new allocated string copying another string.
 *
 * @param string source string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_new(string) _string_new(string_view(string))

String* _string_new(StringView string);

/**
 * @brief Destroys a previously allocated string.
 *
 * @param string_pointer pointer to a string pointer
 *
 * @post *string_pointer == nullptr
 *
 * @exception NULL_POINTER_ERROR if string_pointer || *string_pointer is null
 */
void string_destroy(String** string_pointer);

/**
 * @brief Creates a view over an existing string, referencing it.
 *
 * @param string the string
 *
 * @return a string view
 *
 * @exception NULL_POINTER_ERROR if string is null
 */
#define string_view(string) (_Generic((string), \
    String*: _string_as_view,                   \
    StringView: _string_view_self,              \
    const char*: _string_view,                  \
    char*: _string_view,                        \
    default: _string_view_null                  \
)(string))

StringView _string_as_view(String* string);

StringView _string_view_self(StringView string_view);

StringView _string_view(const char* raw_string);

StringView _string_view_null(char character);

/**
 * @brief Creates a newly allocated formatted string.
 *
 * @param format the format
 * @param ... arguments
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if format is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
String* string_format(const char* format, ...);

/**
 * @brief Retrieves the character at the specified position of the string.
 *
 * @param string the string
 * @param index character position
 *
 * @return the character
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= string.length
 */
#define string_char_at(string, index) _string_char_at(string_view(string), index)

char _string_char_at(StringView string, int index);

/**
 * @brief Checks whether the string is empty.
 *
 * @param string the string
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_empty(string) _string_is_empty(string_view(string))

bool _string_is_empty(StringView string);

/**
 * @brief Checks whether the string is blank.
 *
 * @param string the string
 *
 * @return true if blank, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_blank(string) _string_is_blank(string_view(string))

bool _string_is_blank(StringView string);

/**
 * @brief Calculate a hash value of a string.
 *
 * @param raw_string the string
 *
 * @return the hashed value
 */
uint64_t string_hash(const void* raw_string);

/**
 * @brief Compares two strings lexicographically.
 *
 * @param string first string
 * @param other_string second string
 *
 * @return 0 if equal; negative if the first string is less; positive if greater
 *
 * @exception NULL_POINTER_ERROR if string.data || other_string.data is null
 */
#define string_compare(string, other_string) _string_compare(string_view(string), string_view(other_string))

int _string_compare(StringView string, StringView other_string);

/**
 * @brief Compares two strings lexicographically ignoring case.
 *
 * @param string first string
 * @param other_string second string
 *
 * @return 0 if equal; negative if the first string is less; positive if greater
 *
 * @exception NULL_POINTER_ERROR if string.data || other_string.data is null
 */
#define string_compare_ignore_case(string, other_string) _string_compare_ignore_case(string_view(string), string_view(other_string))

int _string_compare_ignore_case(StringView string, StringView other_string);

/**
 * @brief Checks whether two strings are lexicographically equals.
 *
 * @param string first string
 * @param other_string second string
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data || other_string.data is null
 */
#define string_equals(string, other_string) _string_equals(string_view(string), string_view(other_string))

bool _string_equals(StringView string, StringView other_string);

/**
 * @brief Checks whether two strings are lexicographically equals ignoring case.
 *
 * @param string first string
 * @param other_string second string
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data || other_string.data is null
 */
#define string_equals_ignore_case(string, other_string) _string_equals_ignore_case(string_view(string), string_view(other_string))

bool _string_equals_ignore_case(StringView string, StringView other_string);

/**
 * @brief Retrieves the index of the first occurrence of the specified character or substring in the string.
 *
 * @param string the string
 * @param needle a character or substring to search
 *
 * @return the character or substring first index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_index_of(string, needle) _Generic((needle),  \
    char: _string_index_of_char,                            \
    int:  _string_index_of_char,                            \
    default: _string_index_of_substring                     \
)(                                                          \
    string_view(string), _Generic((needle),                 \
        char: (needle),                                     \
        int: (needle),                                      \
        default: string_view(needle)                        \
    )                                                       \
)

int _string_index_of_char(StringView string, char character);

int _string_index_of_substring(StringView string, StringView substring);

/**
 * @brief Retrieves the index of the last occurrence of the specified character or substring in the string.
 *
 * @param string the string
 * @param needle a character or substring to search
 *
 * @return the character or substring last index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_last_index_of(string, needle) _Generic((needle), \
    char: _string_last_index_of_char,                           \
    int:  _string_last_index_of_char,                           \
    default: _string_last_index_of_substring                    \
)(                                                              \
    string_view(string), _Generic((needle),                     \
        char: (needle),                                         \
        int: (needle),                                          \
        default: string_view(needle)                            \
    )                                                           \
)

int _string_last_index_of_char(StringView string, char character);

int _string_last_index_of_substring(StringView string, StringView substring);

/**
 * @brief Checks whether a substring is present in the string.
 *
 * @param string the string
 * @param substring substring to search
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data || substring.data is null
 */
#define string_contains(string, substring) _string_contains(string_view(string), string_view(substring))

bool _string_contains(StringView string, StringView substring);

/**
 * @brief Checks whether a string starts with the specified prefix.
 *
 * @param string the string
 * @param prefix the prefix
 *
 * @return true if yes, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data || prefix.data is null
 */
#define string_starts_with(string, prefix) _string_starts_with(string_view(string), string_view(prefix))

bool _string_starts_with(StringView string, StringView prefix);

/**
 * @brief Checks whether a string ends with the specified suffix.
 *
 * @param string the string
 * @param suffix the suffix
 *
 * @return true if yes, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data || suffix.data is null
 */
#define string_ends_with(string, prefix) _string_ends_with(string_view(string), string_view(prefix))

bool _string_ends_with(StringView string, StringView suffix);

/**
 * @brief Removes all white spaces from both the beginning and the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_trim(string) _string_trim(string_view(string))

StringView _string_trim(StringView string);

/**
 * @brief Removes all white spaces from the beginning of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_trim_start(string) _string_trim_start(string_view(string))

StringView _string_trim_start(StringView string);

/**
 * @brief Removes all white spaces from the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_trim_end(string) _string_trim_end(string_view(string))

StringView _string_trim_end(StringView string);

/**
 * @brief Creates a substring of the string with the specified position and length.
 *
 * @param string the string
 * @param start start index (inclusive)
 * @param length length of the substring
 *
 * @return a substring view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start < 0 || length < 0 || start > string.length
 */
#define string_substring(string, start, length) _string_substring(string_view(string), start, length)

StringView _string_substring(StringView string, int start, int length);

/**
 * @brief Concatenates two strings into a new allocated string.
 *
 * @param string first string
 * @param other_string second string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data || other_string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_concat(string, other_string) _string_concat(string_view(string), string_view(other_string))

String* _string_concat(StringView string, StringView other_string);

/**
 * Replaces all occurrences of a character or substring in the string with a new one.
 *
 * @param string the string
 * @param target the target character or substring
 * @param replacement the replacement character or substring
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_replace(string, target, replacement) _Generic((target),          \
    char: _string_replace_char,                                                 \
    int: _string_replace_char,                                                  \
    default: _string_replace_substring                                          \
)(                                                                              \
    string_view(string), _Generic((target),                                     \
        char: (target),                                                         \
        int: (target),                                                          \
        default: string_view(target)                                            \
    ), _Generic((replacement),                                                  \
        char: (replacement),                                                    \
        int: (replacement),                                                     \
        default: string_view(replacement)                                       \
    )                                                                           \
)

String* _string_replace_char(StringView string, char character, char replacement);

String* _string_replace_substring(StringView string, StringView target, StringView replacement);

/**
 * @brief Concatenates the same string n times.
 *
 * @param string the string
 * @param times the amount of repetitions
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception ILLEGAL_ARGUMENT_ERROR if times is negative
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_repeat(string, times) _string_repeat(string_view(string), times)

String* _string_repeat(StringView string, int times);

/**
 * @brief Concatenates n strings repeatedly inserting the specified separator between them.
 *
 * @param separator the separator
 * @param ... a string list (terminated by a null string view `(StringView) {}`)
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if separator.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @warning the string list MUST be terminated by a `(StringView) {}` element, otherwise, it's undefined behavior
 *  this macro automatically inserts the null string.
 */
#define string_join(separator, ...) _string_join(string_view(separator), _dispatch_string_join(__VA_ARGS__, \
    _string_join_10,                                                                                        \
    _string_join_9,                                                                                         \
    _string_join_8,                                                                                         \
    _string_join_7,                                                                                         \
    _string_join_6,                                                                                         \
    _string_join_5,                                                                                         \
    _string_join_4,                                                                                         \
    _string_join_3,                                                                                         \
    _string_join_2,                                                                                         \
    _string_join_1                                                                                          \
)(__VA_ARGS__))

#define _dispatch_string_join(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

#define _string_join_10(s, ...) string_view(s), _string_join_9(__VA_ARGS__)
#define _string_join_9(s, ...) string_view(s), _string_join_8(__VA_ARGS__)
#define _string_join_8(s, ...) string_view(s), _string_join_7(__VA_ARGS__)
#define _string_join_7(s, ...) string_view(s), _string_join_6(__VA_ARGS__)
#define _string_join_6(s, ...) string_view(s), _string_join_5(__VA_ARGS__)
#define _string_join_5(s, ...) string_view(s), _string_join_4(__VA_ARGS__)
#define _string_join_4(s, ...) string_view(s), _string_join_3(__VA_ARGS__)
#define _string_join_3(s, ...) string_view(s), _string_join_2(__VA_ARGS__)
#define _string_join_2(s, ...) string_view(s), _string_join_1(__VA_ARGS__)
#define _string_join_1(s) string_view(s), (StringView) {} // sentinel

String* _string_join(StringView separator, ...);

/**
 * @brief Split a string into n strings by the specified delimiter.
 *
 * @param string the string
 * @param delimiter delimiter character
 *
 * @return an array of string views over the original string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the returned array must be freed manually (by `array_destroy()`).
 */
#define string_split(string, delimiter) _string_split(string_view(string), delimiter)

Array(StringView) _string_split(StringView string, char delimiter);

/**
 * @brief Split a string into multiple lines.
 *
 * @param string the string
 *
 * @return an array of string views over the original string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the returned array must be freed manually (by `array_destroy()`).
 */
#define string_lines(string) _string_lines(string_view(string))

Array(StringView) _string_lines(StringView string);

/**
 * @brief Creates a new allocated string from an existing one with all characters uppercased.
 *
 * @param string the string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_to_uppercase(string) _string_to_uppercase(string_view(string))

String* _string_to_uppercase(StringView string);

/**
 * @brief Creates a new allocated string from an existing one with all characters lowercased.
 *
 * @param string the string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_to_lowercase(string) _string_to_lowercase(string_view(string))

String* _string_to_lowercase(StringView string);

/**
 * @brief Converts a primitive value to its string representation.
 *
 * @param value the value
 *
 * @return a new allocated string
 */
#define string_value_of(value) _Generic((value),        \
    bool: _string_value_of_bool,                        \
    char: _string_value_of_char,                        \
    signed char: _string_value_of_int,                  \
    unsigned char: _string_value_of_uint,               \
    short: _string_value_of_int,                        \
    unsigned short: _string_value_of_uint,              \
    int: _string_value_of_int,                          \
    unsigned int: _string_value_of_uint,                \
    long: _string_value_of_long,                        \
    unsigned long: _string_value_of_ulong,              \
    long long: _string_value_of_long_long,              \
    unsigned long long: _string_value_of_ulong_long,    \
    float: _string_value_of_float,                      \
    double: _string_value_of_double,                    \
    long double: _string_value_of_long_double           \
)(value)

String* _string_value_of_bool(bool value);
String* _string_value_of_char(char value);
String* _string_value_of_int(int value);
String* _string_value_of_uint(unsigned int value);
String* _string_value_of_long(long value);
String* _string_value_of_ulong(unsigned long value);
String* _string_value_of_long_long(long long value);
String* _string_value_of_ulong_long(unsigned long long value);
String* _string_value_of_float(float value);
String* _string_value_of_double(double value);
String* _string_value_of_long_double(long double value);

#endif