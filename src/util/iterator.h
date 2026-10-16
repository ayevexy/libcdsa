#ifndef LIBCDSA_ITERATOR_H
#define LIBCDSA_ITERATOR_H

/**
 * @file iterator.h
 * @brief Iterator abstraction.
 */

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
    void (*destroy)(void* iteration_context);
} Iterator;

/**
 * @brief Checks whether more next elements are available.
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
 * @brief Advances the iterator n-times or until no more elements are available.
 *
 * @param iterator pointer to an Iterator
 * @param count how many times the iterator will advance
 *
 * @return number of elements traversed
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_ARGUMENT_ERROR if count is negative
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
int iterator_advance(Iterator* iterator, int count);

/**
 * @brief Checks whether more previous elements are available.
 *
 * @param iterator pointer to an Iterator
 *
 * @return true if more elements are available, false otherwise
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
bool iterator_has_previous(const Iterator* iterator);

/**
 * @brief Retrieves the previous element from the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @return pointer to the previous element
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception NO_SUCH_ELEMENT_ERROR if no more elements are available
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
void* iterator_previous(Iterator* iterator);

/**
 * @brief Retreats the iterator n-times or until no more elements are available.
 *
 * @param iterator pointer to an Iterator
 * @param count how many times the iterator will retreat
 *
 * @return number of elements traversed
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_ARGUMENT_ERROR if count is negative
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support backward traversal
 */
int iterator_retreat(Iterator* iterator, int count);

/**
 * @brief Advances the iterator by the given number of steps and retrieves the element at that position.
 *
 * @param iterator pointer to an Iterator
 * @param position the number of steps from the current position
 *
 * @return pointer to the element at that position
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_ARGUMENT_ERROR if position is negative
 * @exception NO_SUCH_ELEMENT_ERROR if no element exists at that position
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
void* iterator_element(Iterator* iterator, int position);

/**
 * @brief Advances the iterator by the given number of steps and retrieves the element at that position
 * or the default value if no element exists.
 *
 * @param iterator pointer to an Iterator
 * @param position the number of steps from the current position
 * @param default_value the default value
 *
 * @return pointer to the element at that position or the default value
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_ARGUMENT_ERROR if position is negative
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 */
void* iterator_element_or_default(Iterator* iterator, int position, void* default_value);

/**
 * @brief Add the specified element to the underlying data structure (at the specified position if possible) represented by the iterator.
 *
 * @param iterator pointer to an Iterator
 * @param element the element to be added
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
void iterator_add(Iterator* iterator, const void* element);

/**
 * @brief Replaces the last element returned from the iterator.
 *
 * @param iterator pointer to an Iterator
 * @param element the new element
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_STATE_ERROR if neither next nor previous have been called, or remove or add have been called after the last call to next or previous
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
void iterator_set(Iterator* iterator, const void* element);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * @param iterator pointer to an Iterator
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 * @exception ILLEGAL_STATE_ERROR if neither next nor previous have been called, or remove or add have been called after the last call to next or previous
 * @exception CONCURRENT_MODIFICATION_ERROR if the referenced collection was modified after iterator creation
 * @exception UNSUPPORTED_OPERATION_ERROR if the iterator don't support mutability
 */
void iterator_remove(Iterator* iterator);

/**
 * @brief Resets the iterator to its initial state.
 *
 * @param iterator pointer to an Iterator
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 */
void iterator_reset(Iterator* iterator);

/**
 * @brief Returns the number of elements remaining in the iterator, exhausting it in the process.
 *
 * @param iterator pointer to an Iterator
 *
 * @return the number of remaining elements
 *
 * @exception NULL_POINTER_ERROR if iterator is null
 */
int iterator_size(Iterator* iterator);

/**
 * @brief Iterate through the remaining elements of the iterator applying the action function to each element.
 *
 * @param iterator pointer to an Iterator
 * @param action the action to perform
 *
 * @exception NULL_POINTER_ERROR if iterator or action is null
 */
void iterator_for_each_remaining(Iterator* iterator, Consumer action);

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