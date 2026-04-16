#ifndef LIBCDSA_STRING_H
#define LIBCDSA_STRING_H

#include <stddef.h>

/**
 * @brief Internal memory allocation function used internally
 * to allocate StringOwned instances. Defaults to malloc.
 */
extern void* (*strings_memory_alloc)(size_t);

/**
 * @brief Internal memory deallocation function used internally
 * to deallocate StringOwned instances. Defaults to free.
 */
extern void (*strings_memory_dealloc)(void*);

/**
 * @brief String type abstraction over c-strings. The String type is conceptually immutable
 * and all operations that would modify a string return a new modified version instead.
 *
 * They are divided into two distinct subtypes: StringView and StringOwned. String views
 * are just references to other strings while string owned are a distinct objects with their
 * own internal buffer that must be freed manually.
 *
 * Memory ownership:
 * - StringView does NOT own memory and must not be freed.
 * - StringOwned owns its buffer and must be freed using string_destroy().
 * - Functions returning StringView reference existing memory.
 * - Functions returning StringOwned allocate new memory.
 */
typedef struct {
    const char* data;
    int length;
} String;

/** @brief Non-Owning String type that references some raw string */
typedef String StringView;

/** @brief Owned String type, must be freed manually */
typedef String StringOwned;

/**
 * @brief Creates an invalid/null string.
 *
 * This value represents the absence of a string. Most functions do not accept
 * this value and will report NULL_POINTER_ERROR if used.
 *
 * It is also used as a sentinel value in some APIs (e.g., string_join, string_split).
 *
 * @return a null/invalid string (data == nullptr)
 */
static inline String string_null() {
    return (String) { .data = nullptr, .length = 0 };
}

/**
 * @brief Creates an empty string. Unlike string_null(), this represents a valid string.
 *
 * @return an empty string
 */
static inline String string_empty() {
    return (String) { .data = "", .length = 0 };
}

/**
 * @brief Creates a new allocated string copying the raw_string.
 *
 * @param raw_string the raw string
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if raw_string is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
StringOwned string_new(const char* raw_string);

/**
 * @brief Destroys a previously allocated string.
 *
 * @param string pointer to a string
 *
 * @post string.data == nullptr
 *
 * @exception NULL_POINTER_ERROR if string || string.data is null
 */
void string_destroy(StringOwned* string);

/**
 * @brief Creates a view over an existing raw string, referencing it.
 *
 * @param raw_string the raw string
 *
 * @return a string view
 *
 * @exception NULL_POINTER_ERROR if raw_string is null
 */
StringView string_view(const char* raw_string);

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
StringOwned string_format(const char* format, ...);

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
char string_char_at(String string, int index);

/**
 * @brief Checks whether the string is empty.
 *
 * @param string the string
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
bool string_is_empty(String string);

/**
 * @brief Checks whether the string is blank.
 *
 * @param string the string
 *
 * @return true if blank, false otherwise
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
bool string_is_blank(String string);

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
int string_compare(String string, String other_string);

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
int string_compare_ignore_case(String string, String other_string);

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
bool string_equals(String string, String other_string);

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
bool string_equals_ignore_case(String string, String other_string);

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
#define string_index_of(string, needle) \
    _Generic((needle), String: string_index_of_substring, default: string_index_of_char)(string, needle)

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
#define string_last_index_of(string, needle) \
    _Generic((needle), String: string_last_index_of_substring, default: string_last_index_of_char)(string, needle)

/**
 * @brief Retrieves the index of the first occurrence of the specified character in the string.
 *
 * @param string the string
 * @param character character to search
 *
 * @return the character first index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
int string_index_of_char(String string, char character);

/**
 * @brief Retrieves the index of the last occurrence of the specified character in the string.
 *
 * @param string the string
 * @param character character to search
 *
 * @return the character last index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
int string_last_index_of_char(String string, char character);

/**
 * @brief Retrieves the index of the first occurrence of the specified substring in the string.
 *
 * @param string the string
 * @param substring substring to search
 *
 * @return the substring first index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data || substring.data is null
 */
int string_index_of_substring(String string, String substring);

/**
 * @brief Retrieves the index of the last occurrence of the specified substring in the string.
 *
 * @param string the string
 * @param substring substring to search
 *
 * @return the substring last index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if string.data || substring.data is null
 */
int string_last_index_of_substring(String string, String substring);

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
bool string_contains(String string, String substring);

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
bool string_starts_with(String string, String prefix);

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
bool string_ends_with(String string, String suffix);

/**
 * @brief Removes all white spaces from both the beginning and the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
StringView string_trim(String string);

/**
 * @brief Removes all white spaces from the beginning of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
StringView string_trim_start(String string);

/**
 * @brief Removes all white spaces from the end of the string.
 *
 * @param string the string
 *
 * @return a trimmed string view
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 */
StringView string_trim_end(String string);

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
StringView string_substring(String string, int start, int length);

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
StringOwned string_concat(String string, String other_string);

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
#define string_replace(string, target, replacement)  \
    _Generic((target), String: string_replace_substring, default: string_replace_char)(string, target, replacement)

/**
 * Replaces all occurrences of a character in the string with a new one.
 *
 * @param string the string
 * @param character the target character
 * @param replacement the replacement character
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
StringOwned string_replace_char(String string, char character, char replacement);

/**
 * Replaces all occurrences of a substring in the string with a new one.
 *
 * @param string the string
 * @param target the target substring
 * @param replacement the replacement substring
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if string.data || target.data || replacement.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
StringOwned string_replace_substring(String string, String target, String replacement);

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
StringOwned string_repeat(String string, int times);

/**
 * @brief Concatenates n strings repeatedly inserting the specified separator between them.
 *
 * @param separator the separator
 * @param ... a string list (terminated by `string_null()`)
 *
 * @return a new allocated string
 *
 * @exception NULL_POINTER_ERROR if separator.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @warning the string list MUST be terminated by a `string_null()` element, otherwise, it's undefined behavior
 */
StringOwned string_join(String separator, ...);

/**
 * @brief Split a string into n strings by the specified delimiter.
 *
 * @param string the string
 * @param delimiter delimiter character
 *
 * @return an array of string views over the original string (terminated by `string_null()`)
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the returned array must be freed manually. Its elements must not be freed
 */
StringView* string_split(String string, char delimiter);

/**
 * @brief Split a string into multiple lines.
 *
 * @param string the string
 *
 * @return an array of string views over the original string (terminated by `string_null()`)
 *
 * @exception NULL_POINTER_ERROR if string.data is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the returned array must be freed manually. Its elements must not be freed
 */
StringView* string_lines(String string);

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
StringOwned string_to_uppercase(String string);

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
StringOwned string_to_lowercase(String string);

#endif