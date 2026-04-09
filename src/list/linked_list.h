#ifndef LIBCDSA_LINKED_LIST_H
#define LIBCDSA_LINKED_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"
#include "util/optional.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A linked list is a generic dynamic linear data structure where elements are
 * stored in nodes allocated non-contiguously in memory.
 * It supports storing elements of any type, including null pointers.
 *
 * Internally, the list maintains pointers to the head and tail nodes, and
 * each node links to both its previous and next node (doubly linked).
 * All operations receive a pointer to the linked list as their first argument.
 * The LinkedList type is opaque and can only be modified through the public API.
 *
 * A linked list must be configured using a LinkedListOptions structure specifying:
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory deallocation function
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
 * Memory ownership:
 * By default, the linked list does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., linked_list_remove, linked_list_remove_all)
 * - elements are replaced (e.g., linked_list_set, linked_list_replace_all)
 * - the linked list is cleared (linked_list_clear)
 * - the linked list is destroyed (linked_list_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - linked_list_add_first / linked_list_add_last: O(1)
 * - linked_list_add: O(n)
 * - linked_list_remove_first / linked_list_remove_last: O(1)
 * - linked_list_remove: O(n)
 * - linked_list_get_first / linked_list_get_last: O(1)
 * - linked_list_get: O(n)
 * - linked_list_set: O(n)
 * - linked_list_contains / linked_list_index_of: O(n)
 */
typedef struct LinkedList LinkedList;

/**
 * LinkedList configuration structure. Defines the behavior and attributes of a linked list.
 *
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_dealloc != nullptr
 */
typedef struct {
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_dealloc)(void*);
    };
} LinkedListOptions;

/**
 * @brief Utility macro providing default LinkedListOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_LINKED_LIST_OPTIONS(...) &(LinkedListOptions) {     \
    .destruct = noop_destruct,                                      \
    .equals = pointer_equals,                                       \
    .to_string = pointer_to_string,                                 \
    .memory_alloc = malloc,                                         \
    .memory_dealloc = free,                                         \
    __VA_ARGS__                                                     \
}

/**
 * @brief Creates a new empty linked list using the specified options.
 *
 * @param options pointer to an LinkedListOptions structure
 *
 * @return pointer to a newly created linked list
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
LinkedList* linked_list_new(const LinkedListOptions* options);

/**
 * @brief Creates a new linked list containing all elements of the given collection.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created linked list
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
LinkedList* linked_list_from(Collection collection, const LinkedListOptions* options);

/**
 * @brief Destroys a linked list and optionally its elements.
 *
 * @param linked_list_pointer pointer to a linked list pointer
 *
 * @exception NULL_POINTER_ERROR if linked_list_pointer or *linked_list_pointer is null
 *
 * @post *linked_list_pointer == nullptr
 */
void linked_list_destroy(LinkedList** linked_list_pointer);

/**
 * @brief Changes the element destruct function.
 *
 * @param linked_list pointer to a linked list
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if linked_list or destruct is null
 */
void linked_list_change_destructor(LinkedList* linked_list, void (*destruct)(void*));

/**
 * @brief Inserts an element at the specified position of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index insertion position
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > linked_list.size
 * @exception MEMORY_ALLOCATION_ERROR if node allocation fails
 */
void linked_list_add(LinkedList* linked_list, int index, const void* element);

/**
 * @brief Inserts an element at the beginning of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if node allocation fails
 */
void linked_list_add_first(LinkedList* linked_list, const void* element);

/**
 * @brief Inserts an element at the end of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to insert
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if node allocation fails
 */
void linked_list_add_last(LinkedList* linked_list, const void* element);

/**
 * @brief Inserts all elements of a collection at the specified position of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index insertion position
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index > linked_list.size
 * @exception MEMORY_ALLOCATION_ERROR if node allocation or creation of the collection iterator fails
 */
void linked_list_add_all(LinkedList* linked_list, int index, Collection collection);

/**
 * @brief Inserts all elements of a collection at the beginning of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if node allocation or creation of the collection iterator fails
 */
void linked_list_add_all_first(LinkedList* linked_list, Collection collection);

/**
 * @brief Inserts all elements of a collection at the end of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param collection source collection
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if node allocation or creation of the collection iterator fails
 */
void linked_list_add_all_last(LinkedList* linked_list, Collection collection);

/**
 * @brief Retrieves the element at the specified position of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index element position
 *
 * @return pointer to the element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list.size
 */
