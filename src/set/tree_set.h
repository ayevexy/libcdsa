#ifndef TREE_SET_H
#define TREE_SET_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A tree set is an ordered data structure that stores unique elements using a
 * comparator function to keep them sorted. It allows insertion of elements of any type.
 *
 * Internally, the implementation uses a self-balancing binary search tree.
 * All operations receive a pointer to the tree set as their first argument.
 * The TreeSet type is opaque and can only be modified through the public API.
 *
 * A tree set must be configured using a TreeSetOptions structure specifying:
 * - the element comparator function
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct TreeSet {
 *      Node* root;
 *      int size;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the tree set does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., tree_set_remove, tree_set_remove_all)
 * - the tree set is cleared (tree_set_clear)
 * - the tree set is destroyed (tree_set_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - tree_set_add: O(log n)
 * - tree_set_remove: O(log n)
 * - tree_set_get_first / tree_set_get_last: O(log n)
 * - tree_set_contains: O(log n)
 */
typedef struct TreeSet TreeSet;

/**
 * TreeSet configuration structure. Defines the behavior and attributes of a tree set.
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
 * @brief Utility macro providing default TreeSetOptions.
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
 * @brief Creates a new empty tree set using the specified options.
 *
 * @param options pointer to a TreeSetOptions structure
 *
 * @return pointer to a newly created tree set
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_new(const TreeSetOptions* options);

/**
 * @brief Creates a new tree set containing all elements of the given collection.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created tree set
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
TreeSet* tree_set_from(Collection collection, const TreeSetOptions* options);

/**
 * @brief Destroys a tree set and optionally its elements.
 *
 * @param tree_set_pointer pointer to a tree set pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set_pointer or *tree_set_pointer are null
 *
 * @post *tree_set_pointer == nullptr
 */
void tree_set_destroy(TreeSet** tree_set_pointer);

/**
 * @brief Sets the element destruct function.
 *
 * @param tree_set pointer to a tree set
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if tree_set or destruct is null
 */
void tree_set_set_destructor(TreeSet* tree_set, void (*destruct)(void*));

/**
 * @brief Inserts an element in the tree set.
 *
 * @param tree_set pointer to a tree set
 * @param element element to insert
 *
 * @return true if the element was inserted, false if already present
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new node fails
 */
bool tree_set_add(TreeSet* tree_set, const void* element);

/**
 * @brief Inserts all elements of a collection in the tree set.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param tree_set pointer to a tree set
 * @param collection source collection
 *
 * @return true if the tree set was changed, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing, creating a new node or creation of the collection iterator fails
 */
bool tree_set_add_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Retrieves the first element of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return first element
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_get_first(const TreeSet* tree_set);

/**
 * @brief Retrieves the last element of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return last element
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 */
void* tree_set_get_last(const TreeSet* tree_set);

/**
 * @brief Removes the specified element (if present) of the tree set.
 *
 * @param tree_set pointer to a tree set
 * @param element element to remove
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 *
 * @note this function calls the element destruct before returning.
 */
bool tree_set_remove(TreeSet* tree_set, const void* element);

/**
 * @brief Removes and returns the first element of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* tree_set_remove_first(TreeSet* tree_set);

/**
 * @brief Removes and returns the last element of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return pointer to the removed element
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_set is empty
 *
 * @note this function calls the element destruct before returning.
 *       If the destruct frees the element, the returned pointer becomes invalid.
 */
void* tree_set_remove_last(TreeSet* tree_set);

/**
 * @brief Removes all elements of a collection present in the tree set.
 *
 * @param tree_set pointer to a tree set
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 *
 * @note this function calls the element destruct before returning.
 */
int tree_set_remove_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Removes all elements of the tree set matching a predicate.
 *
 * @param tree_set pointer to a tree set
 * @param condition condition to remove
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set or condition is null
 *
 * @note this function calls the element destruct before returning.
 */
int tree_set_remove_if(TreeSet* tree_set, Predicate condition);

/**
 * @brief Retains all elements of a collection present in the tree set while removing all other elements.
 *
 * @param tree_set pointer to a tree set
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 *
 * @note this function calls the element destruct before returning.
 */
int tree_set_retain_all(TreeSet* tree_set, Collection collection);

/**
 * @brief Returns the current number of elements in the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
int tree_set_size(const TreeSet* tree_set);

/**
 * @brief Checks whether the tree set is empty.
 *
 * @param tree_set pointer to a tree set
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
bool tree_set_is_empty(const TreeSet* tree_set);

/**
 * @brief Creates an iterator for the tree set.
 *
 * The iterator traverses elements in sorted order.
 *
 * @param tree_set pointer to a tree set
 *
 * @return pointer to a newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add() and set()
 */
Iterator* tree_set_iterator(const TreeSet* tree_set);

/**
 * @brief Checks whether two tree sets are equal.
 *
 * Two tree sets are equal if:
 * - they reference the same memory address, or
 * - they have the same size, and each element in the first tree set is present in the second
 *   tree set according to the equals function of the first tree set.
 *
 * @param tree_set first tree set
 * @param other_tree_set second tree set
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either tree_set is null
 */
bool tree_set_equals(const TreeSet* tree_set, const TreeSet* other_tree_set);

/**
 * @brief Applies an action to each entry of the tree set.
 *
 * @param tree_set pointer to a tree set
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if tree_set or action is null
 */
void tree_set_for_each(TreeSet* tree_set, Consumer action);

/**
 * @brief Removes all elements of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 *
 * @note this function calls the element destruct before returning.
 */
void tree_set_clear(TreeSet* tree_set);

/**
 * @brief Retrieves the least element strictly greater than the specified element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the element
 *
 * @return the element pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_higher(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the least element greater than or equal to the specified element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the element
 *
 * @return the element pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_ceiling(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the greatest element less than or equal to the specified element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the element
 *
 * @return the element pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_floor(const TreeSet* tree_set, const void* element);

/**
 * @brief Retrieves the greatest element strictly less than the specified element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the element
 *
 * @return the element pointer
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
void* tree_set_lower(const TreeSet* tree_set, const void* element);

/**
 * @brief Checks whether an element is present in the tree set.
 *
 * @param tree_set pointer to a tree set
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
bool tree_set_contains(const TreeSet* tree_set, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the tree set.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param tree_set pointer to a tree set
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool tree_set_contains_all(const TreeSet* tree_set, Collection collection);

/**
 * @brief Creates a view of the portion of the tree set whose elements are less than or equal to the given element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the exclusive upper bound element
 *
 * @return pointer to the newly created tree set
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if element is nonexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_head_set(const TreeSet* tree_set, const void* element);

/**
 * @brief Creates a view of the portion of the tree set whose elements are greater than or equal to the given element.
 *
 * @param tree_set pointer to a tree set
 * @param element pointer to the inclusive lower bound element
 *
 * @return pointer to the newly created tree set
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if element is nonexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_tail_set(const TreeSet* tree_set, const void* element);

/**
 * @brief Creates a shallow copy of the tree set.
 *
 * The new tree set shares element pointers but has independent storage.
 *
 * @param tree_set pointer to a tree set
 *
 * @return pointer to the newly created tree set
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_clone(const TreeSet* tree_set);

/**
 * @brief Creates a subset of the tree set within the specified range.
 *
 * @param tree_set pointer to a tree set
 * @param start_element start element (inclusive)
 * @param end_element end element (exclusive)
 *
 * @return pointer to the newly created tree set
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception ILLEGAL_ARGUMENT_ERROR if start_element or end_element are nonexistent or start_element is greater than end_element
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeSet* tree_set_sub_set(const TreeSet* tree_set, const void* start_element, const void* end_element);

/**
 * @brief Returns a Collection view of the tree set.
 *
 * The returned collection does not own the elements.
 *
 * @param tree_set pointer to a tree set
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 */
Collection tree_set_to_collection(const TreeSet* tree_set);

/**
 * @brief Returns a newly allocated array containing all elements of the tree set.
 *
 * @param tree_set pointer to a tree set
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
void** tree_set_to_array(const TreeSet* tree_set);

/**
 * @brief Converts the tree set to a string representation.
 *
 * @param tree_set pointer to a tree set
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if tree_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* tree_set_to_string(const TreeSet* tree_set);

#endif