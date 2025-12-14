#include "functions.h"

#include <stdio.h>

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