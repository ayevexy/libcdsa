#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

/**
 * @brief Forward iterator abstraction.
 *
 * `Iterator` provides a generic mechanism to traverse elements of a data structure
 * without exposing its internal representation.
 *
 * Iterators are stateful and heap-allocated. They must be explicitly deleted
 * using `iterator_delete`.
 *
 * This API is not thread-safe.
 */
typedef struct Iterator Iterator;

/**
 * @brief Convenience macro to create an `Iterator` from an iterable structure.
 *
 * This macro forwards the iterable structure's memory management functions
 * to the underlying iterator.
 *
 * @param iterable_structure pointer to a structure providing `memory_alloc` and `memory_free` function members
 * @param internal_state pointer to the iterator's internal state
 * @param has_next function that returns whether more elements are available
 * @param next function that returns the next element
 * @param reset function that resets the iterator to its initial state
 *
 * @return pointer to a newly created `Iterator`
 *
 * @pre iterable_structure != `nullptr`
 * @pre internal_state != `nullptr`
 * @pre has_next != `nullptr`
 * @pre next != `nullptr`
 * @pre reset != `nullptr`
 */
#define iterator_from(iterable_structure, internal_state, has_next, next, reset)    \
    iterator_new(internal_state,                                                    \
        (bool (*)(const void*)) has_next,                                           \
        (void* (*)(void*)) next,                                                    \
        (void (*)(void*)) reset,                                                    \
        iterable_structure->memory_alloc,                                           \
        iterable_structure->memory_free                                             \
    )

/**
 * @brief Creates a new `Iterator`.
 *
 * @param internal_state pointer to the iterator's internal state
 * @param has_next function to test for remaining elements
 * @param next function to retrieve the next element
 * @param reset function to reset the iterator
 * @param memory_alloc function used to allocate memory
 * @param memory_free function used to free memory
 *
 * @return pointer to a newly created `Iterator`, or ``nullptr`` on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @pre internal_state != `nullptr`
 * @pre has_next != `nullptr`
 * @pre next != `nullptr`
 * @pre reset != `nullptr`
 * @pre memory_alloc != `nullptr`
 * @pre memory_free != `nullptr`
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
 * @param iterator pointer to an `Iterator`
 *
 * @return `true` if more elements are available, `false` otherwise
 *
 * @pre iterator != `nullptr`
 */
bool iterator_has_next(const Iterator* iterator);

/**
 * @brief Retrieves the next element from the iterator.
 *
 * @param iterator pointer to an `Iterator`
 *
 * @return pointer to the next element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 *
 * @pre iterator != `nullptr`
 */
void* iterator_next(Iterator* iterator);

/**
 * @brief Resets the iterator to its initial position.
 *
 * @param iterator pointer to an `Iterator`
 *
 * @pre iterator != `nullptr`
 */
void iterator_reset(Iterator* iterator);

/**
 * @brief Deletes the iterator and releases its resources.
 *
 * @param iterator_pointer pointer to an `Iterator*`
 *
 * @note After deletion, `*iterator_pointer` is set to ``nullptr``.
 *
 * @pre iterator_pointer != `nullptr`
 */
void iterator_delete(Iterator** iterator_pointer);

#endif