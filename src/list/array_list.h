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
 * An array list is a linear, generic, and dynamic data structure that stores data contiguously in memory.
 * It automatically grows to accommodate new elements and allows insertion of values of any type.
 *
 * Internally, the implementation uses a struct containing an array of void pointers. All operations on an array list
 * receive a pointer to the array list itself as their first argument. The `ArrayList` type is opaque and can
 * only be modified through the API.
 *
 * It must be configured using an `ArrayListOptions` structure defining:
 * - its initial capacity
 * - its growth factor
 * - the equals function utilized to compare elements
 * - the to string function utilized to convert its elements to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to reallocate memory
 * - the function used internally to free memory
 *
 * Underlying implementation (simplified):
 * @code
 * struct ArrayList {
 *      void** elements;
 *      int size;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to `stderr`),
 * or by returning an error object when wrapped with the `attempt` macro.
 */
typedef struct ArrayList ArrayList;

/**
 * `ArrayList` configuration structure. Used to define the default behavior and attributes of an `ArrayList`.
 *
 * To be valid, it must adhere to the following constraints:
 * - `initial_capacity` >= 10 and < INT_MAX
 * - `growth_factor` >= 1.1
 * - `equals` != nullptr
 * - `to_string` != nullptr
 * - `memory_alloc` != nullptr
 * - `memory_realloc` != nullptr
 * - `memory_free` != nullptr
 */
typedef struct ArrayListOptions {
    int initial_capacity;
    double growth_factor;
    bool (*equals)(const void*, const void*);
    int (*to_string)(const void*, char*, size_t);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} ArrayListOptions;

/**
 * A utility macro that provides a reasonable default `ArrayListOptions` structure.
 */
#define DEFAULT_ARRAY_LIST_OPTIONS(...) &(ArrayListOptions) {   \
    .initial_capacity = 10,                                     \
    .growth_factor = 2.0,                                       \
    .equals = pointer_equals,                                   \
    .to_string = pointer_to_string,                             \
    .memory_alloc = malloc,                                     \
    .memory_realloc = realloc,                                  \
    .memory_free = free,                                        \
    __VA_ARGS__                                                 \
}

/**
 * @brief Creates a new empty `ArrayList` using the specified options.
 *
 * @param options pointer to an `ArrayListOptions` structure containing configuration settings
 *
 * @return Pointer to a newly created `ArrayList` on success, or `nullptr` if creation fails
 *
 * @exception ILLEGAL_ARGUMENT_ERROR If `options` violates its constraints
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for the new `ArrayList` fails
 *
 * @pre options != `nullptr`
 */
ArrayList* array_list_new(const ArrayListOptions* options);

/**
 * @brief Creates a new `ArrayList` containing all elements of the given collection using the specified options.
 *
 * @param collection a `Collection` structure containing the elements to be added
 * @param options pointer to an `ArrayListOptions` structure containing configuration settings
 *
 * @return pointer to a newly created `ArrayList` on success, or `nullptr` if creation fails
 *
 * @exception ILLEGAL_ARGUMENT_ERROR If `options` violates its constraints
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for the new `ArrayList` of `collection` iterator fails
 *
 * @pre options != `nullptr`
 */
ArrayList* array_list_from(Collection collection, const ArrayListOptions* options);

/**
 * @brief Deletes an existing `ArrayList` while keeping its elements intact.
 *
 * @param array_list_pointer pointer to an `ArrayList*`
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note after usage, array_list will be set to `nullptr`.
 *
 * @pre array_list_pointer != `nullptr`
 */
void array_list_delete(ArrayList** array_list_pointer);

/**
 * @brief Deletes an existing `ArrayList` and all its elements using the provided delete function.
 *
 * @param array_list_pointer pointer to an `ArrayList*`
 * @param delete clean up function to apply to every element
 *
 * @exception NULL_POINTER_ERROR if array_list is null
 *
 * @note after usage, array_list will be set to `nullptr`
 *
 * @pre array_list_pointer != `nullptr`
 * @pre delete != `nullptr`
 */
void array_list_destroy(ArrayList** array_list_pointer, void (*delete)(void*));

/**
 * @brief Inserts the specified element at the specified position in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`.
 * @param index index at which the specified element is to be inserted
 * @param element pointer to the element to be inserted
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > array_list_size()
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand array_list capacity
 *
 * @pre array_list != `nullptr`
 */
void array_list_add(ArrayList* array_list, int index, const void* element);

/**
 * @brief Inserts the specified element at the beginning of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to be inserted
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand array_list capacity
 *
 * @pre array_list != `nullptr`
 */
void array_list_add_first(ArrayList* array_list, const void* element);

/**
 * @brief Inserts the specified element at the end of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to be inserted
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand array_list capacity
 *
 * @pre array_list != `nullptr`
 */
