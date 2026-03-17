#ifndef QUEUE_H
#define QUEUE_H

#include "deque/deque.h"

/**
 * A Queue is a generic FIFO (First-In-First-Out) data structure built on top of a Deque.
 * It allows insertion of elements at the end (enqueue) and removal from the front (dequeue),
 * while maintaining the order of elements.
 *
 * Internally, a Queue is simply a type alias for a Deque and inherits all its dynamic resizing
 * and generic type capabilities.
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
 * @brief Provides a reasonable default QueueOptions configuration.
 *
 * Alias of DEFAULT_DEQUE_OPTIONS. Optional overrides can be provided.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_QUEUE_OPTIONS DEFAULT_DEQUE_OPTIONS

/**
 * @brief Creates a new empty Queue using the specified options.
 *
 * @param options pointer to a QueueOptions defining the queue configuration
 *
 * @return pointer to a newly created Queue on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Queue* queue_new(const QueueOptions* options) {
    return deque_new(options);
}

/**
 * @brief Creates a new Queue containing all elements of the given collection.
 *
 * @param collection a Collection containing elements to add
 * @param options pointer to a QueueOptions defining configuration
 *
 * @return pointer to a newly created Queue on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Queue* queue_from(Collection collection, const QueueOptions* options) {
    return deque_from(collection, options);
}

/**
 * @brief Destroys an existing Queue and optionally its elements using the destruct function.
 *
 * @param queue pointer to a Queue pointer
 *
 * @post *queue == nullptr
 *
 * @exception NULL_POINTER_ERROR if queue or *queue is null
 */
static inline void queue_destroy(Queue** queue) {
    deque_destroy(queue);
}

/**
 * @brief Sets the destructor function used for elements in the Queue.
 *
 * @param queue pointer to a Queue
 * @param destructor function to free elements
 */
static inline void queue_set_destructor(Queue* queue, void (*destructor)(void*)) {
    deque_set_destructor(queue, destructor);
}

/**
 * @brief Adds an element to the end of the Queue (enqueue).
 *
 * @param queue pointer to a Queue
 * @param element pointer to the element to enqueue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
static inline void queue_enqueue(Queue* queue, const void* element) {
    deque_add_last(queue, element);
}

/**
 * @brief Adds all elements of a collection to the end of the Queue.
 *
 * @param queue pointer to a Queue
 * @param collection Collection of elements to enqueue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if iterator allocation fails
 */
static inline void queue_enqueue_all(Queue* queue, Collection collection) {
    deque_add_all_last(queue, collection);
}

/**
 * @brief Retrieves the first element of the Queue without removing it.
 *
 * @param queue pointer to a Queue
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception NO_SUCH_ELEMENT_ERROR if the queue is empty
 */
static inline void* queue_peek(const Queue* queue) {
    return deque_get_first(queue);
}

/**
 * @brief Removes and returns the first element of the Queue (dequeue).
 *
 * @param queue pointer to a Queue
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception NO_SUCH_ELEMENT_ERROR if the queue is empty
 */
static inline void* queue_dequeue(Queue* queue) {
    return deque_remove_first(queue);
}

/**
 * @brief Returns the current number of elements in the Queue.
 *
 * @param queue pointer to a Queue
 *
 * @return number of elements
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline int queue_size(const Queue* queue) {
    return deque_size(queue);
}

/**
 * @brief Returns the current capacity of the Queue.
 *
 * @param queue pointer to a Queue
 *
 * @return capacity of the queue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline int queue_capacity(const Queue* queue) {
    return deque_capacity(queue);
}

/**
 * @brief Checks whether the Queue is empty.
 *
 * @param queue pointer to a Queue
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline bool queue_is_empty(const Queue* queue) {
    return deque_is_empty(queue);
}

/**
 * @brief Returns an iterator over the elements in the Queue.
 *
 * @param queue pointer to a Queue
 *
 * @return pointer to an Iterator
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline Iterator* queue_iterator(const Queue* queue) {
    return deque_iterator(queue);
}

/**
 * @brief Checks whether two Queues are equal.
 *
 * Two queues are equal if they have the same size and all elements are equal
 * according to the equals function of the first queue.
 *
 * @param queue pointer to a Queue
 * @param other pointer to another Queue
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue or other is null
 */
static inline bool queue_equals(const Queue* queue, const Queue* other) {
    return deque_equals(queue, other);
}

/**
 * @brief Performs an action for each element of the Queue.
 *
 * @param queue pointer to a Queue
 * @param action function to apply to each element
 *
 * @exception NULL_POINTER_ERROR if queue or action is null
 */
static inline void queue_for_each(Queue* queue, Consumer action) {
    deque_for_each(queue, action);
}

/**
 * @brief Removes all elements from the Queue, optionally destructing them.
 *
 * @param queue pointer to a Queue
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline void queue_clear(Queue* queue) {
    deque_clear(queue);
}

/**
 * @brief Checks whether the Queue contains the specified element.
 *
 * @param queue pointer to a Queue
 * @param element pointer to the element to search
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline bool queue_contains(const Queue* queue, const void* element) {
    return deque_contains(queue, element);
}

/**
 * @brief Checks whether the Queue contains all elements of a collection.
 *
 * @param queue pointer to a Queue
 * @param collection Collection of elements to check
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if iterator allocation fails
 */
static inline bool queue_contains_all(const Queue* queue, Collection collection) {
    return deque_contains_all(queue, collection);
}

/**
 * @brief Creates a shallow copy of the Queue.
 *
 * @param queue pointer to a Queue
 *
 * @return pointer to a new Queue containing the same element pointers
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline Queue* queue_clone(const Queue* queue) {
    return deque_clone(queue);
}

/**
 * @brief Returns a Collection view of the Queue.
 *
 * @param queue pointer to a Queue
 *
 * @return Collection representation
 *
 * @exception NULL_POINTER_ERROR if queue is null
 */
static inline Collection queue_to_collection(const Queue* queue) {
    return deque_to_collection(queue);
}

/**
 * @brief Converts the Queue into a newly allocated array.
 *
 * @param queue pointer to a Queue
 *
 * @return newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline void** queue_to_array(const Queue* queue) {
    return deque_to_array(queue);
}

/**
 * @brief Converts the Queue into a string representation.
 *
 * @param queue pointer to a Queue
 *
 * @return newly allocated null-terminated string, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if queue is null
 * @exception MEMORY_ALLOCATION_ERROR if allocation fails
 */
static inline char* queue_to_string(const Queue* queue) {
    return deque_to_string(queue);
}

#endif