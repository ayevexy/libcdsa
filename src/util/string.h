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
 * @brief String type abstraction over c-strings. The String type is immutable
 * and all operations that would modify a string return a new modified version instead.
 * Accessing its fields directly is discouraged.
 *
 * Memory ownership:
 * - String must be freed using string_destroy(), except for strings created by `string_ref()`.
 */
typedef struct String {
    int length;
    const char* data;
    char buffer[]; // internal
} const * String;

/**
 * @brief Creates a new allocated string copying another string.
 *
 * @param string source string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_new(string) string_new(dispatch_string_type(string))

String (string_new)(struct String string);

/**
 * @brief Creates a string that references an existing c-string.
 *
 * @param string the string
 *
 * @return a string reference
 */
#define string_ref(string) \
    (&(struct String) { .length = strlen(string), .data = string })

/**
 * @brief Destroys previously allocated strings.
 *
 * @param ... the strings
 *
 * @post &strings[i] == nullptr
 *
 * @exception NULL_POINTER_ERROR if &strings[i] || strings[i] is null
 */
#define string_destroy(...) (dispatch_string_destroy(__VA_ARGS__,   \
    string_destroy_10,                                              \
    string_destroy_9,                                               \
    string_destroy_8,                                               \
    string_destroy_7,                                               \
    string_destroy_6,                                               \
    string_destroy_5,                                               \
    string_destroy_4,                                               \
    string_destroy_3,                                               \
    string_destroy_2,                                               \
    string_destroy_1                                                \
)(__VA_ARGS__))

#define dispatch_string_destroy(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME

#define string_destroy_10(string, ...) string_destroy(string) string_destroy_9(__VA_ARGS__)
#define string_destroy_9(string, ...) string_destroy(string), string_destroy_8(__VA_ARGS__)
#define string_destroy_8(string, ...) string_destroy(string), string_destroy_7(__VA_ARGS__)
#define string_destroy_7(string, ...) string_destroy(string), string_destroy_6(__VA_ARGS__)
#define string_destroy_6(string, ...) string_destroy(string), string_destroy_5(__VA_ARGS__)
#define string_destroy_5(string, ...) string_destroy(string), string_destroy_4(__VA_ARGS__)
#define string_destroy_4(string, ...) string_destroy(string), string_destroy_3(__VA_ARGS__)
#define string_destroy_3(string, ...) string_destroy(string), string_destroy_2(__VA_ARGS__)
#define string_destroy_2(string, ...) string_destroy(string), string_destroy_1(__VA_ARGS__)
#define string_destroy_1(string) string_destroy(string)

void (string_destroy)(String* string_pointer);