void array_list_add_last(ArrayList* array_list, const void* element);

/**
 * @brief Inserts all elements of the specified `Collection` at the specified position in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param index start index at which the elements will be inserted
 * @param collection a `Collection` structure containing the elements to be added
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > array_list_size()
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for the new `ArrayList` of `collection` iterator fails.
 *
 * @pre array_list != `nullptr`
 */
void array_list_add_all(ArrayList* array_list, int index, Collection collection);

/**
 * @brief Inserts all elements of the specified `Collection` at the beginning of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param collection a `Collection` structure containing the elements to be added
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for the new `ArrayList` of `collection` iterator fails.
 *
 * @pre array_list != `nullptr`
 */
void array_list_add_all_first(ArrayList* array_list, Collection collection);

/**
 * @brief Inserts all elements of the specified `Collection` at the end of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param collection a `Collection` structure containing the elements to be added
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for the new `ArrayList` of `collection` iterator fails.
 *
 * @pre array_list != `nullptr`
 */
void array_list_add_all_last(ArrayList* array_list, Collection collection);

/**
 * @brief Retrieves the element at the specified position of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param index index which the element will be retrieved
 *
 * @return pointer to the retrieved element
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list_size()
 *
 * @pre array_list != `nullptr`
 */
void* array_list_get(const ArrayList* array_list, int index);

/**
 * @brief Retrieves the first element of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return pointer to the retrieved element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if the provided `ArrayList` is empty
 *
 * @pre array_list != `nullptr`
 */
void* array_list_get_first(const ArrayList* array_list);

/**
 * @brief Retrieves the last element of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return pointer to the retrieved element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if the provided `ArrayList` is empty
 *
 * @pre array_list != `nullptr`
 */
void* array_list_get_last(const ArrayList* array_list);

/**
 * @brief Replaces the element at the specified position of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param index index of the element to be replaced
 * @param element the new element
 *
 * @return pointer to the old element
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list_size()
 *
 * @pre array_list != `nullptr`
 */
void* array_list_set(ArrayList* array_list, int index, const void* element);

/**
 * @brief Swaps the elements at specified positions of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param index_a index of the first element to be swapped
 * @param index_b index of the second element to be swapped
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list_size()
 *
 * @pre array_list != `nullptr`
 */
void array_list_swap(ArrayList* array_list, int index_a, int index_b);

/**
 * @brief Removes the element at the specified position of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param index index of the element to be removed
 *
 * @return pointer to the removed element
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= array_list_size()
 *
 * @pre array_list != `nullptr`
 */
void* array_list_remove(ArrayList* array_list, int index);

/**
 * @brief Removes the first element of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return pointer to the removed element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if the provided `ArrayList` is empty
 *
 * @pre array_list != `nullptr`
 */
void* array_list_remove_first(ArrayList* array_list);

/**
 * @brief Removes the last element of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return pointer to the removed element
 *
 * @exception NO_SUCH_ELEMENT_ERROR if the provided `ArrayList` is empty
 *
 * @pre array_list != `nullptr`
 */
void* array_list_remove_last(ArrayList* array_list);

/**
 * @brief Removes the specified element (if present) of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to be removed
 *
 * @return `true` if removed, `false` if not present
 *
 * @pre array_list != `nullptr`
 */
bool array_list_remove_element(ArrayList* array_list, const void* element);

/**
 * @brief Removes all elements of the given collection present in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param collection a `Collection` structure containing the elements to be removed
 *
 * @return number of elements removed
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for`collection` iterator fails
 *
 * @pre array_list != `nullptr`
 */
int array_list_remove_all(ArrayList* array_list, Collection collection);

/**
 * @brief Removes all elements at the specified range in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param start_index start index of the range
 * @param end_index end index of the range
 *
 * @return number of elements removed
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > array_list_size() || start_index > end_index
 *
 * @pre array_list != `nullptr`
 */
int array_list_remove_range(ArrayList* array_list, int start_index, int end_index);

/**
 * @brief Removes all elements matching the given `Predicate` in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param condition the condition to remove elements
 *
 * @return number of elements removed
 *
 * @pre array_list != `nullptr`
 * @pre condition != `nullptr`
 */
int array_list_remove_if(ArrayList* array_list, Predicate condition);

/**
 * @brief Replaces all elements using the given `Operator` of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param operator the operator to replace elements
 *
 * @pre array_list != `nullptr`
 * @pre operator != `nullptr`
 */
void array_list_replace_all(ArrayList* array_list, Operator operator);

/**
 * @brief Retains all elements of the given collection present in the provided `ArrayList` while removing all other elements.
 *
 * @param array_list pointer to an `ArrayList`
 * @param collection a `Collection` structure containing the elements to be hold
 *
 * @return number of elements removed
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for`collection` iterator fails
 *
 * @pre array_list != `nullptr`
 */
