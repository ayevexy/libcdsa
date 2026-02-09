#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"
#include "util/optional.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A linked list is a linear, generic, dynamic data structure consisting of nodes that hold data and a pointer to the next and previous node.
 *
 * Internally, the implementation records the head and the tail, and each node links to a previous and next node.
 * All operations on a linked list receive a pointer to the linked list itself as their first argument.
 * The LinkedList type is opaque and can only be modified through the API.
 *
 * It must be configured using an LinkedListOptions structure defining:
 * - the destruct function utilized to free elements memory (optional)
 * - the equals function utilized to compare elements
 * - the to string function utilized to convert its elements to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to free memory
 *
 * Underlying implementation (simplified):
 * @code
 * struct LinkedList {
 *      Node* head;
 *      Node* tail;
 *      int size;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct LinkedList LinkedList;

/**
 * LinkedList configuration structure. Used to define the default behavior and attributes of an LinkedList.
 *
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
 */
typedef struct {
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
} LinkedListOptions;

/**
 * @brief A utility macro that provides a reasonable default LinkedListOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_LINKED_LIST_OPTIONS(...) &(LinkedListOptions) {     \
    .destruct = nullptr,                                            \
    .equals = pointer_equals,                                       \
    .to_string = pointer_to_string,                                 \
    .memory_alloc = malloc,                                         \
    .memory_free = free,                                            \
    __VA_ARGS__                                                     \
}

/**
 * @brief Creates a new empty LinkedList using the specified options.
 *
 * @param options pointer to an LinkedListOptions defining the linked list configuration
 *
 * @return Pointer to a newly created LinkedList on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the LinkedList fails
 */
LinkedList* linked_list_new(const LinkedListOptions* options);

/**
 * @brief Creates a new LinkedList containing all elements of the given collection using the specified options.
 *
 * @param collection a Collection containing the elements to be added
 * @param options pointer to an LinkedListOptions containing configuration settings
 *
 * @return pointer to a newly created LinkedList on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the new LinkedList or collection's iterator fails
 */
LinkedList* linked_list_from(Collection collection, const LinkedListOptions* options);

/**
 * @brief Destroys an existing LinkedList while keeping its elements intact.
 *
 * @param linked_list_pointer pointer to an LinkedList pointer
 *
 * @exception NULL_POINTER_ERROR if linked_list_pointer or *linked_list_pointer is null
 *
 * @post *linked_list_pointer == nullptr
 */
void linked_list_destroy(LinkedList** linked_list_pointer);

/**
 * @brief Obliterates an existing LinkedList and its elements using the provided destruct function.
 *
 * @param linked_list_pointer pointer to an LinkedList pointer
 *
 * @exception NULL_POINTER_ERROR if linked_list_pointer or *linked_list_pointer is null
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 *
 * @post *linked_list_pointer == nullptr
 */
void linked_list_obliterate(LinkedList** linked_list_pointer);

/**
 * @brief Inserts the specified element at the specified position in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList.
 * @param index index at which the specified element is to be inserted
 * @param element pointer to the element to be inserted
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > linked_list_size()
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand linked_list capacity
 */
void linked_list_add(LinkedList* linked_list, int index, const void* element);

/**
 * @brief Inserts the specified element at the beginning of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to be inserted
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand linked_list capacity
 */
void linked_list_add_first(LinkedList* linked_list, const void* element);

/**
 * @brief Inserts the specified element at the end of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to be inserted
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand linked_list capacity
 */
void linked_list_add_last(LinkedList* linked_list, const void* element);

/**
 * @brief Inserts all elements of the specified Collection at the specified position in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param index start index at which the elements will be inserted
 * @param collection a Collection containing the elements to be added
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > linked_list_size()
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails.
 */
void linked_list_add_all(LinkedList* linked_list, int index, Collection collection);

/**
 * @brief Inserts all elements of the specified Collection at the beginning of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param collection a Collection containing the elements to be added
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails.
 */
void linked_list_add_all_first(LinkedList* linked_list, Collection collection);

/**
 * @brief Inserts all elements of the specified Collection at the end of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param collection a Collection containing the elements to be added
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails.
 */
void linked_list_add_all_last(LinkedList* linked_list, Collection collection);

/**
 * @brief Retrieves the element at the specified position in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param index index at which the element will be retrieved
 *
 * @return pointer to the retrieved element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 */
void* linked_list_get(const LinkedList* linked_list, int index);

/**
 * @brief Retrieves the first element in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return pointer to the retrieved element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 */
void* linked_list_get_first(const LinkedList* linked_list);

/**
 * @brief Retrieves the last element in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return pointer to the retrieved element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 */
void* linked_list_get_last(const LinkedList* linked_list);

/**
 * @brief Replaces the element at the specified position of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param index index of the element to be replaced
 * @param element the new element
 *
 * @return pointer to the old element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 */
void* linked_list_set(LinkedList* linked_list, int index, const void* element);

/**
 * @brief Replaces the element at the specified position of the provided LinkedList, then destructs the old element.
 *
 * @param linked_list pointer to an LinkedList
 * @param index index of the element to be replaced
 * @param element the new element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 */
void linked_list_update(LinkedList* linked_list, int index, const void* element);

/**
 * @brief Swaps the elements at specified positions of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param index_a index of the first element to be swapped
 * @param index_b index of the second element to be swapped
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 */
void linked_list_swap(LinkedList* linked_list, int index_a, int index_b);

/**
 * @brief Removes the element at the specified position of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param index index of the element to be removed
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 */
void* linked_list_remove(LinkedList* linked_list, int index);

/**
 * @brief Removes the first element of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 */
void* linked_list_remove_first(LinkedList* linked_list);

/**
 * @brief Removes the last element of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 */
void* linked_list_remove_last(LinkedList* linked_list);

/**
 * @brief Deletes the element at the specified position of the provided LinkedList, using the given destruct function.
 *
 * @param linked_list pointer to an LinkedList
 * @param index index of the element to be removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list_size()
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 */
void linked_list_delete(LinkedList* linked_list, int index);

/**
 * @brief Deletes the first element of the provided LinkedList, using the given destruct function.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 */
void linked_list_delete_first(LinkedList* linked_list);

/**
 * @brief Deletes the last element of the provided LinkedList, using the given destruct function.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if the provided LinkedList is empty
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 */
void linked_list_delete_last(LinkedList* linked_list);

/**
 * @brief Removes the specified element (if present) of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to be removed
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
bool linked_list_remove_element(LinkedList* linked_list, const void* element);

/**
 * @brief Removes all elements of the given collection present in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param collection a Collection containing the elements to be removed
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int linked_list_remove_all(LinkedList* linked_list, Collection collection);

/**
 * @brief Removes all elements at the specified range in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > linked_list_size() || start_index > end_index
 */
int linked_list_remove_range(LinkedList* linked_list, int start_index, int end_index);

/**
 * @brief Removes all elements matching the given Predicate in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param condition the condition to remove elements
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_remove_if(LinkedList* linked_list, Predicate condition);

/**
 * @brief Replaces all elements using the given Operator of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param operator the operator to replace elements
 *
 * @exception NULL_POINTER_ERROR if linked_list or operator is null
 */
void linked_list_replace_all(LinkedList* linked_list, Operator operator);

/**
 * @brief Retains all elements of the given collection present in the provided LinkedList while removing all other elements.
 *
 * @param linked_list pointer to an LinkedList
 * @param collection a Collection containing the elements to be held
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int linked_list_retain_all(LinkedList* linked_list, Collection collection);

/**
 * @brief Retrieves the current size of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return the current size of the provided LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_size(const LinkedList* linked_list);

/**
 * @brief Checks whether the provided LinkedList is empty.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
bool linked_list_is_empty(const LinkedList* linked_list);

/**
 * @brief Instantiates an Iterator for the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 */
Iterator* linked_list_iterator(const LinkedList* linked_list);

/**
 * @brief Checks whether two LinkedList objects are equal.
 *
 * Two linked lists are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and each corresponding element in the first linked list is considered equal to the element
 * at the same position in the second linked list according to the equals function of the first linked list.
 *
 * @param linked_list pointer to an LinkedList
 * @param other_linked_list pointer to an LinkedList
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list or other_linked_list is null
 */
bool linked_list_equals(const LinkedList* linked_list, const LinkedList* other_linked_list);

/**
 * @brief Performs an action for each element of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if linked_list or action is null
 */
void linked_list_for_each(LinkedList* linked_list, Consumer action);

/**
 * @brief Sorts using the given Comparator and SortingAlgorithm the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param comparator the comparator to be used to compare elements
 * @param algorithm the algorithm used to sort the LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list or comparator is null
 */
void linked_list_sort(LinkedList* linked_list, Comparator comparator, SortingAlgorithm algorithm);

/**
 * @brief Shuffles using the given random function and ShufflingAlgorithm the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param random a function to generate random numbers
 * @param algorithm the algorithm used to shuffle the LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list or random is null
 */
void linked_list_shuffle(LinkedList* linked_list, int (*random)(void), ShufflingAlgorithm algorithm);

/**
 * @brief Reverses the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
void linked_list_reverse(LinkedList* linked_list);

/**
 * @brief Rotates using the given distance the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param distance how many positions to shift
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
void linked_list_rotate(LinkedList* linked_list, int distance);

/**
 * @brief Removes all elements of the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
void linked_list_clear(LinkedList* linked_list);

/**
 * @brief Deletes all elements of the provided LinkedList, applying the destruct function to every element.
 *
 * @param linked_list pointer to a LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception UNSUPPORTED_OPERATION_ERROR if no destruct function was provided
 */
void linked_list_purge(LinkedList* linked_list);

/**
 * @brief Finds the first element matching the given Predicate in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param condition the predicate used to test elements
 *
 * @return an Optional containing the found element, or an empty Optional if no match is found
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
Optional linked_list_find(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Finds the last element matching the given Predicate in the provided LinkedList.
 *
 * @param linked_list pointer to an LinkedList
 * @param condition the predicate used to test elements
 *
 * @return an Optional containing the found element, or an empty Optional if no match is found
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
Optional linked_list_find_last(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Retrieves the index of the first element matching the given Predicate.
 *
 * @param linked_list pointer to an LinkedList
 * @param condition the predicate used to test elements
 *
 * @return the index of the first matching element, or -1 if no match is found
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_index_where(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Retrieves the index of the last element matching the given Predicate.
 *
 * @param linked_list pointer to an LinkedList
 * @param condition the predicate used to test elements
 *
 * @return the index of the last matching element, or -1 if no match is found
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_last_index_where(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Checks whether the provided LinkedList contains the specified element.
 *
 * Comparison is performed using the equals function configured in the LinkedListOptions.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to be checked
 *
 * @return true if the element is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
bool linked_list_contains(const LinkedList* linked_list, const void* element);

/**
 * @brief Checks whether the provided LinkedList contains all elements of the given collection.
 *
 * @param linked_list pointer to an LinkedList
 * @param collection a Collection containing elements to be checked
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
bool linked_list_contains_all(const LinkedList* linked_list, Collection collection);

/**
 * @brief Counts the number of occurrences of the specified element in the provided LinkedList.
 *
 * Comparison is performed using the equals function configured in the LinkedListOptions.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to be counted
 *
 * @return the number of occurrences of the specified element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_occurrences_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Retrieves the index of the first occurrence of the specified element.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to search for
 *
 * @return the index of the first occurrence, or -1 if not found
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_index_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Retrieves the index of the last occurrence of the specified element.
 *
 * @param linked_list pointer to an LinkedList
 * @param element pointer to the element to search for
 *
 * @return the index of the last occurrence, or -1 if not found
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_last_index_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Creates a shallow copy of the provided LinkedList.
 *
 * The new LinkedList will contain the same element pointers but will have independent internal storage.
 *
 * @param linked_list pointer to an LinkedList, or nullptr on failure
 *
 * @return a newly created LinkedList clone
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
LinkedList* linked_list_clone(const LinkedList* linked_list);

/**
 * @brief Creates a sublist of the provided LinkedList within the specified range.
 *
 * The returned LinkedList contains elements from start_index (inclusive)
 * to end_index (exclusive).
 *
 * @param linked_list pointer to an LinkedList
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return a newly created LinkedList containing the specified range, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > linked_list_size() || start_index > end_index
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
LinkedList* linked_list_sub_list(const LinkedList* linked_list, int start_index, int end_index);

/**
 * @brief Converts the provided LinkedList into a Collection view.
 *
 * The returned Collection does not own the underlying elements.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return a Collection representation of the LinkedList
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
Collection linked_list_to_collection(const LinkedList* linked_list);

/**
 * @brief Converts the provided LinkedList into a newly allocated array.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return a newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
void** linked_list_to_array(const LinkedList* linked_list);

/**
 * @brief Converts the provided LinkedList to a string representation.
 *
 * Each element is converted using the to_string function configured
 * in the LinkedListOptions.
 *
 * @param linked_list pointer to an LinkedList
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* linked_list_to_string(const LinkedList* linked_list);

#endif