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
 * @brief String type abstraction over c-strings. The String type is conceptually immutable
 * and all operations that would modify a string return a new modified version instead.
 *
 * Memory ownership:
 * - String must be freed using string_destroy(), except for strings created by `string_ref()`.
 */
typedef struct String {
    int length;
    char* data;
    char _data[];
} * String;

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
#define string_new(string) _string_new(_string_ref(string))

String _string_new(struct String string);

/**
 * @brief Creates a string that references an existing c-string.
 *
 * @param string the string
 *
 * @return a string reference
 */
#define string_ref(string) _Generic((string),                                           \
    const char*: &(struct String) { .length = strlen(string), .data = (char*) string }, \
    char*: &(struct String) { .length = strlen(string), .data = (char*) string }        \
)

/**
 * @brief Destroys previously allocated strings.
 *
 * @param ... the strings
 *
 * @post &strings[i] == nullptr
 *
 * @exception NULL_POINTER_ERROR if &strings[i] || strings[i] is null
 */
#define string_destroy(...) (_dispatch_string_destroy(__VA_ARGS__,  \
    _string_destroy_10,                                             \
    _string_destroy_9,                                              \
    _string_destroy_8,                                              \
    _string_destroy_7,                                              \
    _string_destroy_6,                                              \
    _string_destroy_5,                                              \
    _string_destroy_4,                                              \
    _string_destroy_3,                                              \
    _string_destroy_2,                                              \
    _string_destroy_1                                               \
)(__VA_ARGS__))

#define _dispatch_string_destroy(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

#define _string_destroy_10(string, ...) _string_destroy(string) _string_destroy_9(__VA_ARGS__)
#define _string_destroy_9(string, ...) _string_destroy(string), _string_destroy_8(__VA_ARGS__)
#define _string_destroy_8(string, ...) _string_destroy(string), _string_destroy_7(__VA_ARGS__)
#define _string_destroy_7(string, ...) _string_destroy(string), _string_destroy_6(__VA_ARGS__)
#define _string_destroy_6(string, ...) _string_destroy(string), _string_destroy_5(__VA_ARGS__)
#define _string_destroy_5(string, ...) _string_destroy(string), _string_destroy_4(__VA_ARGS__)
#define _string_destroy_4(string, ...) _string_destroy(string), _string_destroy_3(__VA_ARGS__)
#define _string_destroy_3(string, ...) _string_destroy(string), _string_destroy_2(__VA_ARGS__)
#define _string_destroy_2(string, ...) _string_destroy(string), _string_destroy_1(__VA_ARGS__)
#define _string_destroy_1(string) _string_destroy(string)

void _string_destroy(String* string_pointer);

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
String string_format(const char* format, ...);

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
#define string_char_at(string, index) _string_char_at(_string_ref(string), index)

char _string_char_at(struct String string, int index);