void* linked_list_get(const LinkedList* linked_list, int index);

/**
 * @brief Retrieves the first element of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to the first element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if linked_list is empty
 */
void* linked_list_get_first(const LinkedList* linked_list);

/**
 * @brief Retrieves the last element of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to the last element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if linked_list is empty
 */
void* linked_list_get_last(const LinkedList* linked_list);

/**
 * @brief Replaces the element at the specified position of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index element position
 * @param element the new element
 *
 * @return pointer to the old element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list.size
 * 
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the old element, the returned pointer becomes invalid.
 */
void* linked_list_set(LinkedList* linked_list, int index, const void* element);

/**
 * @brief Swaps the elements at specified positions of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index_a first element position
 * @param index_b second element position
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list.size
 */
void linked_list_swap(LinkedList* linked_list, int index_a, int index_b);

/**
 * @brief Removes and returns the element at the specified position of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param index element position
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if index < 0 || index >= linked_list.size
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* linked_list_remove(LinkedList* linked_list, int index);

/**
 * @brief Removes and returns the first element of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if linked_list is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* linked_list_remove_first(LinkedList* linked_list);

/**
 * @brief Removes and returns the last element of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception NO_SUCH_ELEMENT_ERROR if linked_list is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* linked_list_remove_last(LinkedList* linked_list);

/**
 * @brief Removes the specified element (if present) of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to remove
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 *
 * @note this function calls the element destruct before returning.
 */
bool linked_list_remove_element(LinkedList* linked_list, const void* element);

/**
 * @brief Removes all elements of a collection present in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 *
 * @note this function calls the element destruct before returning.
 */
int linked_list_remove_all(LinkedList* linked_list, Collection collection);

/**
 * @brief Removes all elements at the specified range of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > linked_list.size || start_index > end_index
 *
 * @note this function calls the element destruct before returning.
 */
int linked_list_remove_range(LinkedList* linked_list, int start_index, int end_index);

/**
 * @brief Removes all elements of the linked list matching a predicate.
 *
 * @param linked_list pointer to a linked list
 * @param condition condition to remove
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_remove_if(LinkedList* linked_list, Predicate condition);

/**
 * @brief Replaces all elements using an operator function of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param operator operator function
 *
 * @exception NULL_POINTER_ERROR if linked_list or operator is null
 *
 * @note this function calls the element destruct before returning.
 */
void linked_list_replace_all(LinkedList* linked_list, Operator operator);

/**
 * @brief Retains all elements of a collection present in the linked list while removing all other elements.
 *
 * @param linked_list pointer to a linked list
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 *
 * @note this function calls the element destruct before returning.
 */
int linked_list_retain_all(LinkedList* linked_list, Collection collection);

/**
 * @brief Returns the current number of elements in the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_size(const LinkedList* linked_list);

/**
 * @brief Checks whether the linked list is empty.
 *
 * @param linked_list pointer to a linked list
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
bool linked_list_is_empty(const LinkedList* linked_list);

/**
 * @brief Creates an iterator for the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator supports all operations
 */
Iterator* linked_list_iterator(const LinkedList* linked_list);

/**
 * @brief Creates an iterator for the linked list starting at the specified position.
 *
 * @param linked_list pointer to a linked list
 * @param position start position
 *
 * @return pointer to a newly created iterator
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if position < 0 || position > linked_list.size
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator supports all operations
 */
Iterator* linked_list_iterator_at(const LinkedList* linked_list, int position);

/**
 * @brief Checks whether two linked lists are equal.
 *
 * Two linked lists are equal if:
 * - they reference the same memory address, or
 * - they have the same size and corresponding elements are equal
 *   according to the configured equality function of the first linked list
 *
 * @param linked_list first linked list
 * @param other_linked_list second linked list
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either linked list is null
 */
bool linked_list_equals(const LinkedList* linked_list, const LinkedList* other_linked_list);

/**
 * @brief Applies an action to each element of the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if linked_list or action is null
 */
void linked_list_for_each(LinkedList* linked_list, Consumer action);

/**
 * @brief Sorts the elements of the linked list according to a comparator and a sorting algorithm.
 *
 * @param linked_list pointer to a linked list
 * @param comparator comparator function
 * @param algorithm sorting algorithm
 *
 * @exception NULL_POINTER_ERROR if linked_list or comparator is null
 */
void linked_list_sort(LinkedList* linked_list, Comparator comparator, SortingAlgorithm algorithm);

