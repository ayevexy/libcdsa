#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

typedef void (*Consumer)(void*);

typedef bool (*Predicate)(const void*);

typedef void* (*UnaryOperator)(void*);

typedef int (*Comparator)(const void*, const void*);


bool char_equals(const char*, const char*);

bool int_equals(const int*, const int*);

bool long_equals(const long*, const long*);

bool float_equals(const float*, const float*);

bool double_equals(const double*, const double*);

bool pointer_equals(const void*, const void*);

bool string_equals(const char*, const char*);


int compare_chars(const char*, const char*);

int compare_ints(const int*, const int*);

int compare_longs(const long*, const long*);

int compare_floats(const float*, const float*);

int compare_doubles(const double*, const double*);

int compare_pointers(const void*, const void*);

int compare_strings(const char*, const char*);


int char_to_string(const char*, char*, size_t);

int int_to_string(const int*, char*, size_t);

int long_to_string(const long*, char*, size_t);

int float_to_string(const float*, char*, size_t);

int double_to_string(const double*, char*, size_t);

int pointer_to_string(const void*, char*, size_t);

int string_to_string(const char*, char*, size_t);

#endif