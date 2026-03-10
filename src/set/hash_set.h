#ifndef HASH_SET_H
#define HASH_SET_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A hash set is a generic dynamic unordered data structure that stores elements using a hashing function to ensure uniqueness.
 * It automatically grows to accommodate new elements and allows insertion of values of any type.
 *
 * Internally, the implementation uses a struct containing an array of Node pointers (buckets). Collisions are
 * resolved by chaining nodes. All operations on a hash set receive a pointer to the hash set itself as their first argument.
 * The HashSet type is opaque and can only be modified through the API.
 *
 * It must be configured using a HashSetOptions defining:
 * - its initial capacity (must be a power of two, otherwise it will be automatically rounded to the next power of two)
 * - its load factor
 * - its hash function
 * - the destruct function utilized to free elements memory
 * - the equals function utilized to compare elements
 * - the to string function utilized to convert its elements to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to free memory
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
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct HashSet HashSet;

/**
 * HashSet configuration structure. Used to define the default behavior and attributes of a HashSet.
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
 * @brief A utility macro that provides a reasonable default HashSetOptions.
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
 * @brief Creates a new empty HashSet using the specified options.
 *
 * @param options pointer to a HashSetOptions defining the hash set configuration
 *
 * @return pointer to the newly created HashSet on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the HashSet fails
 */
HashSet* hash_set_new(const HashSetOptions* options);

/**
 * @brief Creates a new HashSet containing all elements of the given collection using the specified options.
 *
 * @param collection a Collection containing the elements to be added
 * @param options pointer to a HashSetOptions containing configuration settings
 *
 * @return pointer to a newly created HashSet on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates the required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the new HashSet or collection's iterator fails
 */
HashSet* hash_set_from(Collection collection, const HashSetOptions* options);

/**
 * @brief Destroys an existing HashSet and (optionally) its elements using the provided destruct function.
 *
 * @param hash_set_pointer pointer to a HashSet pointer
 *
 * @exception NULL_POINTER_ERROR if hash_set_pointer or *hash_set_pointer are null
 *
 * @post *hash_set_pointer == nullptr
 */
void hash_set_destroy(HashSet** hash_set_pointer);

/**
 * @brief Get the current destruct function.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return pointer to the destruct function
 */
void (*hash_set_get_destructor(const HashSet* hash_set))(void*);

/**
 * @brief Set the current destruct function.
 *
 * @param hash_set pointer to a HashSet
 * @param destructor the new destruct function
 */
void hash_set_set_destructor(HashSet* hash_set, void (*destructor)(void*));

/**
 * @brief Inserts the specified element in the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 * @param element pointer to the element to be inserted
 *
 * @return true if the element was inserted, false if already present
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand hash_set capacity or allocate a new node
 */
bool hash_set_add(HashSet* hash_set, const void* element);

/**
 * @brief Inserts all elements of the specified Collection in the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 * @param collection a Collection containing the elements to be added
 *
 * @return true if the HashSet was changed, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand hash_set capacity or allocate a new node, or to allocate the collection's iterator
 */
bool hash_set_add_all(HashSet* hash_set, Collection collection);

/**
 * @brief Removes the specified element (if present) of the provided HashSet, (optionally) destructing it.
 *
 * @param hash_set pointer to a HashSet
 * @param element pointer to the element to be removed
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
bool hash_set_remove(HashSet* hash_set, const void* element);

/**
 * @brief Removes all elements of the given collection present in the provided HashSet, (optionally) destructing them.
 *
 * @param hash_set pointer to a HashSet
 * @param collection a Collection containing the elements to be removed
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int hash_set_remove_all(HashSet* hash_set, Collection collection);

/**
 * @brief Removes all elements matching the given Predicate in the provided HashSet, (optionally) destructing them.
 *
 * @param hash_set pointer to a HashSet
 * @param condition the condition to remove elements
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set or condition are null
 */
int hash_set_remove_if(HashSet* hash_set, Predicate condition);

/**
 * @brief Retains all elements of the given collection present in the provided
 * HashSet while removing all other elements, (optionally) destructing them.
 *
 * @param hash_set pointer to a HashSet
 * @param collection a Collection containing the elements to be held
 *
 * @return number of elements removed
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
int hash_set_retain_all(HashSet* hash_set, Collection collection);

/**
 * @brief Retrieves the current size of the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return the current size of the provided HashSet
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
int hash_set_size(const HashSet* hash_set);

/**
 * @brief Retrieves the current capacity of the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return the current capacity of the provided HashSet
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
int hash_set_capacity(const HashSet* hash_set);

/**
 * @brief Checks whether the provided HashSet is empty.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
bool hash_set_is_empty(const HashSet* hash_set);

/**
 * @brief Instantiates an Iterator for the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 */
Iterator* hash_set_iterator(const HashSet* hash_set);

/**
 * @brief Checks whether two HashSet objects are equal.
 *
 * Two hash sets are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and every element of the first hash set is present
 * in the second hash set according to the equals function of the first hash set.
 *
 * @param hash_set pointer to a HashSet
 * @param other_hash_set pointer to a HashSet
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set or other_hash_set are null
 */
bool hash_set_equals(const HashSet* hash_set, const HashSet* other_hash_set);

/**
 * @brief Performs an action for each element of the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if hash_set or action are null
 */
void hash_set_for_each(HashSet* hash_set, Consumer action);

/**
 * @brief Removes all elements of the provided HashSet, (optionally) destructing them.
 *
 * @param hash_set pointer to a HashSet
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
void hash_set_clear(HashSet* hash_set);

/**
 * @brief Checks whether the provided HashSet contains the specified element.
 *
 * @param hash_set pointer to a HashSet
 * @param element pointer to the element to be checked
 *
 * @return true if the element is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
bool hash_set_contains(const HashSet* hash_set, const void* element);

/**
 * @brief Checks whether the provided HashSet contains all elements of the given collection.
 *
 * @param hash_set pointer to a HashSet
 * @param collection a Collection containing elements to be checked
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the collection's iterator fails
 */
bool hash_set_contains_all(const HashSet* hash_set, Collection collection);

/**
 * @brief Creates a shallow copy of the provided HashSet.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return a newly created HashSet clone, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashSet* hash_set_clone(const HashSet* hash_set);

/**
 * @brief Converts the provided HashSet into a Collection view.
 *
 * The returned Collection does not own the underlying elements.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return a Collection representation of the HashSet
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 */
Collection hash_set_to_collection(const HashSet* hash_set);

/**
 * @brief Converts the provided HashSet into a newly allocated array.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return a newly allocated array containing all elements
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
void** hash_set_to_array(const HashSet* hash_set);

/**
 * @brief Converts the provided HashSet to a string representation.
 *
 * @param hash_set pointer to a HashSet
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if hash_set is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* hash_set_to_string(const HashSet* hash_set);

#endif