/**
 * @brief Shuffles the elements of the linked list according to an RNG function and a shuffle algorithm.
 *
 * @param linked_list pointer to a linked list
 * @param random random number generator function
 * @param algorithm shuffling algorithm
 *
 * @exception NULL_POINTER_ERROR if linked_list or random is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for creation of the temporary array fails
 */
void linked_list_shuffle(LinkedList* linked_list, int (*random)(void), ShufflingAlgorithm algorithm);

/**
 * @brief Reverses the elements of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
void linked_list_reverse(LinkedList* linked_list);

/**
 * @brief Rotates the elements of the linked list by the specified distance.
 *
 * @param linked_list pointer to a linked list
 * @param distance number of positions to shift (can be negative)
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
void linked_list_rotate(LinkedList* linked_list, int distance);

/**
 * @brief Removes all elements of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 *
 * @note this function calls the element destruct before returning.
 */
void linked_list_clear(LinkedList* linked_list);

/**
 * @brief Finds the first element of the linked list matching a predicate.
 *
 * @param linked_list pointer to a linked list
 * @param condition condition to test
 *
 * @return an optional containing the result
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
Optional linked_list_find(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Finds the last element of the linked list matching a predicate.
 *
 * @param linked_list pointer to a linked list
 * @param condition condition to test
 *
 * @return an optional containing the result
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
Optional linked_list_find_last(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Retrieves the index of the first element of the linked list matching a predicate.
 *
 * @param linked_list pointer to a linked list
 * @param condition condition to test
 *
 * @return the first element index, or -1 if no match
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_index_where(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Retrieves the index of the last element of the linked list matching a predicate.
 *
 * @param linked_list pointer to a linked list
 * @param condition condition to test
 *
 * @return the last element index, or -1 if no match
 *
 * @exception NULL_POINTER_ERROR if linked_list or condition is null
 */
int linked_list_last_index_where(const LinkedList* linked_list, Predicate condition);

/**
 * @brief Checks whether an element is present in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
bool linked_list_contains(const LinkedList* linked_list, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool linked_list_contains_all(const LinkedList* linked_list, Collection collection);

/**
 * @brief Applies an accumulator to the elements of the linked list and returns the reduced value.
 *
 * @param linked_list pointer to a linked list
 * @param identity initial value
 * @param accumulator accumulator function
 *
 * @return the result
 *
 * @exception NULL_POINTER_ERROR if linked_list or accumulator is null
 *
 * @note the returned pointer may point to the identity argument, which may be modified.
 */
void* linked_list_reduce(const LinkedList* linked_list, void* identity, BiOperator accumulator);

/**
 * @brief Counts the number of occurrences of an element in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to be counted
 *
 * @return the number of occurrences
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_occurrences_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Retrieves the index of the first occurrence of the specified element in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to search
 *
 * @return the element first index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_index_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Retrieves the index of the last occurrence of the specified element in the linked list.
 *
 * @param linked_list pointer to a linked list
 * @param element element to search
 *
 * @return the element last index, or -1 if not present
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
int linked_list_last_index_of(const LinkedList* linked_list, const void* element);

/**
 * @brief Creates a shallow copy of the linked list.
 *
 * The new linked list shares element pointers but has independent storage.
 *
 * @param linked_list pointer to a linked list
 *
 * @return pointer to the newly created linked list
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
LinkedList* linked_list_clone(const LinkedList* linked_list);

/**
 * @brief Creates a sublist of the linked list within the specified range.
 *
 * @param linked_list pointer to a linked list
 * @param start_index start index (inclusive)
 * @param end_index end index (exclusive)
 *
 * @return pointer to the newly created linked list
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if start_index < 0 || end_index > linked_list.size || start_index > end_index
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
LinkedList* linked_list_sub_list(const LinkedList* linked_list, int start_index, int end_index);

/**
 * @brief Returns a Collection view of the linked list.
 *
 * The returned collection does not own the elements.
 *
 * @param linked_list pointer to a linked list
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 */
Collection linked_list_to_collection(const LinkedList* linked_list);

/**
 * @brief Returns a newly allocated array containing all elements of the linked list.
 *
 * @param linked_list pointer to a linked list
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
void** linked_list_to_array(const LinkedList* linked_list);

/**
 * @brief Converts the linked list to a string representation.
 *
 * @param linked_list pointer to a linked list
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if linked_list is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* linked_list_to_string(const LinkedList* linked_list);

#endif