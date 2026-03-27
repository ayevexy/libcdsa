#ifndef HASH_SET_H
#define HASH_SET_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A hash set is a generic dynamic unordered data structure that stores elements
 * using a hashing function to ensure uniqueness.
 * It automatically resizes to accommodate new elements and supports storing
 * elements of any type, including null pointers.
 *
 * Internally, the implementation uses a dynamically allocated array of `Node*` (buckets).
 * Collisions are resolved by chaining nodes in a singly linked list per bucket.
 * All operations receive a pointer to the hash set as their first argument.
 * The HashSet type is opaque and can only be modified through the public API.
 *
 * A hash set must be configured using a HashSetOptions structure specifying:
 * - the initial capacity (must be a power of two, otherwise it will be automatically rounded to the next power of two)
 * - the load factor
 * - the hash function
 * - the destruct function used to free element memory
 * - the equality function used to compare elements
 * - the to_string function used to convert elements to strings
 * - the memory allocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct HashSet {
 *      Node** buckets;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the hash set does not own its elements. If a destruct function is
 * provided, it will be invoked when:
 * - elements are removed (e.g., hash_set_remove, hash_set_remove_all)
 * - the hash set is cleared (hash_set_clear)
 * - the hash set is destroyed (hash_set_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - hash_set_add: O(1) average, O(n) worst-case
 * - hash_set_remove: O(1) average, O(n) worst-case
 * - hash_set_contains: O(1) average, O(n) worst-case
 *
 * Note:
 * - Average-case O(1) assumes a good hash function and low load factor.
 * - Worst-case O(n) occurs due to hash collisions (e.g., all keys in one bucket).
 */
typedef struct HashSet HashSet;

/**
 * HashSet configuration structure. Defines the behavior and attributes of a hash set.
 *
 * @pre initial_capacity >= 8
 * @pre initial_capacity <= 1'073'741'824
 * @pre load_factor >= 0.5
 * @pre hash != nullptr
 * @pre destruct != nullptr
 * @pre equals != nullptr
 * @pre to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
 */
typedef struct {
    int initial_capacity;
    float load_factor;
    uint64_t (*hash)(const void* element);
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
} HashSetOptions;

/**
 * @brief Utility macro providing default HashSetOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_HASH_SET_OPTIONS(...) &(HashSetOptions) {   \
    .initial_capacity = 16,                                 \
    .load_factor = 0.75f,                                   \
    .hash = pointer_hash,                                   \
    .destruct = noop_destruct,                              \
    .equals = pointer_equals,                               \
    .to_string = pointer_to_string,                         \
    .memory_alloc = malloc,                                 \
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

/**
 * @brief Creates a new empty hash set using the specified options.
 *
 * @param options pointer to a HashSetOptions structure
 *
 * @return pointer to a newly created hash set
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashSet* hash_set_new(const HashSetOptions* options);

/**
 * @brief Creates a new hash set containing all elements of the given collection.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created hash set
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
HashSet* hash_set_from(Collection collection, const HashSetOptions* options);

/**
 * @brief Destroys a hash set and optionally its elements.
 *
 * @param hash_set_pointer pointer to a hash set pointer
 *
 * @exception NULL_POINTER_ERROR if hash_set_pointer or *hash_set_pointer are null
 *
 * @post *hash_set_pointer == nullptr
 */
void hash_set_destroy(HashSet** hash_set_pointer);

/**
 * @brief Sets the element destruct function.
 *
 * @param hash_set pointer to a hash set
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if hash_set or destruct is null
 */
void hash_set_set_destructor(HashSet* hash_set, void (*destruct)(void*));

/**
 * @brief Inserts an element in the hash set.
 *
 * @param hash_set pointer to a hash set
 * @param element element to insert
 *
 * @return true if the element was inserted, false if already present
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new node fails
 */
bool hash_set_add(HashSet* hash_set, const void* element);

/**
 * @brief Inserts all elements of a collection in the hash set.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param hash_set pointer to a hash set
 * @param collection source collection
 *
 * @return true if the hash set was changed, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing, creating a new node or creation of the collection iterator fails
 */
bool hash_set_add_all(HashSet* hash_set, Collection collection);

/**
 * @brief Removes the specified element (if present) of the hash set.
 *
 * @param hash_set pointer to a hash set
 * @param element element to remove
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 *
 * @note this function calls the element destruct before returning.
 */
bool hash_set_remove(HashSet* hash_set, const void* element);

/**
 * @brief Removes all elements of a collection present in the hash set.
 *
 * @param hash_set pointer to a hash set
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 *
 * @note this function calls the element destruct before returning.
 */
int hash_set_remove_all(HashSet* hash_set, Collection collection);

/**
 * @brief Removes all elements of the hash set matching a predicate.
 *
 * @param hash_set pointer to a hash set
 * @param condition condition to remove
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set or condition is null
 *
 * @note this function calls the element destruct before returning.
 */
int hash_set_remove_if(HashSet* hash_set, Predicate condition);

/**
 * @brief Retains all elements of a collection present in the hash set while removing all other elements.
 *
 * @param hash_set pointer to a hash set
 * @param collection source collection
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 *
 * @note this function calls the element destruct before returning.
 */
int hash_set_retain_all(HashSet* hash_set, Collection collection);

/**
 * @brief Returns the current number of elements in the hash set.
 *
 * @param hash_set pointer to a hash set
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
int hash_set_size(const HashSet* hash_set);

/**
 * @brief Returns the current capacity of the hash set.
 *
 * @param hash_set pointer to a hash set
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
int hash_set_capacity(const HashSet* hash_set);

/**
 * @brief Checks whether the hash set is empty.
 *
 * @param hash_set pointer to a hash set
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
bool hash_set_is_empty(const HashSet* hash_set);

/**
 * @brief Creates an iterator for the hash set.
 *
 * The iteration order is not stable and may change after resizing.
 *
 * @param hash_set pointer to a hash set
 *
 * @return pointer to a newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: has_previous(), previous(), add() and set()
 */
Iterator* hash_set_iterator(const HashSet* hash_set);

/**
 * @brief Checks whether two hash sets are equal.
 *
 * Two hash sets are equal if:
 * - they reference the same memory address, or
 * - they have the same size, and each element in the first hash set is present in the second
 *   hash set according to the equals function of the first hash set.
 *
 * @param hash_set first hash set
 * @param other_hash_set second hash set
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either hash_set is null
 */
bool hash_set_equals(const HashSet* hash_set, const HashSet* other_hash_set);

/**
 * @brief Applies an action to each entry of the hash set.
 *
 * @param hash_set pointer to a hash set
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if hash_set or action is null
 */
void hash_set_for_each(HashSet* hash_set, Consumer action);

/**
 * @brief Removes all elements of the hash set.
 *
 * @param hash_set pointer to a hash set
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 *
 * @note this function calls the element destruct before returning.
 */
void hash_set_clear(HashSet* hash_set);

/**
 * @brief Checks whether an element is present in the hash set.
 *
 * @param hash_set pointer to a hash set
 * @param element element to check
 *
 * @return true if present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
bool hash_set_contains(const HashSet* hash_set, const void* element);

/**
 * @brief Checks whether all elements of a collection are present in the hash set.
 *
 * Duplicated elements in the source collection are ignored.
 *
 * @param hash_set pointer to a hash set
 * @param collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
bool hash_set_contains_all(const HashSet* hash_set, Collection collection);

/**
 * @brief Creates a shallow copy of the hash set.
 *
 * The new hash set shares element pointers but has independent storage.
 *
 * @param hash_set pointer to a hash set
 *
 * @return pointer to the newly created hash set
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashSet* hash_set_clone(const HashSet* hash_set);

/**
 * @brief Returns a Collection view of the hash set.
 *
 * The returned collection does not own the elements.
 *
 * @param hash_set pointer to a hash set
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
Collection hash_set_to_collection(const HashSet* hash_set);

/**
 * @brief Returns a newly allocated array containing all elements of the hash set.
 *
 * @param hash_set pointer to a hash set
 *
 * @return an array of elements
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually
 */
void** hash_set_to_array(const HashSet* hash_set);

/**
 * @brief Converts the hash set to a string representation.
 *
 * @param hash_set pointer to a hash set
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* hash_set_to_string(const HashSet* hash_set);

#endif