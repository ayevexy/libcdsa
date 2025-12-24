#include "functions.h"

#include <stdio.h>
#include "internal/memory.h"

bool default_equals(void* a, void* b) {
    return a == b;
}

#define DEFINE_COMPARATOR(T) int T##_compare(void* a, void* b) {    \
    return (*(T*) a > *(T*) b) - (*(T*) a < *(T*) b);               \
}

DEFINE_COMPARATOR(char)
DEFINE_COMPARATOR(int)
DEFINE_COMPARATOR(long)
DEFINE_COMPARATOR(float)
DEFINE_COMPARATOR(double)

#undef DEFINE_COMPARATOR

int default_compare(void* a, void* b) {
    return (a > b) - (a < b);
}

#define DEFINE_TO_STRING(T, format) char* T##_to_string(void* e) {      \
    const int length = snprintf(nullptr, 0, format, *(T*) e) + 1;       \
    char* string = memory_alloc(sizeof(char) * length);                 \
    snprintf(string, length, format, *(T*) e);                          \
    return string;                                                      \
}

DEFINE_TO_STRING(char, "%c")
DEFINE_TO_STRING(int, "%d")
DEFINE_TO_STRING(long, "%ld")
DEFINE_TO_STRING(float, "%f")
DEFINE_TO_STRING(double, "%lf")

#undef DEFINE_TO_STRING

char* default_to_string(void* e) {
    const int length = snprintf(nullptr, 0, "%p", e) + 1;
    char* string = memory_alloc(sizeof(char) * length);
    snprintf(string, length, "%p", e);
    return string;
}