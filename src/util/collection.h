#ifndef COLLECTION_H
#define COLLECTION_H

#include "iterator.h"

/**
 * @brief Generic collection view abstraction.
 *
 * Collection provides a lightweight, non-owning view over a data structure
 * that supports size retrieval and iteration.
 *
 * It allows generic algorithms to operate on different container types
 * without knowledge of their internal representation.
 *
 * A Collection does not own the underlying data structure or its elements.
 */
typedef struct {
    const void* const data_structure;
    int (*const size)(const void*);
    Iterator* (*const iterator)(const void*);
    bool (*const contains)(const void*, const void*);
} Collection;

/**
 * @brief Retrieves the size of the given Collection.
 *
 * @param collection a Collection
 *
 * @return number of elements in the collection
 */
static inline int collection_size(Collection collection) {
    return collection.size(collection.data_structure);
}

/**
 * @brief Checks whether the given Collection is empty.
 *
 * @param collection a Collection
 *
 * @return true if empty, false otherwise
 */
static inline bool collection_is_empty(Collection collection) {
    return collection_size(collection) == 0;
}

/**
 * @brief Creates an iterator for the given Collection.
 *
 * @param collection a Collection
 *
 * @return pointer to a newly created Iterator
 */
static inline Iterator* collection_iterator(Collection collection) {
    return collection.iterator(collection.data_structure);
}
/**
 * @brief Checks whether an element is present in the given Collection.
 *
 * @param collection a Collection
 * @param element the element to check
 *
 * @return true if the element is present, false otherwise
 */
static inline bool collection_contains(Collection collection, const void* element) {
    return collection.contains(collection.data_structure, element);
}

#endif