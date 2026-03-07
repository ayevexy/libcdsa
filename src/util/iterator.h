#ifndef ITERATOR_H
#define ITERATOR_H

/**
 * @brief Forward iterator abstraction.
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
    void (*reset)(void* iteration_context);
    void (*memory_free)(void*);
} Iterator;

/**
 * @brief Checks whether more elements are available.
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
 * @brief Resets the iterator to its initial position.
 *
 * @param iterator pointer to an Iterator
 */
static inline void iterator_reset(Iterator* iterator) {
    iterator->reset(iterator->iteration_context);
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
    iterator->memory_free(iterator->iteration_context);
    *iterator_pointer = nullptr;
}

#endif