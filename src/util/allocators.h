#ifndef LIBCDSA_ALLOCATORS_H
#define LIBCDSA_ALLOCATORS_H

#include "core/types.h"
#include "core/memory.h"

/**
 * @brief Represents a Memory Arena.
 */
typedef struct MemoryArena MemoryArena;

/**
 * @brief Creates a new memory arena.
 *
 * @param capacity the initial capacity
 *
 * @return the newly created arena, or nullptr on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 */
MemoryArena* memory_arena_new(bytes capacity);

/**-
 * @brief Destroys a memory arena and releases all memory allocated by it.
 *
 * @param arena_pointer pointer to an Arena*
 *
 * @exception NULL_POINTER_ERROR If arena_pointer or *arena_pointer is null
 *
 * @post *arena_pointer == null
 */
void memory_arena_destroy(MemoryArena** arena_pointer);

/**
 * @brief Allocates memory from an arena.
 *
 * @param arena the arena
 * @param size the size
 *
 * @return a pointer to the allocated block, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR If arena is null
 * @exception MEMORY_ALLOCATION_ERROR If the arena does not have enough capacity
 */
void* memory_arena_alloc(MemoryArena* arena, bytes size);

/**
 * @brief Resets an arena.
 *
 * @param arena the arena
 *
 * @exception NULL_POINTER_ERROR If arena is null
 */
void memory_arena_reset(MemoryArena* arena);

/**
 * @brief Returns the arena as an allocator.
 *
 * @param arena the arena
 *
 * @return the arena
 *
 * @exception NULL_POINTER_ERROR if arena is null
 */
Allocator* memory_arena_as_allocator(MemoryArena* arena);

/**
 * @brief Represents a Memory Pool.
 */
typedef struct MemoryPool MemoryPool;

/**
 * @brief Creates a new memory pool.
 *
 * @param block_size the block size
 * @param capacity the capacity (number of blocks)
 *
 * @return the newly created memory pool, or nullptr on failure
 *
 * @exception ILLEGAL_ARGUMENT_ERROR If block_size or capacity is zero
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 */
MemoryPool* memory_pool_new(usize capacity, bytes block_size);

/**-
 * @brief Destroys a memory pool and releases all memory allocated by it.
 *
 * @param pool_pointer pointer to an MemoryPool*
 *
 * @exception NULL_POINTER_ERROR If pool_pointer or *pool_pointer is null
 *
 * @post *pool_pointer == null
 */
void memory_pool_destroy(MemoryPool** pool_pointer);

/**
 * @brief Allocates a block from the pool.
 *
 * @param pool the memory pool
 *
 * @return a pointer to the allocated block, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR If pool is null
 * @exception MEMORY_ALLOCATION_ERROR If the memory pool does not have enough capacity
 */
void* memory_pool_alloc(MemoryPool* pool);

/**
 * @brief Returns a block to the pool.
 *
 * @param pool the memory pool
 * @param pointer a block previously allocated from the pool
 *
 * @exception NULL_POINTER_ERROR If pool or pointer is null
 * @exception ILLEGAL_ARGUMENT_ERROR If the pointer does not belong to the pool
 */
void memory_pool_dealloc(MemoryPool* pool, void* pointer);

/**
 * @brief Returns the memory pool as an allocator.
 *
 * @param pool the memory pool
 *
 * @return the memory pool
 *
 * @exception NULL_POINTER_ERROR if pool is null
 */
Allocator* memory_pool_as_allocator(MemoryPool* pool);

#endif