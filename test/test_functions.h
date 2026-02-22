#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

static inline int* int_new(int value) {
    int* ptr = malloc(sizeof(int));
    assert(ptr);
    *ptr = value;
    return ptr;
}

static inline char* char_new(char value) {
    char* ptr = malloc(sizeof(char));
    assert(ptr);
    *ptr = value;
    return ptr;
}

static inline bool int_pointer_value_equals(const void* a, const void* b) {
    return *(int*) a == *(int*) b;
}

static inline bool char_pointer_value_equals(const void* a, const void* b) {
    return *(char*) a == *(char*) b;
}

static inline int int_pointer_value_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%d", *(int*) value);
}

static inline int char_pointer_value_to_string(const void* value, char* buffer, size_t size) {
    return snprintf(buffer, size, "%c", *(char*) value);
}

static inline int compare_int_pointers_value(const void* a, const void* b) {
    return (*(int*) a > *(int*) b) - (*(int*) a < *(int*) b);
}

typedef struct { char key; int value; } CharIntEntry;

static inline uint64_t char_hash(const void *character) {
    return *(const unsigned char *) character % 5;
}

#endif