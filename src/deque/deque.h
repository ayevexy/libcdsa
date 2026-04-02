#ifndef LIBCDSA_DEQUE_H
#define LIBCDSA_DEQUE_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A deque is a generic dynamic linear data structure that allows insertion
 * and removal of elements at both the front and back of the sequence.
 * It automatically resizes to accommodate new elements and supports storing
 * elements of any type, including null pointers.
 *
 * Internally, the implementation uses a circular buffer backed by a dynamically
 * allocated array of `void*`. All operations receive a pointer to the deque as
 * their first argument. The Deque type is opaque and can only be modified
 * through the public API.
 *
 * A deque must be configured using a DequeOptions structure specifying:
 * - the initial capacity (must be a power of two; otherwise it is rounded up)
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct Deque {
 *     void** elements;
 *     int first;    // index of the first element
 *     int last;     // index one past the last element
 *     int size;
 *     int capacity;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the deque does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., deque_remove_first, deque_remove_last)
 * - the deque is cleared (deque_clear)
 * - the deque is destroyed (deque_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - deque_add_first / deque_add_last: amortized O(1)
 * - deque_remove_first / deque_remove_last: O(1)
 * - deque_get_first / deque_get_last: O(1)
 * - deque_contains: O(n)
 */
typedef struct Deque Deque;

/**
 * Deque configuration structure. Defines the behavior and attributes of a deque.
 *
 * @pre initial_capacity >= 8
 * @pre initial_capacity <= 1'073'741'824
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_dealloc != nullptr
 */
typedef struct {
    int initial_capacity;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_dealloc)(void*);
    };
} DequeOptions;

/**
 * @brief Utility macro providing default DequeOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_DEQUE_OPTIONS(...) &(DequeOptions) {    \
    .initial_capacity = 8,                              \
    .destruct = noop_destruct,                          \
    .equals = pointer_equals,                           \
    .to_string = pointer_to_string,                     \
    .memory_alloc = malloc,                             \
    .memory_dealloc = free,                             \
    __VA_ARGS__                                         \
}

/**
 * @brief Creates a new empty deque using the specified options.
 *
 * @param options pointer to a DequeOptions structure
 *
 * @return pointer to a newly created deque
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
Deque* deque_new(const DequeOptions* options);

/**
 * @brief Creates a new deque containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created deque
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
Deque* deque_from(Collection collection, const DequeOptions* options);

/**
 * @brief Destroys a deque and optionally its elements.
 *
 * @param deque_pointer pointer to a deque pointer
 *
 * @exception NULL_POINTER_ERROR if deque_pointer or *deque_pointer is null
 *
 * @post *deque_pointer == nullptr
 */
void deque_destroy(Deque** deque_pointer);

/**
 * @brief Changes the element destruct function.
 *
 * @param deque pointer to a deque
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if deque or destruct is null
 */
void deque_change_destructor(Deque* deque, void (*destruct)(void*));

/**
 * @brief Inserts an element at the front of the deque.
 *
 * @param deque pointer to a deque
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void deque_add_first(Deque* deque, const void* element);

/**
 * @brief Inserts an element at the end of the deque.
 *
 * @param deque pointer to a deque
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void deque_add_last(Deque* deque, const void* element);

/**
 * @brief Inserts all elements of a collection at the front of the deque.
 *
 * @param deque pointer to a deque
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void deque_add_all_first(Deque* deque, Collection collection);

/**
 * @brief Inserts all elements of a collection at the end of the deque.
 *
 * @param deque pointer to a deque
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void deque_add_all_last(Deque* deque, Collection collection);

/**
 * @brief Retrieves the first element of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if deque is empty
 */
void* deque_get_first(const Deque* deque);

/**
 * @brief Retrieves the last element of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to the last element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if deque is empty
 */
void* deque_get_last(const Deque* deque);

/**
 * @brief Removes and returns the first element of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if deque is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* deque_remove_first(Deque* deque);

/**
 * @brief Removes and returns the last element of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if deque is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* deque_remove_last(Deque* deque);

/**
 * @brief Returns the current number of elements in the deque.
 *
 * @param deque pointer to a deque
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
int deque_size(const Deque* deque);

/**
 * @brief Returns the current capacity of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
int deque_capacity(const Deque* deque);

/**
 * @brief Checks whether the deque is empty.
 *
 * @param deque pointer to a deque
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
bool deque_is_empty(const Deque* deque);

/**
 * @brief Creates an iterator for the deque.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add(), set(), and remove()
 */
Iterator* deque_iterator(const Deque* deque);

/**
 * @brief Creates an iterator for the deque starting at the specified position.
 *
 * @param deque pointer to a deque
 * @param position start position
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if position < 0 || position > deque.size
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add(), set(), and remove()
 */
Iterator* deque_iterator_at(const Deque* deque, int position);

/**
 * @brief Checks whether two deques are equal.
 *
 * Two deques are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first deque
 *
 * @param deque first deque
 * @param other_deque second deque
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either deque is null
 */
bool deque_equals(const Deque* deque, const Deque* other_deque);

/**
 * @brief Applies an action to each element of the deque.
 *
 * @param deque pointer to a deque
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if deque or action is null
 */
void deque_for_each(Deque* deque, Consumer action);

/**
 * @brief Reverses the elements of the deque.
 *
 * @param deque pointer to a deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
void deque_reverse(Deque* deque);

/**
 * @brief Removes all elements of the deque.
 *
 * @param deque pointer to a deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 *
 * @note this function calls the element destruct
 */
void deque_clear(Deque* deque);

/**
 * @brief Checks whether an element is present in the deque.
 *
 * @param deque pointer to a deque
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
bool deque_contains(const Deque* deque, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the deque.
 *
 * @param deque pointer to a deque
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool deque_contains_all(const Deque* deque, Collection collection);

/**
 * @brief Creates a shallow copy of the deque.
 *
 * The new deque shares element pointers but has independent storage.
 *
 * @param deque pointer to a deque
 *
 * @return pointer to the newly created deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
Deque* deque_clone(const Deque* deque);

/**
 * @brief Returns a Collection view of the deque.
 *
 * The returned collection does not own the elements.
 *
 * @param deque pointer to a deque
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
Collection deque_to_collection(const Deque* deque);

/**
 * @brief Returns a newly allocated array containing all elements of the deque.
 *
 * @param deque pointer to a deque
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
void** deque_to_array(const Deque* deque);

/**
 * @brief Converts the deque to a string representation.
 *
 * @param deque pointer to a deque
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* deque_to_string(const Deque* deque);

#endif