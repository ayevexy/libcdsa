#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void* memory_alloc(size_t bytes);

void* memory_realloc(void* pointer, size_t bytes);

void memory_free(void** pointer);

#endif