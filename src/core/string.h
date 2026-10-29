#ifndef LIBCDSA_STRING_H
#define LIBCDSA_STRING_H

/**
 * @file string.h
 * @brief String implementation.
 */

#include "types.h"
#include "memory.h"
#include "array.h"

/**
 * @brief String memory allocator used for string allocations.
 *
 * @details By default it points to the `global_memory_allocator`.
 */
extern Allocator* string_memory_allocator;

/**
 * @brief String type abstraction over c-strings. The String type is immutable
 * and all operations that would modify a string return a new modified version instead.
 * The String type is opaque.
 *
 * Memory ownership:
 * - String must be freed using `string_destroy()`, except for strings created by `string_static()`.
 */
typedef struct String * String;

/**
 * @brief A non-owning view of a string, intended for use as a function parameter only.
 * The view does not manage the lifetime of the underlying character data.
 */
typedef struct {
    const int length;
    const char* const data;
} StringView;

/**
 * @brief Creates a string view.
 *
 * @param string the string
 *
 * @return the string view
 */
#define string_view(string) (_Generic((string),     \
    String: string_view_from_string,                \
    const char*: string_view_from_raw_string,       \
    char*: string_view_from_raw_string,             \
    int: string_view_ignore_char,                   \
    char: string_view_ignore_char                   \
)(string))

StringView string_view_from_string(String string);

StringView string_view_from_raw_string(const char* raw_string);

char string_view_ignore_char(char c);

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
#define string_new(string) string_new(string_view(string))

String (string_new)(StringView string);

/**
 * @brief Creates a new allocated string that references an existing string.
 *
 * @param string the string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_ref(string) string_ref(string_view(string))

String (string_ref)(StringView string);

/**
 * @brief Returns a static-storage string that references an existing string.
 *
 * @param string the string
 *
 * @return the static-storage string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if the string pool is exhausted
 */
#define string_static(string) string_static(string_view(string))

String (string_static)(StringView string);

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
 * @brief Destroys all previously allocated strings.
 *
 * @exception UNSUPPORTED_OPERATION_ERROR If the current string memory allocator does not support the reset operation
 */
void string_destroy_all(void);

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
#define string_format(format, ...) string_format(string_view(format), __VA_ARGS__)

String (string_format)(StringView format, ...);

/**
 * @brief Calculates the hash value of a string.
 *
 * @param string the string
 *
 * @return the hashed value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_hash(string) string_hash(string_view(string))

uint64 (string_hash)(StringView string);

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
#define string_char_at(string, index) string_char_at(string_view(string), index)

char (string_char_at)(StringView string, int index);

/**
 * @brief Retrieves the length of a string.
 *
 * @param string the string
 *
 * @return the length
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_length(string) string_length(string_view(string))

int (string_length)(StringView string);

/**
 * @brief Retrieves the data of a string.
 *
 * @param string the string
 *
 * @return the data
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_data(string) string_data(string_view(string))

const char* (string_data)(StringView string);

/**
 * @brief Checks whether the string is empty.
 *
 * @param string the string
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_empty(string) string_is_empty(string_view(string))

bool (string_is_empty)(StringView string);

/**
 * @brief Checks whether the string is blank.
 *
 * @param string the string
 *
 * @return true if blank, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
#define string_is_blank(string) string_is_blank(string_view(string))

bool (string_is_blank)(StringView string);

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
#define string_compare(string, other_string) string_compare(string_view(string), string_view(other_string))

int (string_compare)(StringView string, StringView other_string);

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
#define string_compare_ignore_case(string, other_string) string_compare_ignore_case(string_view(string), string_view(other_string))

int (string_compare_ignore_case)(StringView string, StringView other_string);

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
#define string_equals(string, other_string) string_equals(string_view(string), string_view(other_string))

bool (string_equals)(StringView string, StringView other_string);

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
#define string_equals_ignore_case(string, other_string) string_equals_ignore_case(string_view(string), string_view(other_string))

bool (string_equals_ignore_case)(StringView string, StringView other_string);

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
)(string_view(string), string_view(needle))

int string_index_of_char(StringView string, char character);

int string_index_of_substring(StringView string, StringView substring);

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
)(string_view(string), string_view(needle))

int string_last_index_of_char(StringView string, char character);

int string_last_index_of_substring(StringView string, StringView substring);

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
#define string_contains(string, substring) string_contains(string_view(string), string_view(substring))

bool (string_contains)(StringView string, StringView substring);

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
#define string_starts_with(string, prefix) string_starts_with(string_view(string), string_view(prefix))

bool (string_starts_with)(StringView string, StringView prefix);

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
#define string_ends_with(string, prefix) string_ends_with(string_view(string), string_view(prefix))

bool (string_ends_with)(StringView string, StringView suffix);

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
#define string_trim(string) string_trim(string_view(string))

String (string_trim)(StringView string);

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
#define string_trim_start(string) string_trim_start(string_view(string))

String (string_trim_start)(StringView string);

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
#define string_trim_end(string) string_trim_end(string_view(string))

String (string_trim_end)(StringView string);

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
#define string_substring(string, start, length) string_substring(string_view(string), start, length)

String (string_substring)(StringView string, int start, int length);

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
#define string_concat(string, other_string) string_concat(string_view(string), string_view(other_string))

String (string_concat)(StringView string, StringView other_string);

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
)(string_view(string), string_view(target), string_view(replacement))

String string_replace_char(StringView string, char character, char replacement);

String string_replace_substring(StringView string, StringView target, StringView replacement);

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
#define string_repeat(string, times) string_repeat(string_view(string), times)

String (string_repeat)(StringView string, int times);

/**
 * @brief Concatenates n strings repeatedly inserting the specified separator between them.
 *
 * @param separator the separator
 * @param ... a string list (terminated by a null string `(StringView) {}`)
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if separator.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @warning the string list MUST be terminated by a `(StringView) {}` element, otherwise, it's undefined behavior
 *  this macro automatically inserts the null string.
 */
