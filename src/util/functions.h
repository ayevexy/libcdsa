#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

typedef void (*Consumer)(void*);

typedef bool (*Predicate)(const void*);

typedef void* (*UnaryOperator)(void*);

typedef int (*Comparator)(const void*, const void*);

typedef struct string string;

#define DEFAULT_EQUALS(T) _Generic((T*) 0,  \
    char*: char_equals,                     \
    int*: int_equals,                       \
    long*: long_equals,                     \
    float*: float_equals,                   \
    double*: double_equals,                 \
    void*: pointer_equals,                  \
    string*: string_equals                  \
)

bool char_equals(const void*, const void*);

bool int_equals(const void*, const void*);

bool long_equals(const void*, const void*);

bool float_equals(const void*, const void*);

bool double_equals(const void*, const void*);

bool pointer_equals(const void*, const void*);

bool string_equals(const void*, const void*);

#define DEFAULT_COMPARATOR(T) _Generic((T*) 0,  \
    char*: compare_chars,                       \
    int*: compare_ints,                         \
    long*: compare_longs,                       \
    float*: compare_floats,                     \
    double*: compare_doubles,                   \
    void*: compare_pointers,                    \
    string*: compare_strings                    \
)

int compare_chars(const void*, const void*);

int compare_ints(const void*, const void*);

int compare_longs(const void*, const void*);

int compare_floats(const void*, const void*);

int compare_doubles(const void*, const void*);

int compare_pointers(const void*, const void*);

int compare_strings(const void*, const void*);

#define DEFAULT_TO_STRING(T) _Generic((T*) 0,   \
    char*: char_to_string,                      \
    int*: int_to_string,                        \
    long*: long_to_string,                      \
    float*: float_to_string,                    \
    double*: double_to_string,                  \
    void*: pointer_to_string,                   \
    string*: string_to_string                   \
)

int char_to_string(const void*, char*, size_t);

int int_to_string(const void*, char*, size_t);

int long_to_string(const void*, char*, size_t);

int float_to_string(const void*, char*, size_t);

int double_to_string(const void*, char*, size_t);

int pointer_to_string(const void*, char*, size_t);

int string_to_string(const void*, char*, size_t);

#endif