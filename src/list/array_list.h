#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"
#include "util/optional.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * An array list is a generic dynamic linear data structure that stores elements
 * contiguously in memory allowing random access to any of them.
 * It automatically resizes to accommodate new elements and supports storing
 * elements of any type, including null pointers.
 *
 * Internally, the implementation uses a dynamically allocated array of `void*`.
 * All operations receive a pointer to the array list as their first argument.
 * The ArrayList type is opaque and can only be modified through the public API.
 *
 * An array list must be configured using an ArrayListOptions structure specifying:
 * - the initial capacity
 * - the growth factor
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory reallocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct ArrayList {
 *      void** elements;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the array list does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., array_list_remove, array_list_remove_all)
 * - elements are replaced (e.g., array_list_set, array_list_replace_all)
 * - the array list is cleared (array_list_clear)
 * - the array list is destroyed (array_list_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - array_list_add / array_list_add_first: O(n)
 * - array_list_add_last: amortized O(1)
 * - array_list_remove / array_list_remove_first: O(n)
 * - array_list_remove_last: O(1)
 * - array_list_get: O(1)
 * - array_list_set: O(1)
 * - array_list_contains / array_list_index_of: O(n)
 */
typedef struct ArrayList ArrayList;

/**
 * ArrayList configuration structure. Defines the behavior and attributes of an array list.
 *
 * @pre initial_capacity >= 10
 * @pre initial_capacity <= 1'073'741'824
 * @pre growth_factor >= 1.10
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_realloc != nullptr
 * @pre memory_free != nullptr
 */
typedef struct {
    int initial_capacity;
    float growth_factor;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} ArrayListOptions;

/**
 * @brief Utility macro providing default ArrayListOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_ARRAY_LIST_OPTIONS(...) &(ArrayListOptions) {   \
    .initial_capacity = 10,                                     \
    .growth_factor = 2.0f,                                      \
    .destruct = noop_destruct,                                  \
    .equals = pointer_equals,                                   \
    .to_string = pointer_to_string,                             \
    .memory_alloc = malloc,                                     \
    .memory_realloc = realloc,                                  \
    .memory_free = free,                                        \
    __VA_ARGS__                                                 \
}

/**
 * @brief Creates a new empty array list using the specified options.
 *
 * @param options pointer to an ArrayListOptions structure
 *
 * @return pointer to a newly created array list
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for fails
 */
ArrayList* array_list_new(const ArrayListOptions* options);

/**
 * @brief Creates a new array list containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created array list
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
ArrayList* array_list_from(Collection collection, const ArrayListOptions* options);

/**
 * @brief Destroys an array list and optionally its elements.
 *
 * @param array_list_pointer pointer to an array list pointer
 *
 * @exception NULL_POINTER_ERROR if array_list_pointer or *array_list_pointer is null
 *
 * @post *array_list_pointer == nullptr
 */
void array_list_destroy(ArrayList** array_list_pointer);

/**
 * @brief Sets the element destruct function.
 *
 * @param array_list pointer to an array list
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if array_list or destruct is null
 */
void array_list_set_destructor(ArrayList* array_list, void (*destruct)(void*));

/**
 * @brief Inserts an element at the specified position of the array list.
 *
 * @param array_list pointer to an array list
 * @param index insertion position
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > array_list.size
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void array_list_add(ArrayList* array_list, int index, const void* element);

/**
 * @brief Inserts an element at the beginning of the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void array_list_add_first(ArrayList* array_list, const void* element);

/**
 * @brief Inserts an element at the end of the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void array_list_add_last(ArrayList* array_list, const void* element);

/**
 * @brief Inserts all elements of a collection at the specified position of the array list.
 *
 * @param array_list pointer to an array list
 * @param index insertion position
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > array_list.size
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void array_list_add_all(ArrayList* array_list, int index, Collection collection);

/**
 * @brief Inserts all elements of a collection at the beginning of the array list.
 *
 * @param array_list pointer to an array list
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void array_list_add_all_first(ArrayList* array_list, Collection collection);

/**
 * @brief Inserts all elements of a collection at the end of the array list.
 *
 * @param array_list pointer to an array list
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creation of the collection iterator fails
 */
void array_list_add_all_last(ArrayList* array_list, Collection collection);

/**
 * @brief Retrieves the element at the specified position of the array list.
 *
 * @param array_list pointer to an array list
 * @param index element position
 *
 * @return pointer to the element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list.size
 */
void* array_list_get(const ArrayList* array_list, int index);

/**
 * @brief Retrieves the first element of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if array_list is empty
 */
void* array_list_get_first(const ArrayList* array_list);

/**
 * @brief Retrieves the last element of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to the last element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if array_list is empty
 */
void* array_list_get_last(const ArrayList* array_list);

/**
 * @brief Replaces the element at the specified position of the array list.
 *
 * @param array_list pointer to an array list
 * @param index element position
 * @param element the new element
 *
 * @return pointer to the old element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list.size
 * 
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the old element, the returned pointer becomes invalid.
 */
void* array_list_set(ArrayList* array_list, int index, const void* element);

/**
 * @brief Swaps the elements at specified positions of the array list.
 *
 * @param array_list pointer to an array list
 * @param index_a first element position
 * @param index_b second element position
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list.size
 */
void array_list_swap(ArrayList* array_list, int index_a, int index_b);

/**
 * @brief Removes and returns the element at the specified position of the array list.
 *
 * @param array_list pointer to an array list
 * @param index element position
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list.size
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* array_list_remove(ArrayList* array_list, int index);

/**
 * @brief Removes and returns the first element of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if array_list is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* array_list_remove_first(ArrayList* array_list);

/**
 * @brief Removes and returns the last element of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if array_list is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* array_list_remove_last(ArrayList* array_list);

/**
 * @brief Removes the specified element (if present) of the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to remove
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note this function calls the element destruct before returning.
 */
bool array_list_remove_element(ArrayList* array_list, const void* element);

/**
 * @brief Removes all elements of a collection present in the array list.
 *
 * @param array_list pointer to an array list
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note this function calls the element destruct before returning.
 */
int array_list_remove_all(ArrayList* array_list, Collection collection);

/**
 * @brief Removes all elements at the specified range of the array list.
 *
 * @param array_list pointer to an array list
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > array_list.size || start_index > end_index
 *
 * @note this function calls the element destruct before returning.
 */
int array_list_remove_range(ArrayList* array_list, int start_index, int end_index);

/**
 * @brief Removes all elements of the array list matching a predicate.
 *
 * @param array_list pointer to an array list
 * @param condition condition to remove
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if array_list or condition is null
 */
int array_list_remove_if(ArrayList* array_list, Predicate condition);

/**
 * @brief Replaces all elements using an operator function of the array list.
 *
 * @param array_list pointer to an array list
 * @param operator operator function
 *
 * @exception NULL_POINTER_ERROR if array_list or operator is null
 *
 * @note this function calls the element destruct before returning.
 */
void array_list_replace_all(ArrayList* array_list, Operator operator);

/**
 * @brief Retains all elements of a collection present in the array list while removing all other elements.
 *
 * @param array_list pointer to an array list
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note this function calls the element destruct before returning.
 */
int array_list_retain_all(ArrayList* array_list, Collection collection);

/**
 * @brief Returns the current number of elements in the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
int array_list_size(const ArrayList* array_list);

/**
 * @brief Shrink the current capacity to match the current size of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 *
 * @note this function will not shrink below MIN_CAPACITY.
 */
void array_list_trim_to_size(ArrayList* array_list);

/**
 * @brief Returns the current capacity of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
int array_list_capacity(const ArrayList* array_list);

/**
 * @brief Ensures the array list has the specified sufficient capacity, expanding if necessary.
 *
 * @param array_list pointer to an array list
 * @param capacity the required capacity
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing fails
 */
void array_list_ensure_capacity(ArrayList* array_list, int capacity);

/**
 * @brief Checks whether the array list is empty.
 *
 * @param array_list pointer to an array list
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
bool array_list_is_empty(const ArrayList* array_list);

/**
 * @brief Creates an iterator for the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator supports all operations
 */
Iterator* array_list_iterator(const ArrayList* array_list);

/**
 * @brief Checks whether two array lists are equal.
 *
 * Two array lists are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first array list
 *
 * @param array_list first array list
 * @param other_array_list second array list
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either array list is null
 */
bool array_list_equals(const ArrayList* array_list, const ArrayList* other_array_list);

/**
 * @brief Applies an action to each element of the array list.
 *
 * @param array_list pointer to an array list
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if array_list or action is null
 */
void array_list_for_each(ArrayList* array_list, Consumer action);

/**
 * @brief Sorts the elements of the array list according to a comparator and a sorting algorithm.
 *
 * @param array_list pointer to an array list
 * @param comparator comparator function
 * @param algorithm sorting algorithm
 *
 * @exception NULL_POINTER_ERROR if array_list or comparator is null
 * @exception MEMORY_ALLOCATION_ERROR if merge sort is chosen and memory allocation for temporary arrays fails
 */
void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm);

