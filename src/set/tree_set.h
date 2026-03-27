#ifndef TREE_SET_H
#define TREE_SET_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A tree set is an ordered data structure that stores unique elements.
 * It automatically grows to accommodate new elements and allows insertion of values of any type.
 *
 * Internally, the implementation uses a self-balancing binary search tree.
 * All operations on a TreeSet receive a pointer to the TreeSet itself as their first argument.
 * The TreeSet type is opaque and can only be modified through the provided API functions.
 *
 * It must be configured using a TreeSetOptions defining:
 * - the function to compare elements
 * - the destruct function utilized to free elements memory
 * - the equals function utilized to compare elements
 * - the to string function utilized to convert its elements to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to free memory
 *
 * Underlying implementation (simplified):
 * @code
 * struct TreeSet {
 *      Node* root;
 *      int size;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct TreeSet TreeSet;

/**
 * TreeSet configuration structure. Used to define the default behavior and attributes of a TreeSet.
 *
 * @pre compare != nullptr
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
 */
typedef struct {
    Comparator compare;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
} TreeSetOptions;

/**
 * @brief A utility macro that provides a reasonable default TreeSetOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_TREE_SET_OPTIONS(...) &(TreeSetOptions) {   \
    .compare = compare_pointers,                            \
    .destruct = noop_destruct,                              \
    .equals = pointer_equals,                               \
    .to_string = pointer_to_string,                         \
    .memory_alloc = malloc,                                 \
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

/**
 * @brief Creates a new empty TreeSet using the specified options.
 *
 * @param options pointer to a TreeSetOptions defining the tree set configuration
 *
 * @return pointer to the newly created TreeSet on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the TreeSet fails
 */
TreeSet* tree_set_new(const TreeSetOptions* options);

/**
 * @brief Creates a new TreeSet containing all elements of the given collection using the specified options.
 *
 * @param collection a Collection containing the elements to be added
 * @param options pointer to a TreeSetOptions containing configuration settings
 *
 * @return pointer to a newly created TreeSet on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the new TreeSet or collection's iterator fails
 */
TreeSet* tree_set_from(Collection collection, const TreeSetOptions* options);

/**
 * @brief Destroys an existing TreeSet and (optionally) its elements using the provided destruct function.
 *
 * @param tree_set_pointer pointer to a TreeSet pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set_pointer or *tree_set_pointer are null
 *
 * @post *tree_set_pointer == nullptr
 */
void tree_set_destroy(TreeSet** tree_set_pointer);

/**
 * @brief Set the current destruct function.
 *
 * @param tree_set pointer to a TreeSet
 * @param destructor the new destruct function
 */
void tree_set_set_destructor(TreeSet* tree_set, void (*destructor)(void*));

/**
 * @brief Inserts the specified element in the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element to be inserted
 *
 * @return true if the element was inserted, false if already present
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand tree_set capacity or allocate a new node
 */
bool tree_set_add(TreeSet* tree_set, const void* element);

/**
 * @brief Inserts all elements of the specified Collection in the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 * @param collection a Collection containing the elements to be added
 *
 * @return true if the TreeSet was changed, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand tree_set capacity or allocate a new node, or to allocate the collection's iterator
 */
bool tree_set_add_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Retrieves the first element of the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return pointer to the element, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_get_first(const TreeSet* tree_set);

/**
 * @brief Retrieves the last element of the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return pointer to the element, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_get_last(const TreeSet* tree_set);

/**
 * @brief Removes the specified element (if present) of the provided TreeSet, (optionally) destructing it.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element to be removed
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
bool tree_set_remove(TreeSet* tree_set, const void* element);

/**
 * @brief Removes the first element of the provided TreeSet, (optionally) destructing it.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return pointer to the element, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_remove_first(TreeSet* tree_set);

/**
 * @brief Removes the last element of the provided TreeSet, (optionally) destructing it.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return pointer to the element, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_remove_last(TreeSet* tree_set);

/**
 * @brief Removes all elements of the given collection present in the provided TreeSet, (optionally) destructing them.
 *
 * @param tree_set pointer to a TreeSet
 * @param collection a Collection containing the elements to be removed
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int tree_set_remove_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Removes all elements matching the given Predicate in the provided TreeSet, (optionally) destructing them.
 *
 * @param tree_set pointer to a TreeSet
 * @param condition the condition to remove elements
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set or condition are null
 */
int tree_set_remove_if(TreeSet* tree_set, Predicate condition);

/**
 * @brief Retains all elements of the given collection present in the provided
 * TreeSet while removing all other elements, (optionally) destructing them.
 *
 * @param tree_set pointer to a TreeSet
 * @param collection a Collection containing the elements to be held
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int tree_set_retain_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Retrieves the current size of the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return the current size of the provided TreeSet
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
int tree_set_size(const TreeSet* tree_set);

/**
 * @brief Checks whether the provided TreeSet is empty.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
bool tree_set_is_empty(const TreeSet* tree_set);

/**
 * @brief Instantiates an Iterator for the provided TreeSet.
 *
 * The iteration order is ascending according to the comparator.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 *
 * @note this iterator doesn't support  adding or setting elements
 */
Iterator* tree_set_iterator(const TreeSet* tree_set);

/**
 * @brief Checks whether two TreeSet objects are equal.
 *
 * Two tree sets are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and for every element in the first set, there exists an equal element in the second set.
 *
 * @param tree_set pointer to a TreeSet
 * @param other_tree_set pointer to a TreeSet
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set or other_tree_set are null
 */
bool tree_set_equals(const TreeSet* tree_set, const TreeSet* other_tree_set);

/**
 * @brief Performs an action for each element of the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if tree_set or action are null
 */
void tree_set_for_each(TreeSet* tree_set, Consumer action);

/**
 * @brief Removes all elements of the provided TreeSet, (optionally) destructing them.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void tree_set_clear(TreeSet* tree_set);

/**
 * @brief Retrieves the least element strictly greater than the specified element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element
 *
 * @return the element pointer, or nullptr if no such element exists
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_higher(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the least element greater than or equal to the specified element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element
 *
 * @return the element pointer, or nullptr if no such element exists
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_ceiling(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the greatest element less than or equal to the specified element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element
 *
 * @return the element pointer, or nullptr if no such element exists
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_floor(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the greatest element strictly less than the specified element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element
 *
 * @return the element pointer, or nullptr if no such element exists
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_lower(const TreeSet* tree_set, const void* element);

/**
 * @brief Checks whether the provided TreeSet contains the specified element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the element to be checked
 *
 * @return true if the element is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
bool tree_set_contains(const TreeSet* tree_set, const void* element);

/**
 * @brief Checks whether the provided TreeSet contains all elements of the given collection.
 *
 * @param tree_set pointer to a TreeSet
 * @param collection a Collection containing elements to be checked
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
bool tree_set_contains_all(const TreeSet* tree_set, Collection collection);

/**
 * @brief Returns a view of the portion of the TreeSet whose elements are less than or equal the given element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the exclusive upper bound element
 *
 * @return a newly created TreeSet containing the head set, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if element is inexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_head_set(const TreeSet* tree_set, const void* element);

/**
 * @brief Returns a view of the portion of the TreeSet whose elements are greater than or equal to the given element.
 *
 * @param tree_set pointer to a TreeSet
 * @param element pointer to the inclusive lower bound element
 *
 * @return a newly created TreeSet containing the tail set, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if element is inexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_tail_set(const TreeSet* tree_set, const void* element);

/**
 * @brief Creates a shallow copy of the provided TreeSet.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return a newly created TreeSet clone, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_clone(const TreeSet* tree_set);

/**
 * @brief Creates a subset of the provided TreeSet within the specified range.
 *
 * The returned TreeSet contains elements from start_element (inclusive)
 * to end_element (exclusive).
 *
 * @param tree_set pointer to a TreeSet
 * @param start_element start element (inclusive)
 * @param end_element end element (exclusive)
 *
 * @return a newly created TreeSet containing the specified range, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if start_element or end_element are inexistent or start_element is greater than end_element
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_sub_set(const TreeSet* tree_set, const void* start_element, const void* end_element);

/**
 * @brief Converts the provided TreeSet into a Collection view.
 *
 * The returned Collection does not own the underlying elements.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return a Collection representation of the TreeSet
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
Collection tree_set_to_collection(const TreeSet* tree_set);

/**
 * @brief Converts the provided TreeSet into a newly allocated array.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return a newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
void** tree_set_to_array(const TreeSet* tree_set);

/**
 * @brief Converts the provided TreeSet to a string representation.
 *
 * @param tree_set pointer to a TreeSet
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* tree_set_to_string(const TreeSet* tree_set);

#endif