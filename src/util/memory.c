#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

void* memory_alloc(size_t bytes) {
    void* pointer = malloc(bytes);
    if (pointer == nullptr) {
        fprintf(stderr, "Error: Failed to allocate memory, aborting...\n");
        exit(EXIT_FAILURE);
    }
    return pointer;
}

void* memory_realloc(void* pointer, size_t bytes) {
    void* new_pointer = realloc(pointer, bytes);
    if (new_pointer == nullptr) {
        fprintf(stderr, "Error: Failed to realloc memory, aborting...\n");
        exit(EXIT_FAILURE);
    }
    return new_pointer;
}

void memory_free(void** pointer) {
    if (pointer == nullptr) {
        fprintf(stderr, "Error: failed to free memory, aborting...\n");
        exit(EXIT_FAILURE);
    }
    free(*pointer);
    *pointer = nullptr;
}