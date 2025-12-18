#include "functions.h"

#include <stdio.h>

#define DEFINE_COMPARATOR(T) int T##_compare(void* a, void* b) {    \
    return (*(T*) a > *(T*) b) - (*(T*) a < *(T*) b);               \
}

DEFINE_COMPARATOR(char)
DEFINE_COMPARATOR(int)
DEFINE_COMPARATOR(long)
DEFINE_COMPARATOR(float)
DEFINE_COMPARATOR(double)

bool default_equals(void* a, void* b) {
    return a == b;
}

int default_compare(void* a, void* b) {
    return (a > b) - (a < b);
}

char* default_to_string(void* e) {
    constexpr int SIZE = 15;
    static char string[SIZE];
    snprintf(string, SIZE, "%p", e);
    return string;
}