/**
 * @brief Shuffles the elements of the array list according to an RNG function and a shuffle algorithm.
 *
 * @param array_list pointer to an array list
 * @param random random number generator function
 * @param algorithm shuffling algorithm
 *
 * @exception NULL_POINTER_ERROR if array_list or random is null
 */
void array_list_shuffle(ArrayList* array_list, int (*random)(void), ShufflingAlgorithm algorithm);

/**
 * @brief Reverses the elements of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
void array_list_reverse(ArrayList* array_list);

/**
 * @brief Rotates the elements of the array list by the specified distance.
 *
 * @param array_list pointer to an array list
 * @param distance number of positions to shift (can be negative)
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
void array_list_rotate(ArrayList* array_list, int distance);

/**
 * @brief Removes all elements of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note this function calls the element destruct before returning.
 */
void array_list_clear(ArrayList* array_list);

/**
 * @brief Finds the first element of the array list matching a predicate.
 *
 * @param array_list pointer to an array list
 * @param condition condition to test
 *
 * @return an optional containing the result
 *
 * @exception NULL_POINTER_ERROR if array_list or condition is null
 */
Optional array_list_find(const ArrayList* array_list, Predicate condition);

/**
 * @brief Finds the last element of the array list matching a predicate.
 *
 * @param array_list pointer to an array list
 * @param condition condition to test
 *
 * @return an optional containing the result
 *
 * @exception NULL_POINTER_ERROR if array_list or condition is null
 */
