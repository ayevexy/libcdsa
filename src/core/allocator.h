#ifndef LIBCDSA_ALLOCATOR_H
#define LIBCDSA_ALLOCATOR_H

#include "types.h"

/**
 * @brief Allocator abstraction.
 *
 * Provides a flexible and uniform way to manage memory.
 */
typedef struct {
    void* storage;
    void* (*alloc)(bytes, void*);
    void* (*realloc)(void*, bytes, void*);
    void (*dealloc)(void*, void*);
    void (*reset)(void*);
} Allocator;

/**
 * @brief Allocates n bytes of memory using the specified allocator.
 *
 * @param allocator the allocator
 * @param size the size
 *
 * @return pointer to the allocated block
 */
static inline void* allocator_alloc(const Allocator* allocator, bytes size) {
    return allocator->alloc(size, allocator->storage);
}

/**
 * @brief Reallocates n bytes of memory using the specified allocator.
 *
 * @param allocator the allocator
 * @param pointer pointer to the old block
 * @param size the new size
 *
 * @return pointer to the new reallocated block
 */
static inline void* allocator_realloc(const Allocator* allocator, void* pointer, bytes size) {
    return allocator->realloc(pointer, size, allocator->storage);
}

/**
 * @brief Deallocates a block of memory using the specified allocator.
 *
 * @param allocator the allocator
 * @param pointer pointer to the memory block
 */
static inline void allocator_dealloc(const Allocator* allocator, void* pointer) {
    allocator->dealloc(pointer, allocator->storage);
}

/**
 * @brief Resets an allocator to its initial state (optional operation).
 *
 * @param allocator the allocator
 */
static inline void allocator_reset(const Allocator* allocator) {
    allocator->reset(allocator->storage);
}

#endif