/**
 * @brief Retrieves the length of a string.
 *
 * @param string the string
 *
 * @return the length
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_length(string) _string_length(_string_ref(string))

int _string_length(struct String string);

/**
 * @brief Retrieves the data of a string.
 *
 * @param string the string
 *
 * @return the data
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_data(string) _string_data(_string_ref(string))

const char* _string_data(struct String string);

/**
 * @brief Checks whether the string is empty.
 *
 * @param string the string
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_empty(string) _string_is_empty(_string_ref(string))

bool _string_is_empty(struct String string);

/**
 * @brief Checks whether the string is blank.
 *
 * @param string the string
 *
 * @return true if blank, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_blank(string) _string_is_blank(_string_ref(string))

bool _string_is_blank(struct String string);

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
#define string_compare(string, other_string) _string_compare(_string_ref(string), _string_ref(other_string))

int _string_compare(struct String string, struct String other_string);

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
#define string_compare_ignore_case(string, other_string) _string_compare_ignore_case(_string_ref(string), _string_ref(other_string))

int _string_compare_ignore_case(struct String string, struct String other_string);

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
#define string_equals(string, other_string) _string_equals(_string_ref(string), _string_ref(other_string))

bool _string_equals(struct String string, struct String other_string);

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
#define string_equals_ignore_case(string, other_string) _string_equals_ignore_case(_string_ref(string), _string_ref(other_string))

bool _string_equals_ignore_case(struct String string, struct String other_string);

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
)(_string_ref(string), _dispatch_string_type(needle))

int _string_index_of_char(struct String string, char character);

int _string_index_of_substring(struct String string, struct String substring);

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
)(_string_ref(string), _dispatch_string_type(needle))

int _string_last_index_of_char(struct String string, char character);

int _string_last_index_of_substring(struct String string, struct String substring);

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
#define string_contains(string, substring) _string_contains(_string_ref(string), _string_ref(substring))

bool _string_contains(struct String string, struct String substring);

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
#define string_starts_with(string, prefix) _string_starts_with(_string_ref(string), _string_ref(prefix))

bool _string_starts_with(struct String string, struct String prefix);

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
#define string_ends_with(string, prefix) _string_ends_with(_string_ref(string), _string_ref(prefix))

bool _string_ends_with(struct String string, struct String suffix);

/**
 * @brief Removes all white spaces from both the beginning and the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_trim(string) _string_trim(_string_ref(string))

String _string_trim(struct String string);

/**
 * @brief Removes all white spaces from the beginning of the string.
 *
 * @param string the string
 *
 * @return a trimmed allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_trim_start(string) _string_trim_start(_string_ref(string))

String _string_trim_start(struct String string);

/**
 * @brief Removes all white spaces from the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_trim_end(string) _string_trim_end(_string_ref(string))

String _string_trim_end(struct String string);

/**
 * @brief Creates a substring of the string with the specified position and length.
 *
 * @param string the string
 * @param start start index (inclusive)
 * @param length length of the substring
 *
 * @return a new allocated substring
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start < 0 || length < 0 || start > string.length
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_substring(string, start, length) _string_substring(_string_ref(string), start, length)

String _string_substring(struct String string, int start, int length);

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
#define string_concat(string, other_string) _string_concat(_string_ref(string), _string_ref(other_string))

String _string_concat(struct String string, struct String other_string);

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
#define string_replace(string, target, replacement) _Generic((target),                     \
    char: _string_replace_char,                                                            \
    int: _string_replace_char,                                                             \
    default: _string_replace_substring                                                     \
)(_string_ref(string), _dispatch_string_type(target), _dispatch_string_type(replacement))

String _string_replace_char(struct String string, char character, char replacement);

String _string_replace_substring(struct String string, struct String target, struct String replacement);

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
#define string_repeat(string, times) _string_repeat(_string_ref(string), times)

String _string_repeat(struct String string, int times);

/**
 * @brief Concatenates n strings repeatedly inserting the specified separator between them.
 *
 * @param separator the separator
 * @param ... a string list (terminated by a null string `(struct String) {}`)
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if separator.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @warning the string list MUST be terminated by a `(struct String) {}` element, otherwise, it's undefined behavior
 *  this macro automatically inserts the null string.
 */
#define string_join(separator, ...) _string_join(_string_ref(separator), _dispatch_string_join(__VA_ARGS__,    \
    _string_join_20,                                                                                            \
    _string_join_19,                                                                                            \
    _string_join_18,                                                                                            \
    _string_join_17,                                                                                            \
    _string_join_16,                                                                                            \
    _string_join_15,                                                                                            \
    _string_join_14,                                                                                            \
    _string_join_13,                                                                                            \
    _string_join_12,                                                                                            \
    _string_join_11,                                                                                            \
    _string_join_10,                                                                                            \
    _string_join_9,                                                                                             \
    _string_join_8,                                                                                             \
    _string_join_7,                                                                                             \
    _string_join_6,                                                                                             \
    _string_join_5,                                                                                             \
    _string_join_4,                                                                                             \
    _string_join_3,                                                                                             \
    _string_join_2,                                                                                             \
    _string_join_1                                                                                              \
)(__VA_ARGS__))

