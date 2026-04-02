#ifndef LIBCDSA_ITERATOR_H
#define LIBCDSA_ITERATOR_H

#include "functions.h"

/**
 * @brief iterator abstraction.
 *
 * Iterator provides a generic mechanism to traverse elements of a data structure
 * without exposing its internal representation.
 *
 * Iterators are stateful and heap-allocated. They must be explicitly deleted using iterator_destroy.
 *
 * This API is not thread-safe.
 */
typedef struct {
    void* iteration_context;
    bool (*has_next)(const void* iteration_context);
    void* (*next)(void* iteration_context);
    bool (*has_previous)(const void* iteration_context);
    void* (*previous)(void* iteration_context);
    void (*add)(void* iteration_context, const void* element);
    void (*set)(void* iteration_context, const void* element);
    void (*remove)(void* iteration_context);
    void (*reset)(void* iteration_context);
    void (*memory_dealloc)(void*);
} Iterator;

/**
 * @brief Checks whether more next elements are available.
 *
 * @param iterator pointer to an Iterator
 *
 * @return true if more elements are available, false otherwise
 */
static inline bool iterator_has_next(const Iterator* iterator) {
    return iterator->has_next(iterator->iteration_context);
}

/**
 * @brief Retrieves the next element from the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @return pointer to the next element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
static inline void* iterator_next(Iterator* iterator) {
    return iterator->next(iterator->iteration_context);
}

/**
 * @brief Advances the iterator n-times and retrieves the current element.
 *
 * @param iterator pointer to an Iterator
 * @param count how many times the iterator will advance
 *
 * @return pointer to the retrieved element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
static inline void* iterator_advance(Iterator* iterator, int count) {
    for (int i = 0; i < count - 1; i++) {
        iterator_next(iterator);
    }
    return count > 0 ? iterator_next(iterator) : nullptr;
}

/**
 * @brief Checks whether more previous elements are available.
 *
 * @param iterator pointer to an Iterator
 *
 * @return true if more elements are available, false otherwise
 *
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
static inline bool iterator_has_previous(const Iterator* iterator) {
    return iterator->has_previous(iterator->iteration_context);
}

/**
 * @brief Retrieves the previous element from the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @return pointer to the previous element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
static inline void* iterator_previous(Iterator* iterator) {
    return iterator->previous(iterator->iteration_context);
}

/**
 * @brief Retreats the iterator n-times and retrieves the current element.
 *
 * @param iterator pointer to an Iterator
 * @param count how many times the iterator will retreat
 *
 * @return pointer to the retrieved element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
static inline void* iterator_retreat(Iterator* iterator, int count) {
    for (int i = 0; i < count - 1; i++) {
        iterator_previous(iterator);
    }
    return count > 0 ? iterator_previous(iterator) : nullptr;
}

/**
 * @brief Add the specified element to the underlying data structure (at the specified position if possible) represented by the iterator.
 *
 * @param iterator pointer to an Iterator
 * @param element the element to be added
 *
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
static inline void iterator_add(Iterator* iterator, const void* element) {
    iterator->add(iterator->iteration_context, element);
}

/**
 * @brief Replaces the last element returned from the iterator.
 *
 * @param iterator pointer to an Iterator
 * @param element the new element
 *
 * @exception ILLEGAL_STATE_ERROR if neither next nor previous have been called, or remove or add have been called after the last call to next or previous
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
static inline void iterator_set(Iterator* iterator, const void* element) {
    iterator->set(iterator->iteration_context, element);
}

/**
 * @brief Removes the last element returned by the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @exception ILLEGAL_STATE_ERROR if neither next nor previous have been called, or remove or add have been called after the last call to next or previous
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
static inline void iterator_remove(Iterator* iterator) {
    iterator->remove(iterator->iteration_context);
}

/**
 * @brief Resets the iterator to its initial position.
 *
 * @param iterator pointer to an Iterator
 */
static inline void iterator_reset(Iterator* iterator) {
    iterator->reset(iterator->iteration_context);
}

/**
 * @brief Iterate through the remaining elements of the iterator applying the action function to each element.
 *
 * @param iterator pointer to an Iterator
 * @param action the action to perform
 */
static inline void iterator_for_each_remaining(Iterator* iterator, Consumer action) {
    while (iterator_has_next(iterator)) {
        action(iterator_next(iterator));
    }
}

/**
 * @brief Destroys the iterator and releases its resources.
 *
 * @param iterator_pointer pointer to an Iterator*
 *
 * @post *iterator_pointer == nullptr
 */
static inline void iterator_destroy(Iterator** iterator_pointer) {
    const Iterator* iterator = *iterator_pointer;
    iterator->memory_dealloc(iterator->iteration_context);
    *iterator_pointer = nullptr;
}

#endif