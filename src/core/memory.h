#ifndef LIBCDSA_MEMORY_H
#define LIBCDSA_MEMORY_H

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

[[maybe_unused]]
static inline void* allocator_alloc(const Allocator* allocator, bytes size) {
    return allocator->alloc(size, allocator->storage);
}

[[maybe_unused]]
static inline void* allocator_realloc(const Allocator* allocator, void* pointer, bytes size) {
    return allocator->realloc(pointer, size, allocator->storage);
}

[[maybe_unused]]
static inline void allocator_dealloc(const Allocator* allocator, void* pointer) {
    allocator->dealloc(pointer, allocator->storage);
}

[[maybe_unused]]
static inline void allocator_reset(const Allocator* allocator) {
    allocator->reset(allocator->storage);
}

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

void* (new)(bytes size, const void* source);

/**
 * @brief Allocate and initialize an object of type `T` on the stack.
 *
 * If no initial value is provided, the object will be initialized with
 * the default values for the type.
 *
 * @param T the type of the object to allocate
 * @param ... optional literal value used to initialize the object
 *
 * @return a pointer to the newly allocated object
 */
#define ref(T, ...) (&(T){__VA_ARGS__})

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
void* memory_alloc(bytes size);

/**
 * @brief Allocate `size` bytes of memory.
 *
 * @param size number of bytes
 *
 * @return pointer to the memory block, or nullptr on failure
 */
void* memory_try_alloc(bytes size);

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
void* memory_realloc(void* pointer, bytes size);

/**
 * @brief Reallocate a memory block changing its size.
 *
 * @param pointer pointer to the old memory block
 * @param size the new size in bytes
 *
 * @return pointer to the reallocated memory block, or nullptr on failure
 */
void* memory_try_realloc(void* pointer, bytes size);

/**
 * @brief Deallocate a memory block.
 *
 * @param pointer pointer to the memory block
 */
void memory_dealloc(void* pointer);

/**
 * @brief Copies a block of memory.
 *
 * @param source the source memory
 * @param destination the destination memory
 * @param size the number of bytes to copy
 *
 * @return the destination pointer
 */
void* memory_copy(const void* source, void* destination, bytes size);

/**
 * @brief Moves a block of memory.
 *
 * @param source the source memory
 * @param destination the destination memory
 * @param size the number of bytes to move
 *
 * @return the destination pointer
 */
void* memory_move(const void* source, void* destination, bytes size);

/**
 * @brief Sets a block of memory to a value.
 *
 * @param pointer the memory block
 * @param value the value to set
 * @param size the number of bytes to set
 *
 * @return the pointer to the memory block
 */
void* memory_set(void* pointer, int value, bytes size);

/**
 * @brief Compares two blocks of memory.
 *
 * @param first the first memory block
 * @param second the second memory block
 * @param size the number of bytes to compare
 *
 * @return a negative value, zero, or a positive value if the first block is
 * less than, equal to, or greater than the second block, respectively
 */
int memory_compare(const void* first, const void* second, bytes size);

/**
 * @brief Finds a byte in a block of memory.
 *
 * @param pointer the memory block
 * @param value the byte to find
 * @param size the number of bytes to search
 *
 * @return a pointer to the first occurrence of the byte, or nullptr if not found
 */
const void* memory_find(const void* pointer, byte value, bytes size);

#endif