#define _dispatch_string_join(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ...) NAME

#define _string_join_20(s, ...) _string_ref(s), _string_join_19(__VA_ARGS__)
#define _string_join_19(s, ...) _string_ref(s), _string_join_18(__VA_ARGS__)
#define _string_join_18(s, ...) _string_ref(s), _string_join_17(__VA_ARGS__)
#define _string_join_17(s, ...) _string_ref(s), _string_join_16(__VA_ARGS__)
#define _string_join_16(s, ...) _string_ref(s), _string_join_15(__VA_ARGS__)
#define _string_join_15(s, ...) _string_ref(s), _string_join_14(__VA_ARGS__)
#define _string_join_14(s, ...) _string_ref(s), _string_join_13(__VA_ARGS__)
#define _string_join_13(s, ...) _string_ref(s), _string_join_12(__VA_ARGS__)
#define _string_join_12(s, ...) _string_ref(s), _string_join_11(__VA_ARGS__)
#define _string_join_11(s, ...) _string_ref(s), _string_join_10(__VA_ARGS__)
#define _string_join_10(s, ...) _string_ref(s), _string_join_9(__VA_ARGS__)
#define _string_join_9(s, ...) _string_ref(s), _string_join_8(__VA_ARGS__)
#define _string_join_8(s, ...) _string_ref(s), _string_join_7(__VA_ARGS__)
#define _string_join_7(s, ...) _string_ref(s), _string_join_6(__VA_ARGS__)
#define _string_join_6(s, ...) _string_ref(s), _string_join_5(__VA_ARGS__)
#define _string_join_5(s, ...) _string_ref(s), _string_join_4(__VA_ARGS__)
#define _string_join_4(s, ...) _string_ref(s), _string_join_3(__VA_ARGS__)
#define _string_join_3(s, ...) _string_ref(s), _string_join_2(__VA_ARGS__)
#define _string_join_2(s, ...) _string_ref(s), _string_join_1(__VA_ARGS__)
#define _string_join_1(s) _string_ref(s), (struct String) {} // sentinel

String _string_join(struct String separator, ...);

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
#define string_split(string, delimiter) _string_split(_string_ref(string), delimiter)

Array(String) _string_split(struct String string, char delimiter);

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
#define string_lines(string) _string_lines(_string_ref(string))

Array(String) _string_lines(struct String string);

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
#define string_to_uppercase(string) _string_to_uppercase(_string_ref(string))

String _string_to_uppercase(struct String string);

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
#define string_to_lowercase(string) _string_to_lowercase(_string_ref(string))

String _string_to_lowercase(struct String string);

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

String _string_value_of_bool(bool value);
String _string_value_of_char(char value);
String _string_value_of_int(int value);
String _string_value_of_uint(unsigned int value);
String _string_value_of_long(long value);
String _string_value_of_ulong(unsigned long value);
String _string_value_of_long_long(long long value);
String _string_value_of_ulong_long(unsigned long long value);
String _string_value_of_float(float value);
String _string_value_of_double(double value);
String _string_value_of_long_double(long double value);

#define _dispatch_string_type(string) (_Generic((string),   \
    String:  _string_ref_of_string,                         \
    const char*: _string_ref_of_raw_string,                 \
    char*: _string_ref_of_raw_string,                       \
    int: _char_self,                                        \
    char: _char_self                                        \
)(string))

#define _string_ref(string) (_Generic((string),     \
    String: _string_ref_of_string,                  \
    const char*: _string_ref_of_raw_string,         \
    char*: _string_ref_of_raw_string                \
)(string))

struct String _string_ref_of_string(String string);

struct String _string_ref_of_raw_string(const char* raw_string);

char _char_self(char c);

#endif