Optional array_list_find_last(const ArrayList* array_list, Predicate condition);

/**
 * @brief Retrieves the index of the first element of the array list matching a predicate.
 *
 * @param array_list pointer to an array list
 * @param condition condition to test
 *
 * @return the first element index, or -1 if no match
 *
 * @exception NULL_POINTER_ERROR if array_list or condition is null
 */
int array_list_index_where(const ArrayList* array_list, Predicate condition);

/**
 * @brief Retrieves the index of the last element of the array list matching a predicate.
 *
 * @param array_list pointer to an array list
 * @param condition condition to test
 *
 * @return the last element index, or -1 if no match
 *
 * @exception NULL_POINTER_ERROR if array_list or condition is null
 */
int array_list_last_index_where(const ArrayList* array_list, Predicate condition);

/**
 * @brief Checks whether an element is present in the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
bool array_list_contains(const ArrayList* array_list, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the array list.
 *
 * @param array_list pointer to an array list
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool array_list_contains_all(const ArrayList* array_list, Collection collection);

/**
 * @brief Counts the number of occurrences of an element in the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to be counted
 *
 * @return the number of occurrences
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
int array_list_occurrences_of(const ArrayList* array_list, const void* element);

/**
 * @brief Retrieves the index of the first occurrence of the specified element in the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to search
 *
 * @return the element first index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
int array_list_index_of(const ArrayList* array_list, const void* element);

/**
 * @brief Retrieves the index of the last occurrence of the specified element in the array list.
 *
 * @param array_list pointer to an array list
 * @param element element to search
 *
 * @return the element last index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
int array_list_last_index_of(const ArrayList* array_list, const void* element);

/**
 * @brief Performs a binary search for the specified element in the array list
 *
 * @param array_list pointer to an array list
 * @param element element to search
 * @param comparator comparator function
 *
 * @return the element index, or -1 if not found
 *
 * @exception NULL_POINTER_ERROR if array_list or comparator is null
 *
 * @note the array list must be sorted before calling this function, otherwise, it is undefined behavior.
 */
int array_list_binary_search(const ArrayList* array_list, const void* element, Comparator comparator);

/**
 * @brief Creates a shallow copy of the array list.
 *
 * The new array list shares element pointers but has independent storage.
 *
 * @param array_list pointer to an array list
 *
 * @return pointer to the newly created array list
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
ArrayList* array_list_clone(const ArrayList* array_list);

/**
 * @brief Creates a sublist of the array list within the specified range.
 *
 * @param array_list pointer to an array list
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return pointer to the newly created array list
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > array_list.size || start_index > end_index
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
ArrayList* array_list_sub_list(const ArrayList* array_list, int start_index, int end_index);

/**
 * @brief Returns a Collection view of the array list.
 *
 * The returned collection does not own the elements.
 *
 * @param array_list pointer to an array list
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 */
Collection array_list_to_collection(const ArrayList* array_list);

/**
 * @brief Returns a newly allocated array containing all elements of the array list.
 *
 * @param array_list pointer to an array list
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
void** array_list_to_array(const ArrayList* array_list);

/**
 * @brief Converts the array list to a string representation.
 *
 * @param array_list pointer to an array list
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* array_list_to_string(const ArrayList* array_list);

#endif