#define string_join(separator, ...) string_join(string_view(separator), dispatch_string_join(__VA_ARGS__,  \
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

#define string_join_20(s, ...) string_view(s), string_join_19(__VA_ARGS__)
#define string_join_19(s, ...) string_view(s), string_join_18(__VA_ARGS__)
#define string_join_18(s, ...) string_view(s), string_join_17(__VA_ARGS__)
#define string_join_17(s, ...) string_view(s), string_join_16(__VA_ARGS__)
#define string_join_16(s, ...) string_view(s), string_join_15(__VA_ARGS__)
#define string_join_15(s, ...) string_view(s), string_join_14(__VA_ARGS__)
#define string_join_14(s, ...) string_view(s), string_join_13(__VA_ARGS__)
#define string_join_13(s, ...) string_view(s), string_join_12(__VA_ARGS__)
#define string_join_12(s, ...) string_view(s), string_join_11(__VA_ARGS__)
#define string_join_11(s, ...) string_view(s), string_join_10(__VA_ARGS__)
#define string_join_10(s, ...) string_view(s), string_join_9(__VA_ARGS__)
#define string_join_9(s, ...) string_view(s), string_join_8(__VA_ARGS__)
#define string_join_8(s, ...) string_view(s), string_join_7(__VA_ARGS__)
#define string_join_7(s, ...) string_view(s), string_join_6(__VA_ARGS__)
#define string_join_6(s, ...) string_view(s), string_join_5(__VA_ARGS__)
#define string_join_5(s, ...) string_view(s), string_join_4(__VA_ARGS__)
#define string_join_4(s, ...) string_view(s), string_join_3(__VA_ARGS__)
#define string_join_3(s, ...) string_view(s), string_join_2(__VA_ARGS__)
#define string_join_2(s, ...) string_view(s), string_join_1(__VA_ARGS__)
#define string_join_1(s) string_view(s), (StringView) {} // sentinel

String (string_join)(StringView separator, ...);

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
#define string_split(string, delimiter) string_split(string_view(string), delimiter)

Array(String) (string_split)(StringView string, char delimiter);

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
#define string_lines(string) string_lines(string_view(string))

Array(String) (string_lines)(StringView string);

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
#define string_to_uppercase(string) string_to_uppercase(string_view(string))

String (string_to_uppercase)(StringView string);

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
#define string_to_lowercase(string) string_to_lowercase(string_view(string))

String (string_to_lowercase)(StringView string);

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
    uchar: string_value_of_uint,                        \
    short: string_value_of_int,                         \
    unsigned short: string_value_of_uint,               \
    int: string_value_of_int,                           \
    uint: string_value_of_uint,                         \
    long: string_value_of_long,                         \
    ulong: string_value_of_ulong,                       \
    long long: string_value_of_long_long,               \
    unsigned long long: string_value_of_ulong_long,     \
    float: string_value_of_float,                       \
    double: string_value_of_double,                     \
    long double: string_value_of_long_double            \
)(value)

String string_value_of_bool(bool value);
String string_value_of_char(char value);
String string_value_of_int(int value);
String string_value_of_uint(uint value);
String string_value_of_long(long value);
String string_value_of_ulong(ulong value);
String string_value_of_long_long(long long value);
String string_value_of_ulong_long(unsigned long long value);
String string_value_of_float(float value);
String string_value_of_double(double value);
String string_value_of_long_double(long double value);