/**
 * @brief Creates a newly allocated formatted string.
 *
 * @param format the format
 * @param ... arguments
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if format.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_format(format, ...) string_format(dispatch_string_type(format), __VA_ARGS__)

String (string_format)(struct String format, ...);

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
#define string_char_at(string, index) string_char_at(dispatch_string_type(string), index)

char (string_char_at)(struct String string, int index);

/**
 * @brief Retrieves the length of a string.
 *
 * @param string the string
 *
 * @return the length
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_length(string) string_length(dispatch_string_type(string))

int (string_length)(struct String string);

/**
 * @brief Retrieves the data of a string.
 *
 * @param string the string
 *
 * @return the data
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_data(string) string_data(dispatch_string_type(string))

const char* (string_data)(struct String string);

/**
 * @brief Checks whether the string is empty.
 *
 * @param string the string
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_empty(string) string_is_empty(dispatch_string_type(string))

bool (string_is_empty)(struct String string);

/**
 * @brief Checks whether the string is blank.
 *
 * @param string the string
 *
 * @return true if blank, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_blank(string) string_is_blank(dispatch_string_type(string))

bool (string_is_blank)(struct String string);

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
#define string_compare(string, other_string) string_compare(dispatch_string_type(string), dispatch_string_type(other_string))

int (string_compare)(struct String string, struct String other_string);

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
#define string_compare_ignore_case(string, other_string) string_compare_ignore_case(dispatch_string_type(string), dispatch_string_type(other_string))

int (string_compare_ignore_case)(struct String string, struct String other_string);

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
#define string_equals(string, other_string) string_equals(dispatch_string_type(string), dispatch_string_type(other_string))

bool (string_equals)(struct String string, struct String other_string);

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
#define string_equals_ignore_case(string, other_string) string_equals_ignore_case(dispatch_string_type(string), dispatch_string_type(other_string))

bool (string_equals_ignore_case)(struct String string, struct String other_string);

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
#define string_index_of(string, needle) _Generic((needle),      \
    char: string_index_of_char,                                 \
    int:  string_index_of_char,                                 \
    default: string_index_of_substring                          \
)(dispatch_string_type(string), dispatch_string_type(needle))

int string_index_of_char(struct String string, char character);

int string_index_of_substring(struct String string, struct String substring);

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
#define string_last_index_of(string, needle) _Generic((needle),     \
    char: string_last_index_of_char,                                \
    int:  string_last_index_of_char,                                \
    default: string_last_index_of_substring                         \
)(dispatch_string_type(string), dispatch_string_type(needle))

int string_last_index_of_char(struct String string, char character);

int string_last_index_of_substring(struct String string, struct String substring);

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
#define string_contains(string, substring) string_contains(dispatch_string_type(string), dispatch_string_type(substring))

bool (string_contains)(struct String string, struct String substring);

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
#define string_starts_with(string, prefix) string_starts_with(dispatch_string_type(string), dispatch_string_type(prefix))

bool (string_starts_with)(struct String string, struct String prefix);

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
#define string_ends_with(string, prefix) string_ends_with(dispatch_string_type(string), dispatch_string_type(prefix))

bool (string_ends_with)(struct String string, struct String suffix);

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
#define string_trim(string) string_trim(dispatch_string_type(string))

String (string_trim)(struct String string);

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
#define string_trim_start(string) string_trim_start(dispatch_string_type(string))

String (string_trim_start)(struct String string);

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
#define string_trim_end(string) string_trim_end(dispatch_string_type(string))

String (string_trim_end)(struct String string);

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
#define string_substring(string, start, length) string_substring(dispatch_string_type(string), start, length)

String (string_substring)(struct String string, int start, int length);

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
#define string_concat(string, other_string) string_concat(dispatch_string_type(string), dispatch_string_type(other_string))

String (string_concat)(struct String string, struct String other_string);

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
#define string_replace(string, target, replacement) _Generic((target),                              \
    char: string_replace_char,                                                                      \
    int: string_replace_char,                                                                       \
    default: string_replace_substring                                                               \
)(dispatch_string_type(string), dispatch_string_type(target), dispatch_string_type(replacement))

String string_replace_char(struct String string, char character, char replacement);

String string_replace_substring(struct String string, struct String target, struct String replacement);

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
#define string_repeat(string, times) string_repeat(dispatch_string_type(string), times)

String (string_repeat)(struct String string, int times);

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
#define string_join(separator, ...) string_join(dispatch_string_type(separator), dispatch_string_join(__VA_ARGS__,  \
    string_join_20,                                                                                                 \
    string_join_19,                                                                                                 \
    string_join_18,                                                                                                 \
    string_join_17,                                                                                                 \
    string_join_16,                                                                                                 \
    string_join_15,                                                                                                 \
    string_join_14,                                                                                                 \
    string_join_13,                                                                                                 \
    string_join_12,                                                                                                 \
    string_join_11,                                                                                                 \
    string_join_10,                                                                                                 \
    string_join_9,                                                                                                  \
    string_join_8,                                                                                                  \
    string_join_7,                                                                                                  \
    string_join_6,                                                                                                  \
    string_join_5,                                                                                                  \
    string_join_4,                                                                                                  \
    string_join_3,                                                                                                  \
    string_join_2,                                                                                                  \
    string_join_1                                                                                                   \
)(__VA_ARGS__))

#define dispatch_string_join(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, NAME, ...) NAME

#define string_join_20(s, ...) dispatch_string_type(s), string_join_19(__VA_ARGS__)
#define string_join_19(s, ...) dispatch_string_type(s), string_join_18(__VA_ARGS__)
#define string_join_18(s, ...) dispatch_string_type(s), string_join_17(__VA_ARGS__)
#define string_join_17(s, ...) dispatch_string_type(s), string_join_16(__VA_ARGS__)
#define string_join_16(s, ...) dispatch_string_type(s), string_join_15(__VA_ARGS__)
#define string_join_15(s, ...) dispatch_string_type(s), string_join_14(__VA_ARGS__)
#define string_join_14(s, ...) dispatch_string_type(s), string_join_13(__VA_ARGS__)
#define string_join_13(s, ...) dispatch_string_type(s), string_join_12(__VA_ARGS__)
#define string_join_12(s, ...) dispatch_string_type(s), string_join_11(__VA_ARGS__)
#define string_join_11(s, ...) dispatch_string_type(s), string_join_10(__VA_ARGS__)
#define string_join_10(s, ...) dispatch_string_type(s), string_join_9(__VA_ARGS__)
#define string_join_9(s, ...) dispatch_string_type(s), string_join_8(__VA_ARGS__)
#define string_join_8(s, ...) dispatch_string_type(s), string_join_7(__VA_ARGS__)
#define string_join_7(s, ...) dispatch_string_type(s), string_join_6(__VA_ARGS__)
#define string_join_6(s, ...) dispatch_string_type(s), string_join_5(__VA_ARGS__)
#define string_join_5(s, ...) dispatch_string_type(s), string_join_4(__VA_ARGS__)
#define string_join_4(s, ...) dispatch_string_type(s), string_join_3(__VA_ARGS__)
#define string_join_3(s, ...) dispatch_string_type(s), string_join_2(__VA_ARGS__)
#define string_join_2(s, ...) dispatch_string_type(s), string_join_1(__VA_ARGS__)
#define string_join_1(s) dispatch_string_type(s), (struct String) {} // sentinel

String (string_join)(struct String separator, ...);

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
#define string_split(string, delimiter) string_split(dispatch_string_type(string), delimiter)

Array(String) (string_split)(struct String string, char delimiter);

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
#define string_lines(string) string_lines(dispatch_string_type(string))

Array(String) (string_lines)(struct String string);

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
#define string_to_uppercase(string) string_to_uppercase(dispatch_string_type(string))

String (string_to_uppercase)(struct String string);

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
#define string_to_lowercase(string) string_to_lowercase(dispatch_string_type(string))

String (string_to_lowercase)(struct String string);

/**
 * @brief Converts a primitive value to its string representation.
 *
 * @param value the value
 *
 * @return a new allocated string
 */
