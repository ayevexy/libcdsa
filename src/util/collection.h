#ifndef LIBCDSA_COLLECTION_H
#define LIBCDSA_COLLECTION_H

#include "core/array.h"
#include "functions.h"
#include "iterator.h"

/**
 * @brief Enumeration of existing collection types.
 */
typedef enum {
    ARRAY_LIST,
    LINKED_LIST,
    HASH_MAP,
    TREE_MAP,
    HASH_SET,
    TREE_SET,
    DEQUE,
    STACK,
    QUEUE,
    PRIORITY_QUEUE
} CollectionType;

/**
 * @brief A generic collection view abstraction that provides a common interface over data structures.
 */
typedef struct {
    const CollectionType type;
    const void* const data_structure;
    int (*const size)(const void*);
    Iterator* (*const iterator)(const void*);
    bool (*const contains)(const void*, const void*);
} Collection;

/**
 * @brief Retrieves the current size of the collection.
 *
 * @param collection the collection
 *
 * @return the current size
 */
int collection_size(Collection collection);

/**
 * @brief Checks whether the collection is empty.
 *
 * @param collection the collection
 *
 * @return true if empty, false otherwise
 */
bool collection_is_empty(Collection collection);

/**
 * @brief Creates an iterator for the collection.
 *
 * @param collection the collection
 *
 * @return pointer to a newly created Iterator
 */
Iterator* collection_iterator(Collection collection);

/**
 * @brief Checks whether two collections are equals.
 *
 * @param collection first collection
 * @param other_collection second collection
 *
 * @return true if equals, false otherwise
 */
bool collection_equals(Collection collection, Collection other_collection);

/**
 * @brief Applies an action to each element of the collection.
 *
 * @param collection the collection
 * @param action function to apply
 *
 * @exception MEMORY_ALLOCATION_ERROR if creation of the collection iterator fails
 */
void collection_for_each(Collection collection, Consumer action);

/**
 * @brief Checks whether an element is present in the collection.
 *
 * @param collection the collection
 * @param element the element to check
 *
 * @return true if the element is present, false otherwise
 */
bool collection_contains(Collection collection, const void* element);

/**
 * @brief Checks whether all elements of another collection are present in the collection.
 *
 * @param collection the collection
 * @param other_collection source collection
 *
 * @return true if all elements are present, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR if creation of other_collection iterator fails
 */
bool collection_contains_all(Collection collection, Collection other_collection);

/**
 * @brief Returns a newly allocated array containing all elements of the collection.
 *
 * @param collection the collection
 *
 * @return an array of elements
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created array must be freed manually (by `array_destroy()`)
 */
Array(void*) collection_to_array(Collection collection);

#endif