/**
 * @brief Converts a string to a `short`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_short(string) string_to_short(string_view(string))

short (string_to_short)(StringView string);

/**
 * @brief Converts a string to an `ushort`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_ushort(string) string_to_ushort(string_view(string))

ushort (string_to_ushort)(StringView string);

/**
 * @brief Converts a string to an `int`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_int(string) string_to_int(string_view(string))

int (string_to_int)(StringView string);

/**
 * @brief Converts a string to a `uint`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_uint(string) string_to_uint(string_view(string))

uint (string_to_uint)(StringView string);

/**
 * @brief Converts a string to a `long`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_long(string) string_to_long(string_view(string))

long (string_to_long)(StringView string);

/**
 * @brief Converts a string to an `ulong`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_ulong(string) string_to_ulong(string_view(string))

ulong (string_to_ulong)(StringView string);

/**
 * @brief Converts a string to a `long long`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_long_long(string) string_to_long_long(string_view(string))

long long (string_to_long_long)(StringView string);

/**
 * @brief Converts a string to an `unsigned long long`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_ulong_long(string) string_to_ulong_long(string_view(string))

unsigned long long (string_to_ulong_long)(StringView string);

/**
 * @brief Converts a string to a `float`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_float(string) string_to_float(string_view(string))

float (string_to_float)(StringView string);

/**
 * @brief Converts a string to a `double`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_double(string) string_to_double(string_view(string))

double (string_to_double)(StringView string);

/**
 * @brief Converts a string to a `long double`.
 *
 * @param string the string to convert
 *
 * @return the converted value
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception NUMBER_FORMAT_ERROR If the string is invalid or the value is out of range
 */
#define string_to_long_double(string) string_to_long_double(string_view(string))

long double (string_to_long_double)(StringView string);

/**
 * @brief String hash function to be used in containers.
 *
 * @param raw_string the string
 *
 * @return the hashed value
 */
uint64 string_hash_generic(const void* raw_string);

/**
 * @brief String equality function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string]
 *
 * @return true if equal, false otherwise
 */
bool string_equals_generic(const void* string, const void* other_string);

/**
 * @brief String comparator function to be used in containers (case-sensitive).
 *
 * @param string first string
 * @param other_string second string
 *
 * @return 0 if equal; negative if the first string is less; positive if greater
 */
int string_compare_generic(const void* string, const void* other_string);

/**
 * @brief String destructor function to be used in containers.
 *
 * @param string the string
 */
void string_destroy_generic(void* string);

/**
 * @brief String to string function to be used in containers.
 *
 * @param string the string
 *
 * @return the string
 */
String string_to_string_generic(const void* string);

/**
 * @brief The StringBuilder type represent a mutable resizable sequence of characters.
 * It should be used to dynamically create custom strings of variable size.
 * Accessing its fields directly is discouraged.
 *
 * Memory ownership:
 * - StringBuilder must be freed using `string_builder_destroy()`.
 */
typedef struct {
    char* buffer;
    int length;
    int capacity;
} StringBuilder;

/**
 * @brief Creates a new string builder with a default capacity of ten.
 *
 * @return a newly created string builder.
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define string_builder_new() string_builder_init(&(StringBuilder){})

StringBuilder* string_builder_init(StringBuilder* builder);

/**
 * @brief Appends a string at the end of the string builder character sequence.
 *
 * @param builder the string builder
 * @param string the string
 *
 * @exception NULL_POINTER_ERROR if builder is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to resize capacity
 */
#define string_builder_append(builder, string) string_builder_append(builder, string_view(string))

void (string_builder_append)(StringBuilder* builder, StringView string);

/**
 * @brief Inserts a string at the specified position of the string builder character sequence.
 *
 * @param builder the string builder
 * @param index the position
 * @param string the string
 *
 * @exception NULL_POINTER_ERROR if builder is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > builder.capacity
 * @exception MEMORY_ALLOCATION_ERROR if failed to resize string builder capacity
 */
#define string_builder_insert(builder, index, string) string_builder_insert(builder, index, string_view(string))

void (string_builder_insert)(StringBuilder* builder, int index, StringView string);

/**
 * @brief Constructs a string from the string builder character sequence current state.
 *
 * @param builder the string builder
 *
 * @return a new allocated string
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
String string_builder_to_string(StringBuilder* builder);

/**
 * @brief Destroy a string builder and its internal buffer.
 *
 * @param builder pointer to a string builder
 *
 * @post builder == nullptr
 */
#define string_builder_destroy(builder) do { string_builder_close(*builder); *builder = nullptr; } while (false)

void string_builder_close(StringBuilder* builder);

#endif