#define string_value_of(value) _Generic((value),        \
    bool: string_value_of_bool,                         \
    char: string_value_of_char,                         \
    signed char: string_value_of_int,                   \
    unsigned char: string_value_of_uint,                \
    short: string_value_of_int,                         \
    unsigned short: string_value_of_uint,               \
    int: string_value_of_int,                           \
    unsigned int: string_value_of_uint,                 \
    long: string_value_of_long,                         \
    unsigned long: string_value_of_ulong,               \
    long long: string_value_of_long_long,               \
    unsigned long long: string_value_of_ulong_long,     \
    float: string_value_of_float,                       \
    double: string_value_of_double,                     \
    long double: string_value_of_long_double            \
)(value)

String string_value_of_bool(bool value);
String string_value_of_char(char value);
String string_value_of_int(int value);
String string_value_of_uint(unsigned int value);
String string_value_of_long(long value);
String string_value_of_ulong(unsigned long value);
String string_value_of_long_long(long long value);
String string_value_of_ulong_long(unsigned long long value);
String string_value_of_float(float value);
String string_value_of_double(double value);
String string_value_of_long_double(long double value);

/**
 * @brief String hash function to be used in containers.
 *
 * @param raw_string the string
 *
 * @return the hashed value
 */
uint64_t string_hash_callback(const void* raw_string);

/**
 * @brief String equality function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string]
 *
 * @return true if equal, false otherwise
 */
bool string_equals_callback(const void* string, const void* other_string);

/**
 * @brief String comparator function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string
 *
 * @return 0 if equal; negative if the first string is less; positive if greater
 */
int string_compare_callback(const void* string, const void* other_string);

/**
 * @brief String destructor function to be used in containers.
 *
 * @param string the string
 */
void string_destroy_callback(void* string);

/**
 * @brief String to string function to be used in containers.
 *
 * @param string the string
 * @param buffer buffer
 * @param size size
 *
 * @return the string length
 */
int string_to_string_callback(const void* string, char* buffer, size_t size);

/* --------------------------------------------------------------------------------- */

#define dispatch_string_type(string) (_Generic((string),    \
    String: string_self,                                    \
    const char*: string_ref,                                \
    char*: string_ref,                                      \
    int: char_self,                                         \
    char: char_self                                         \
)(string))

struct String string_self(String string);

struct String (string_ref)(const char* raw_string);

char char_self(char c);

#endif