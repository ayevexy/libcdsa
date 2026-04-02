#ifndef LIBCDSA_QUEUE_H
#define LIBCDSA_QUEUE_H

#include "deque/deque.h"

/**
 * A queue is a generic FIFO (First-In-First-Out) data structure built on top of a deque.
 * It allows insertion of elements at the end (enqueue) and removal from the front (dequeue),
 * while maintaining the order of elements. Internally, a queue is simply a type alias for a deque.
 */
typedef Deque Queue;

/**
 * Queue configuration structure. Alias of DequeOptions.
 *
 * Use QueueOptions to configure:
 * - initial capacity
 * - destruct function
 * - equals function
 * - to_string function
 * - memory allocation functions
 */
typedef DequeOptions QueueOptions;

/**
 * @brief Utility macro providing default QueueOptions.
 *
 * Alias of DEFAULT_DEQUE_OPTIONS. Optional overrides can be provided.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_QUEUE_OPTIONS DEFAULT_DEQUE_OPTIONS

/**
 * @brief Creates a new empty queue using the specified options.
 *
 * @param options pointer to a QueueOptions structure
 *
 * @return pointer to a newly created queue
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Queue* queue_new(const QueueOptions* options) {
    return deque_new(options);
}

/**
 * @brief Creates a new queue containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created queue
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
static inline Queue* queue_from(Collection collection, const QueueOptions* options) {
    return deque_from(collection, options);
}

/**
 * @brief Destroys a queue and optionally its elements.
 *
 * @param queue_pointer pointer to a queue pointer
 *
 * @exception NULL_POINTER_ERROR if queue_pointer or *queue_pointer is null
 *
 * @post *queue_pointer == nullptr
 */
static inline void queue_destroy(Queue** queue_pointer) {
    deque_destroy(queue_pointer);
}

/**
 * @brief Sets the element destruct function.
 *
 * @param queue pointer to a queue
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if queue or destruct is null
 */
static inline void queue_set_destructor(Queue* queue, void (*destruct)(void*)) {
    deque_set_destructor(queue, destruct);
}

/**
 * @brief Adds an element to the end of the queue.
 *
 * @param queue pointer to a queue
 * @param element element to enqueue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
static inline void queue_enqueue(Queue* queue, const void* element) {
    deque_add_last(queue, element);
}

/**
 * @brief Adds all elements of a collection to the end of the queue.
 *
 * @param queue pointer to a queue
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
static inline void queue_enqueue_all(Queue* queue, Collection collection) {
    deque_add_all_last(queue, collection);
}

/**
 * @brief Retrieves the first element of the queue without removing it.
 *
 * @param queue pointer to a queue
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception NO_SUCH_ELEMENT_ERROR if queue is empty
 */
static inline void* queue_peek(const Queue* queue) {
    return deque_get_first(queue);
}

/**
 * @brief Removes and returns the first element of the queue.
 *
 * @param queue pointer to a queue
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception NO_SUCH_ELEMENT_ERROR if queue is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
static inline void* queue_dequeue(Queue* queue) {
    return deque_remove_first(queue);
}

/**
 * @brief Returns the current number of elements in the queue.
 *
 * @param queue pointer to a queue
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline int queue_size(const Queue* queue) {
    return deque_size(queue);
}

/**
 * @brief Returns the current capacity of the queue.
 *
 * @param queue pointer to a queue
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline int queue_capacity(const Queue* queue) {
    return deque_capacity(queue);
}

/**
 * @brief Checks whether the queue is empty.
 *
 * @param queue pointer to a queue
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline bool queue_is_empty(const Queue* queue) {
    return deque_is_empty(queue);
}

/**
 * @brief Creates an iterator for the queue.
 *
 * @param queue pointer to a queue
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add(), set(), and remove()
 */
static inline Iterator* queue_iterator(const Queue* queue) {
    return deque_iterator(queue);
}

/**
 * @brief Checks whether two queues are equal.
 *
 * Two queues are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first queue
 *
 * @param queue first queue
 * @param other_queue second queue
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either queue is null
 */
static inline bool queue_equals(const Queue* queue, const Queue* other_queue) {
    return deque_equals(queue, other_queue);
}

/**
 * @brief Applies an action to each element of the queue.
 *
 * @param queue pointer to a queue
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if queue or action is null
 */
static inline void queue_for_each(Queue* queue, Consumer action) {
    deque_for_each(queue, action);
}

/**
 * @brief Removes all elements of the queue.
 *
 * @param queue pointer to a queue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 *
 * @note this function calls the element destruct
 */
static inline void queue_clear(Queue* queue) {
    deque_clear(queue);
}

/**
 * @brief Checks whether an element is present in the queue.
 *
 * @param queue pointer to a queue
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline bool queue_contains(const Queue* queue, const void* element) {
    return deque_contains(queue, element);
}

/**
 * @brief Checks whether all elements of a collection are present in the queue.
 *
 * @param queue pointer to a queue
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
static inline bool queue_contains_all(const Queue* queue, Collection collection) {
    return deque_contains_all(queue, collection);
}

/**
 * @brief Creates a shallow copy of the queue.
 *
 * The new queue shares element pointers but has independent storage.
 *
 * @param queue pointer to a queue
 *
 * @return pointer to the newly created queue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Queue* queue_clone(const Queue* queue) {
    return deque_clone(queue);
}

/**
 * @brief Returns a Collection view of the queue.
 *
 * @param queue pointer to a queue
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline Collection queue_to_collection(const Queue* queue) {
    return deque_to_collection(queue);
}

/**
 * @brief Returns a newly allocated array containing all elements of the queue.
 *
 * @param queue pointer to a queue
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
static inline void** queue_to_array(const Queue* queue) {
    return deque_to_array(queue);
}

/**
 * @brief Converts the queue to a string representation.
 *
 * @param queue pointer to a queue
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
static inline char* queue_to_string(const Queue* queue) {
    return deque_to_string(queue);
}

#endif