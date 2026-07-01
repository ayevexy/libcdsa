#ifndef LIBCDSA_MEMORY_H
#define LIBCDSA_MEMORY_H

#include "allocator.h"
#include <stddef.h>

/**
 * @brief Global memory allocator instance used by default for memory management.
 */
extern Allocator global_memory_allocator;

/**
 * @brief Allocate and initialize an object of type `T` on the heap.
 *
 * If no initial value is provided, the object will be initialized with
 * the default values for the type.
 *
 * @param T the type of the object to allocate
 * @param ... optional literal value used to initialize the object
 *
 * @return a pointer to the newly allocated object, or nullptr on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
#define new(T, ...) ((T*) (new)(sizeof(T), &(T){__VA_ARGS__}))

void* (new)(size_t size, const void* source);

/**
 * @brief Deallocate a memory block and set its pointer to nullptr.
 *
 * Frees the memory previously allocated with `new` and
 * assigns `nullptr` to the pointer to avoid dangling references.
 *
 * @param pointer pointer to the memory block
 */
#define delete(pointer) ((delete)(pointer), pointer = nullptr)

void (delete)(void* pointer);

/**
 * @brief Allocate `size` bytes of memory.
 *
 * @param size number of bytes
 *
 * @return pointer to the memory block
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
void* memory_alloc(size_t size);

/**
 * @brief Allocate `size` bytes of memory.
 *
 * @param size number of bytes
 *
 * @return pointer to the memory block, or nullptr on failure
 */
void* memory_try_alloc(size_t size);

/**
 * @brief Reallocate a memory block changing its size.
 *
 * @param pointer pointer to the old memory block
 * @param size the new size in bytes
 *
 * @return pointer to the reallocated memory block
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory reallocation fails
 */
void* memory_realloc(void* pointer, size_t size);

/**
 * @brief Reallocate a memory block changing its size.
 *
 * @param pointer pointer to the old memory block
 * @param size the new size in bytes
 *
 * @return pointer to the reallocated memory block, or nullptr on failure
 */
void* memory_try_realloc(void* pointer, size_t size);

/**
 * @brief Deallocate a memory block.
 *
 * @param pointer pointer to the memory block
 */
void memory_dealloc(void* pointer);

#endif