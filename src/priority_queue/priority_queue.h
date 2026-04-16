#ifndef LIBCDSA_PRIORITY_QUEUE_H
#define LIBCDSA_PRIORITY_QUEUE_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"
#include "util/string.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A priority queue is a generic dynamic linear data structure that stores elements
 * according to their priority in a queue using a comparator function.
 * It automatically resizes to accommodate new elements and supports storing
 * elements of any type, including null pointers.
 *
 * Internally, the implementation uses a dynamically allocated array of `void*` (a binary heap).
 * All operations receive a pointer to the priority queue as their first argument.
 * The PriorityQueue type is opaque and can only be modified through the public API.
 *
 * A priority queue must be configured using a PriorityQueueOptions structure specifying:
 * - the initial capacity
 * - the growth factor
 * - the element comparator function
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct PriorityQueue {
 *      void** elements;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the priority queue does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., priority_queue_dequeue)
 * - the priority queue is cleared (priority_queue_clear)
 * - the priority queue is destroyed (priority_queue_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - priority_queue_enqueue: O(log n)
 * - priority_queue_peek: O(1)
 * - priority_queue_dequeue: O(log n)
 * - priority_queue_contains: O(n)
 */
typedef struct PriorityQueue PriorityQueue;

/**
 * PriorityQueue configuration structure. Defines the behavior and attributes of priority queue.
 *
 * @pre initial_capacity >= 10
 * @pre initial_capacity <= 1'073'741'824
 * @pre growth_factor >= 1.10
 * @pre compare != nullptr
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_dealloc != nullptr
 */
typedef struct {
    int initial_capacity;
    float growth_factor;
    Comparator compare;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_dealloc)(void*);
    };
} PriorityQueueOptions;

/**
 * @brief Utility macro providing default PriorityQueueOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_PRIORITY_QUEUE_OPTIONS(...) &(PriorityQueueOptions) {   \
    .initial_capacity = 10,                                             \
    .growth_factor = 2.0f,                                              \
    .compare = pointer_compare,                                         \
    .destruct = noop_destruct,                                          \
    .equals = pointer_equals,                                           \
    .to_string = pointer_to_string,                                     \
    .memory_alloc = malloc,                                             \
    .memory_dealloc = free,                                             \
    __VA_ARGS__                                                         \
}

/**
 * @brief Creates a new empty priority queue using the specified options.
 *
 * @param options pointer to a PriorityQueueOptions structure
 *
 * @return pointer to a newly created priority queue
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
PriorityQueue* priority_queue_new(const PriorityQueueOptions* options);

/**
 * @brief Creates a new priority queue containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created priority queue
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
PriorityQueue* priority_queue_from(Collection collection, const PriorityQueueOptions* options);

/**
 * @brief Destroys a priority queue and optionally its elements.
 *
 * @param priority_queue_pointer pointer to a priority queue pointer
 *
 * @exception NULL_POINTER_ERROR if priority_queue_pointer or *priority_queue_pointer is null
 *
 * @post *priority_queue_pointer == nullptr
 */
void priority_queue_destroy(PriorityQueue** priority_queue_pointer);

/**
 * @brief Changes the element destruct function.
 *
 * @param priority_queue pointer to a priority queue 
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if priority_queue or destruct is null
 */
void priority_queue_change_destructor(PriorityQueue* priority_queue, void (*destruct)(void*));

/**
 * @brief Adds an element to the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 * @param element element to enqueue
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void priority_queue_enqueue(PriorityQueue* priority_queue, const void* element);

/**
 * @brief Adds all elements of a collection to the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void priority_queue_enqueue_all(PriorityQueue* priority_queue, Collection collection);

/**
 * @brief Retrieves the first element of the priority queue without removing it.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception NO_SUCH_ELEMENT_ERROR if priority_queue is empty
 */
void* priority_queue_peek(const PriorityQueue* priority_queue);

/**
 * @brief Removes and returns the first element of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception NO_SUCH_ELEMENT_ERROR if priority_queue is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* priority_queue_dequeue(PriorityQueue* priority_queue);

/**
 * @brief Returns the current number of elements in the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 */
int priority_queue_size(const PriorityQueue* priority_queue);

/**
 * @brief Returns the current capacity of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 */
int priority_queue_capacity(const PriorityQueue* priority_queue);

/**
 * @brief Checks whether the priority queue is empty.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 */
bool priority_queue_is_empty(const PriorityQueue* priority_queue);

/**
 * @brief Creates an iterator for the priority queue.
 *
 * The iteration order is unspecified.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: has_previous(), previous(), add(), set(), and remove()
 */
Iterator* priority_queue_iterator(const PriorityQueue* priority_queue);

/**
 * @brief Checks whether two priority queues are equal.
 *
 * Two priority queues are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first queue
 *
 * @param priority_queue first priority queue
 * @param other_queue second priority queue
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either priority queue is null
 */
bool priority_queue_equals(const PriorityQueue* priority_queue, const PriorityQueue* other_queue);

/**
 * @brief Applies an action to each element of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if priority_queue or action is null
 *
 * @note the iteration order is unspecified
 */
void priority_queue_for_each(PriorityQueue* priority_queue, Consumer action);

/**
 * @brief Removes all elements of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 *
 * @note this function calls the element destruct
 */
void priority_queue_clear(PriorityQueue* priority_queue);

/**
 * @brief Checks whether an element is present in the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 */
bool priority_queue_contains(const PriorityQueue* priority_queue, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool priority_queue_contains_all(const PriorityQueue* priority_queue, Collection collection);

/**
 * @brief Creates a shallow copy of the priority queue.
 *
 * The new priority queue shares element pointers but has independent storage.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return pointer to the newly created priority queue
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
PriorityQueue* priority_queue_clone(const PriorityQueue* priority_queue);

/**
 * @brief Returns a Collection view of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 */
Collection priority_queue_to_collection(const PriorityQueue* priority_queue);

/**
 * @brief Returns a newly allocated array containing all elements of the priority queue.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 * @note the order of elements in the returned array is unspecified
 */
void** priority_queue_to_array(const PriorityQueue* priority_queue);

/**
 * @brief Converts the priority queue to a string representation.
 *
 * @param priority_queue pointer to a priority queue 
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if priority_queue is null 
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 * @note the order of elements in the string is unspecified
 */
StringOwned priority_queue_to_string(const PriorityQueue* priority_queue);

#endif