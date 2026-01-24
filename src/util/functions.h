#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

/**
 * @brief Generic function pointer types for functional-style operations.
 */

/** @brief Function to consume a value (e.g., for `for_each`) */
typedef void (*Consumer)(void*);

/** @brief Predicate function returning true or false for a given element */
typedef bool (*Predicate)(const void*);

/** @brief Operator function that transforms an element and returns the result */
typedef void* (*Operator)(void*);

/** @brief Comparator function returning negative, zero, or positive value */
typedef int (*Comparator)(const void*, const void*);

/**
 * @brief Check if two characters are equal.
 *
 * @param a Pointer to the first character.
 * @param b Pointer to the second character.
 * @return true if *a == *b, false otherwise.
 */
bool char_equals(const char* a, const char* b);

/**
 * @brief Check if two integers are equal.
 *
 * @param a Pointer to the first integer.
 * @param b Pointer to the second integer.
 * @return true if *a == *b, false otherwise.
 */
bool int_equals(const int* a, const int* b);

/**
 * @brief Check if two long integers are equal.
 *
 * @param a Pointer to the first long integer.
 * @param b Pointer to the second long integer.
 * @return true if *a == *b, false otherwise.
 */
bool long_equals(const long* a, const long* b);

/**
 * @brief Check if two floats are equal.
 *
 * @param a Pointer to the first float.
 * @param b Pointer to the second float.
 * @return true if *a == *b (bitwise equality), false otherwise.
 *
 * @note For floating-point comparison with tolerance, consider implementing an epsilon check.
 */
bool float_equals(const float* a, const float* b);

/**
 * @brief Check if two doubles are equal.
 *
 * @param a Pointer to the first double.
 * @param b Pointer to the second double.
 * @return true if *a == *b (bitwise equality), false otherwise.
 *
 * @note For floating-point comparison with tolerance, consider implementing an epsilon check.
 */
bool double_equals(const double* a, const double* b);

/**
 * @brief Check if two pointers are equal.
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @return true if a == b, false otherwise.
 */
bool pointer_equals(const void* a, const void* b);

/**
 * @brief Check if two C strings are equal.
 *
 * @param a Pointer to the first null-terminated string.
 * @param b Pointer to the second null-terminated string.
 * @return true if strings are identical, false otherwise.
 *
 * @note Compares character by character until a null terminator is reached.
 */
bool string_equals(const char* a, const char* b);

/**
 * @brief Compare two characters.
 *
 * @param a Pointer to the first character.
 * @param b Pointer to the second character.
 * @return Negative if *a < *b, 0 if *a == *b, positive if *a > *b.
 */
int compare_chars(const char* a, const char* b);

/**
 * @brief Compare two integers.
 *
 * @param a Pointer to the first integer.
 * @param b Pointer to the second integer.
 * @return Negative if *a < *b, 0 if *a == *b, positive if *a > *b.
 */
int compare_ints(const int* a, const int* b);

/**
 * @brief Compare two long integers.
 *
 * @param a Pointer to the first long integer.
 * @param b Pointer to the second long integer.
 * @return Negative if *a < *b, 0 if *a == *b, positive if *a > *b.
 */
int compare_longs(const long* a, const long* b);

/**
 * @brief Compare two floats.
 *
 * @param a Pointer to the first float.
 * @param b Pointer to the second float.
 * @return Negative if *a < *b, 0 if *a == *b, positive if *a > *b.
 *
 * @note Direct comparison may be affected by floating-point precision issues.
 */
int compare_floats(const float* a, const float* b);

/**
 * @brief Compare two doubles.
 *
 * @param a Pointer to the first double.
 * @param b Pointer to the second double.
 * @return Negative if *a < *b, 0 if *a == *b, positive if *a > *b.
 *
 * @note Direct comparison may be affected by floating-point precision issues.
 */
int compare_doubles(const double* a, const double* b);

/**
 * @brief Compare two pointers.
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @return Negative if a < b, 0 if a == b, positive if a > b (compares addresses).
 */
int compare_pointers(const void* a, const void* b);

/**
 * @brief Compare two C strings.
 *
 * @param a Pointer to the first null-terminated string.
 * @param b Pointer to the second null-terminated string.
 * @return Negative if a < b, 0 if a == b, positive if a > b (lexicographic comparison).
 */
int compare_strings(const char* a, const char* b);

/**
 * @brief Convert a character to string.
 *
 * @param value Pointer to the character.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int char_to_string(const char* value, char* buffer, size_t size);

/**
 * @brief Convert an integer to string.
 *
 * @param value Pointer to the integer.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int int_to_string(const int* value, char* buffer, size_t size);

/**
 * @brief Convert a long integer to string.
 *
 * @param value Pointer to the long integer.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int long_to_string(const long* value, char* buffer, size_t size);

/**
 * @brief Convert a float to string.
 *
 * @param value Pointer to the float.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int float_to_string(const float* value, char* buffer, size_t size);

/**
 * @brief Convert a double to string.
 *
 * @param value Pointer to the double.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int double_to_string(const double* value, char* buffer, size_t size);

/**
 * @brief Convert a pointer to string (implementation-defined format).
 *
 * @param value Pointer to convert.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int pointer_to_string(const void* value, char* buffer, size_t size);

/**
 * @brief Copy a C string to a buffer.
 *
 * @param value Pointer to the source null-terminated string.
 * @param buffer Destination buffer.
 * @param size Size of the buffer in bytes.
 * @return Number of characters written (excluding null terminator) or negative on error.
 */
int string_to_string(const char* value, char* buffer, size_t size);

#endif