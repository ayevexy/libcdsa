#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

/**
 * @brief Forward iterator abstraction.
 *
 * Iterator provides a generic mechanism to traverse elements of a data structure
 * without exposing its internal representation.
 *
 * Iterators are stateful and heap-allocated. They must be explicitly deleted
 * using iterator_delete.
 *
 * This API is not thread-safe.
 */
typedef struct Iterator Iterator;

/**
 * @brief Creates a new Iterator.
 *
 * @param internal_state pointer to the iterator's internal state
 * @param has_next function to test for remaining elements
 * @param next function to retrieve the next element
 * @param reset function to reset the iterator
 * @param memory_alloc function used to allocate memory
 * @param memory_free function used to free memory
 *
 * @return pointer to a newly created Iterator, or nullptr on failure
 *
 * @pre internal_state != nullptr
 * @pre has_next != nullptr
 * @pre next != nullptr
 * @pre reset != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
 */
Iterator* iterator_new(
    void* internal_state,
    bool (*has_next)(const void* internal_state),
    void* (*next)(void* internal_state),
    void (*reset)(void* internal_state),
    void* (*memory_alloc)(size_t),
    void (*memory_free)(void*)
);

/**
 * @brief Checks whether more elements are available.
 *
 * @param iterator pointer to an Iterator
 *
 * @return true if more elements are available, false otherwise
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 */
bool iterator_has_next(const Iterator* iterator);

/**
 * @brief Retrieves the next element from the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @return pointer to the next element
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
void* iterator_next(Iterator* iterator);

/**
 * @brief Resets the iterator to its initial position.
 *
 * @param iterator pointer to an Iterator
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 */
void iterator_reset(Iterator* iterator);

/**
 * @brief Destroys the iterator and releases its resources.
 *
 * @param iterator_pointer pointer to an Iterator*
 *
 * @exception NULL_POINTER_ERROR if iterator_pointer or *iterator_pointer is null
 *
 * @post *iterator_pointer == nullptr
 */
void iterator_destroy(Iterator** iterator_pointer);

#endif