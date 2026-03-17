#ifndef DEQUE_H
#define DEQUE_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A deque is a generic dynamic linear data structure that allows insertion and removal of elements at both the front and back of the sequence.
 * It automatically grows to accommodate new elements and allows insertion of values of any type.
 *
 * Internally, the implementation uses a struct containing an array of void pointers. All operations on a deque
 * receive a pointer to the deque itself as their first argument. The Deque type is opaque and can
 * only be modified through the API.
 *
 * It must be configured using a DequeOptions defining:
 * - its initial capacity (must be a power of two, otherwise it will be automatically rounded to the next power of two)
 * - the destruct function utilized to free elements memory
 * - the equals function utilized to compare elements
 * - the to string function utilized to convert its elements to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to free memory
 *
 * Underlying implementation (simplified):
 * @code
 * struct Deque {
 *      void** elements;
 *      int first;
 *      int last;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct Deque Deque;

/**
 * Deque configuration structure. Used to define the default behavior and attributes of a Deque.
 *
 * @pre initial_capacity >= 8
 * @pre initial_capacity < INT_MAX
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
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
        void (*memory_free)(void*);
    };
} DequeOptions;

/**
 * @brief A utility macro that provides a reasonable default DequeOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_DEQUE_OPTIONS(...) &(DequeOptions) {    \
    .initial_capacity = 8,                              \
    .destruct = noop_destruct,                          \
    .equals = pointer_equals,                           \
    .to_string = pointer_to_string,                     \
    .memory_alloc = malloc,                             \
    .memory_free = free,                                \
    __VA_ARGS__                                         \
}

/**
 * @brief Creates a new empty Deque using the specified options.
 *
 * @param options pointer to a DequeOptions defining the deque configuration
 *
 * @return Pointer to a newly created Deque on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the Deque fails
 */
Deque* deque_new(const DequeOptions* options);

/**
 * @brief Creates a new Deque containing all elements of the given collection using the specified options.
 *
 * @param collection a Collection containing the elements to be added
 * @param options pointer to a DequeOptions containing configuration settings
 *
 * @return pointer to a newly created Deque on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the new Deque or collection's iterator fails
 */
Deque* deque_from(Collection collection, const DequeOptions* options);

/**
 * @brief Destroys an existing Deque and (optionally) its elements using the provided destruct function.
 *
 * @param deque_pointer pointer to an Deque pointer
 *
 * @exception NULL_POINTER_ERROR if deque_pointer or *deque_pointer are null
 *
 * @post *deque_pointer == nullptr
 */
void deque_destroy(Deque** deque_pointer);

/**
 * @brief Set the current destruct function.
 *
 * @param deque pointer to a Deque
 * @param destructor the new destruct function
 */
void deque_set_destructor(Deque* deque, void (*destructor)(void*));

/**
 * @brief Inserts the specified element at the beginning of the provided Deque.
 *
 * @param deque pointer to a Deque
 * @param element pointer to the element to be inserted
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand deque capacity
 */
void deque_add_first(Deque* deque, const void* element);

/**
 * @brief Inserts the specified element at the end of the provided Deque.
 *
 * @param deque pointer to a Deque
 * @param element pointer to the element to be inserted
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand deque capacity
 */
void deque_add_last(Deque* deque, const void* element);

/**
 * @brief Inserts all elements of the specified Collection at the beginning of the provided Deque.
 *
 * @param deque pointer to a Deque
 * @param collection a Collection containing the elements to be added
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails.
 */
void deque_add_all_first(Deque* deque, Collection collection);

/**
 * @brief Inserts all elements of the specified Collection at the end of the provided Deque.
 *
 * @param deque pointer to a Deque
 * @param collection a Collection containing the elements to be added
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails.
 */
void deque_add_all_last(Deque* deque, Collection collection);

/**
 * @brief Retrieves the first element of the provided Deque.
 *
 * @param deque pointer to a Deque
 *
 * @return pointer to the retrieved element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided Deque is empty
 */
void* deque_get_first(const Deque* deque);

/**
 * @brief Retrieves the last element of the provided Deque.
 *
 * @param deque pointer to a Deque
 *
 * @return pointer to the retrieved element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided Deque is empty
 */
void* deque_get_last(const Deque* deque);

/**
 * @brief Removes the first element of the provided Deque, (optionally) destructing it.
 *
 * @param deque pointer to a Deque
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided Deque is empty
 */
void* deque_remove_first(Deque* deque);

/**
 * @brief Removes the last element of the provided Deque, (optionally) destructing it.
 *
 * @param deque pointer to a Deque
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided Deque is empty
 */
void* deque_remove_last(Deque* deque);

/**
 * @brief Retrieves the current size of the provided Deque.
 *
 * @param deque pointer to a Deque
 *
 * @return the current size of the provided Deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
int deque_size(const Deque* deque);

/**
 * @brief Retrieves the current capacity of the provided Deque.
 *
 * @param deque pointer to a Deque
 *
 * @return the current capacity of the provided Deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
int deque_capacity(const Deque* deque);

/**
 * @brief Checks whether the provided Deque is empty.
 *
 * @param deque pointer to a Deque
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
bool deque_is_empty(const Deque* deque);

/**
 * @brief Instantiates an Iterator for the provided Deque.
 *
 * @param deque pointer to a Deque
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 */
Iterator* deque_iterator(const Deque* deque);

/**
 * @brief Checks whether two Deque objects are equal.
 *
 * Two dequeues are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and each corresponding element in the first deque is considered equal to the element
 * at the same position in the second deque according to the equals function of the first deque.
 *
 * @param deque pointer to a Deque
 * @param other_deque pointer to a Deque
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque or other_deque are null
 */
bool deque_equals(const Deque* deque, const Deque* other_deque);

/**
 * @brief Performs an action for each element of the provided Deque.
 *
 * @param deque pointer to a Deque
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if deque or action are null
 */
void deque_for_each(Deque* deque, Consumer action);

/**
 * @brief Removes all elements of the provided Deque, (optionally) destructing them.
 *
 * @param deque pointer to a Deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
void deque_clear(Deque* deque);

/**
 * @brief Checks whether the provided Deque contains the specified element.
 *
 * @param deque pointer to a Deque
 * @param element pointer to the element to be checked
 *
 * @return true if the element is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
bool deque_contains(const Deque* deque, const void* element);

/**
 * @brief Checks whether the provided Deque contains all elements of the given collection.
 *
 * @param deque pointer to a Deque
 * @param collection a Collection containing elements to be checked
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
bool deque_contains_all(const Deque* deque, Collection collection);

/**
 * @brief Creates a shallow copy of the provided Deque.
 *
 * The new Deque will contain the same element pointers but will have independent internal storage.
 *
 * @param deque pointer to a Deque
 *
 * @return a newly created Deque clone, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
Deque* deque_clone(const Deque* deque);

/**
 * @brief Converts the provided Deque into a Collection view.
 *
 * The returned Collection does not own the underlying elements.
 *
 * @param deque pointer to a Deque
 *
 * @return a Collection representation of the Deque
 *
 * @exception NULL_POINTER_ERROR if deque is null
 */
Collection deque_to_collection(const Deque* deque);

/**
 * @brief Converts the provided Deque into a newly allocated array.
 *
 * @param deque pointer to a Deque
 *
 * @return a newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
void** deque_to_array(const Deque* deque);

/**
 * @brief Converts the provided Deque to a string representation.
 *
 * @param deque pointer to a Deque
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if deque is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* deque_to_string(const Deque* deque);

#endif