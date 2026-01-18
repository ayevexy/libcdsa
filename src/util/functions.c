#include "functions.h"

#include <stdio.h>
#include <string.h>

#define DEFINE_EQUALS(T) bool T##_equals(const T* a, const T* b) {  \
    return *a == *b;                                                \
}

DEFINE_EQUALS(char)
DEFINE_EQUALS(int)
DEFINE_EQUALS(long)
DEFINE_EQUALS(float)
DEFINE_EQUALS(double)

#undef DEFINE_EQUALS

bool pointer_equals(const void* a, const void* b) {
    return a == b;
}

bool string_equals(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

#define DEFINE_COMPARATOR(T) int compare_##T##s(const T* a, const T* b) {   \
    return (*a > *b) - (*a < *b);                                           \
}

DEFINE_COMPARATOR(char)
DEFINE_COMPARATOR(int)
DEFINE_COMPARATOR(long)
DEFINE_COMPARATOR(float)
DEFINE_COMPARATOR(double)

#undef DEFINE_COMPARATOR

int compare_pointers(const void* a, const void* b) {
    return (a > b) - (a < b);
}

int compare_strings(const char* a, const char* b) {
    return strcmp(a, b);
}

#define DEFINE_TO_STRING(T, format) int T##_to_string(const T* e, char* string, size_t length) {    \
    return snprintf(string, length, format, *e);                                                    \
}

DEFINE_TO_STRING(char, "%c")
DEFINE_TO_STRING(int, "%d")
DEFINE_TO_STRING(long, "%ld")
DEFINE_TO_STRING(float, "%f")
DEFINE_TO_STRING(double, "%lf")

#undef DEFINE_TO_STRING

int pointer_to_string(const void* e, char* string, size_t length) {
    return snprintf(string, length, "%p", e);
}

int string_to_string(const char* e, char* string, size_t length) {
    return snprintf(string, length, "%s", e);
}