int array_list_retain_all(ArrayList* array_list, Collection collection);

/**
 * @brief Retrieves the current size of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return the current size of the provided `ArrayList`
 *
 * @pre array_list != `nullptr`
 */
int array_list_size(const ArrayList* array_list);

/**
 * @brief Trims the current capacity to match the minimum size of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to shrink `array_list` capacity
 *
 * @pre array_list != `nullptr`
 */
void array_list_trim_to_size(ArrayList* array_list);

/**
 * @brief Retrieves the current capacity of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return the current capacity of the provided `ArrayList`
 *
 * @pre array_list != `nullptr`
 */
int array_list_capacity(const ArrayList* array_list);

/**
 * @brief Ensures the provided `ArrayList` has sufficient capacity, if not, it will expand.
 *
 * @param array_list pointer to an `ArrayList`
 * @param capacity the required capacity
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand `array_list` capacity
 *
 * @pre array_list != `nullptr`
 */
void array_list_ensure_capacity(ArrayList* array_list, int capacity);

/**
 * @brief Checks whether the provided `ArrayList` is empty.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return `true` if empty, `false` otherwise
 *
 * @pre array_list != `nullptr`
 */
bool array_list_is_empty(const ArrayList* array_list);

/**
 * @brief Instantiates an `Iterator` for the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return pointer to the newly created `Iterator`
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for `iterator`
 *
 * @pre array_list != `nullptr`
 */
Iterator* array_list_iterator(const ArrayList* array_list);

/**
 * @brief Checks whether two `ArrayList` objects are equals.
 *
 * Two `ArrayList` are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and each corresponding element in the first array list is considered equal to the element
 * at the same position in the second array list according to the `equals` function of the first array list.
 *
 * @param array_list pointer to an `ArrayList`
 * @param other_array_list pointer to an `ArrayList`
 *
 * @return `true` if equals, `false` otherwise
 *
 * @pre array_list != `nullptr`
 * @pre other_array_list != `nullptr`
 */
bool array_list_equals(const ArrayList* array_list, const ArrayList* other_array_list);

/**
 * @brief Performs an action for each element of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param action the action to be performed
 *
 * @pre array_list != `nullptr`
 * @pre action != `nullptr`
 */
void array_list_for_each(ArrayList* array_list, Consumer action);

/**
 * @brief Sorts using the given `Comparator` and `SortingAlgorithm` the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param comparator the comparator to be used to compare elements
 * @param algorithm the algorithm used to sort the `ArrayList`
 *
 * @pre array_list != `nullptr`
 * @pre comparator != `nullptr`
 */
void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm);

/**
 * @brief Shuffles using the given `random` function and `ShufflingAlgorithm` the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param random a function to generate random numbers
 * @param algorithm the algorithm used to shuffle the `ArrayList`
 *
 * @pre array_list != `nullptr`
 * @pre random != `nullptr`
 */
void array_list_shuffle(ArrayList* array_list, int (*random)(void), ShufflingAlgorithm algorithm);

/**
 * @brief Reverses the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @pre array_list != `nullptr`
 */
void array_list_reverse(ArrayList* array_list);

/**
 * @brief Rotates using the given distance the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param distance how many positions to shift
 *
 * @pre array_list != `nullptr`
 */
void array_list_rotate(ArrayList* array_list, int distance);

/**
 * @brief Removes all elements of the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @pre array_list != `nullptr`
 */
void array_list_clear(ArrayList* array_list);

/**
 * @brief Removes all elements of the provided `ArrayList` applying the provided cleanup function for each element.
 *
 * @param array_list pointer to an `ArrayList`
 * @param delete clean up function to apply to every element
 *
 * @pre array_list != `nullptr`
 * @pre delete != `nullptr`
 */
void array_list_clear_data(ArrayList* array_list, void (*delete)(void*));

/**
 * @brief Finds the first element matching the given `Predicate` in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param condition the predicate used to test elements
 *
 * @return an `Optional` containing the found element, or an empty `Optional` if no match is found
 *
 * @pre array_list != `nullptr`
 * @pre condition != `nullptr`
 */
Optional array_list_find(const ArrayList* array_list, Predicate condition);

/**
 * @brief Finds the last element matching the given `Predicate` in the provided `ArrayList`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param condition the predicate used to test elements
 *
 * @return an `Optional` containing the found element, or an empty `Optional` if no match is found
 *
 * @pre array_list != `nullptr`
 * @pre condition != `nullptr`
 */
Optional array_list_find_last(const ArrayList* array_list, Predicate condition);

/**
 * @brief Retrieves the index of the first element matching the given `Predicate`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param condition the predicate used to test elements
 *
 * @return the index of the first matching element, or `-1` if no match is found
 *
 * @pre array_list != `nullptr`
 * @pre condition != `nullptr`
 */
