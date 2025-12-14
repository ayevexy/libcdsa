#include "functions.h"

#include <stdio.h>

bool equals(void* a, void* b) {
    return a == b;
}

int compare(void* a, void* b) {
    return (a > b) - (a < b);
}

char* to_string(void* e) {
    constexpr int SIZE = 15;
    static char string[SIZE];
    snprintf(string, SIZE, "%p", e);
    return string;
}