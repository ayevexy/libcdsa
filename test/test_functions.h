#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <stddef.h>
#include <stdio.h>

static inline bool int_pointer_value_equals(const void* a, const void* b) {
    return *(int*) a == *(int*) b;
}

static inline int int_pointer_value_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%d", *(int*) value);
}

static inline int compare_int_pointers_value(const void* a, const void* b) {
    return (*(int*) a > *(int*) b) - (*(int*) a < *(int*) b);
}

#endif