int array_list_index_where(const ArrayList* array_list, Predicate condition);

/**
 * @brief Retrieves the index of the last element matching the given `Predicate`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param condition the predicate used to test elements
 *
 * @return the index of the last matching element, or `-1` if no match is found
 *
 * @pre array_list != `nullptr`
 * @pre condition != `nullptr`
 */
int array_list_last_index_where(const ArrayList* array_list, Predicate condition);

/**
 * @brief Checks whether the provided `ArrayList` contains the specified element.
 *
 * Comparison is performed using the `equals` function configured in the `ArrayListOptions`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to be checked
 *
 * @return `true` if the element is present, `false` otherwise
 *
 * @pre array_list != `nullptr`
 */
bool array_list_contains(const ArrayList* array_list, const void* element);

/**
 * @brief Checks whether the provided `ArrayList` contains all elements of the given collection.
 *
 * @param array_list pointer to an `ArrayList`
 * @param collection a `Collection` structure containing elements to be checked
 *
 * @return `true` if all elements are present, `false` otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation for `collection` iterator fails
 *
 * @pre array_list != `nullptr`
 */
bool array_list_contains_all(const ArrayList* array_list, Collection collection);

/**
 * @brief Counts the number of occurrences of the specified element in the provided `ArrayList`.
 *
 * Comparison is performed using the `equals` function configured in the `ArrayListOptions`.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to be counted
 *
 * @return the number of occurrences of the specified element
 *
 * @pre array_list != `nullptr`
 */
int array_list_occurrences_of(const ArrayList* array_list, const void* element);

/**
 * @brief Retrieves the index of the first occurrence of the specified element.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to search for
 *
 * @return the index of the first occurrence, or `-1` if not found
 *
 * @pre array_list != `nullptr`
 */
int array_list_index_of(const ArrayList* array_list, const void* element);

/**
 * @brief Retrieves the index of the last occurrence of the specified element.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to search for
 *
 * @return the index of the last occurrence, or `-1` if not found
 *
 * @pre array_list != `nullptr`
 */
int array_list_last_index_of(const ArrayList* array_list, const void* element);

/**
 * @brief Performs a binary search for the specified element in the provided `ArrayList`.
 *
 * The `ArrayList` must be sorted according to the same `Comparator` prior to calling this function.
 *
 * @param array_list pointer to an `ArrayList`
 * @param element pointer to the element to search for
 * @param comparator the comparator used to compare elements
 *
 * @return the index of the found element, or `-1` if not found
 *
 * @pre array_list != `nullptr`
 * @pre comparator != `nullptr`
 */
int array_list_binary_search(const ArrayList* array_list, const void* element, Comparator comparator);

/**
 * @brief Creates a shallow copy of the provided `ArrayList`.
 *
 * The new `ArrayList` will contain the same element pointers but will have independent internal storage.
 *
 * @param array_list pointer to an `ArrayList`, or `nullptr` on failure
 *
 * @return a newly created `ArrayList` clone
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 *
 * @pre array_list != `nullptr`
 */
ArrayList* array_list_clone(const ArrayList* array_list);

/**
 * @brief Creates a sub list of the provided `ArrayList` within the specified range.
 *
 * The returned `ArrayList` contains elements from `start_index` (inclusive)
 * to `end_index` (exclusive).
 *
 * @param array_list pointer to an `ArrayList`
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return a newly created `ArrayList` containing the specified range, or `nullptr` on failure
 *
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > array_list_size() || start_index > end_index
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 *
 * @pre array_list != `nullptr`
 */
ArrayList* array_list_sub_list(const ArrayList* array_list, int start_index, int end_index);

/**
 * @brief Converts the provided `ArrayList` into a `Collection` view.
 *
 * The returned `Collection` does not own the underlying elements.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return a `Collection` representation of the `ArrayList`
 *
 * @pre array_list != `nullptr`
 */
Collection array_list_to_collection(const ArrayList* array_list);

/**
 * @brief Converts the provided `ArrayList` into a newly allocated array.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return a newly allocated array containing all elements
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 *
 * @pre array_list != `nullptr`
 */
void** array_list_to_array(const ArrayList* array_list);

/**
 * @brief Converts the provided `ArrayList` to a string representation.
 *
 * Each element is converted using the `to_string` function configured
 * in the `ArrayListOptions`.
 *
 * @param array_list pointer to an `ArrayList`
 *
 * @return a newly allocated null-terminated string representation, or `nullptr` on failure
 *
 * @exception MEMORY_ALLOCATION_ERROR If memory allocation fails
 *
 * @pre array_list != `nullptr`
 */
char* array_list_to_string(const ArrayList* array_list);

#endif