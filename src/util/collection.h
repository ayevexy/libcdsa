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
} Collection;

/**
 * @brief Creates a Collection view from a data structure.
 *
 * This macro assumes the data structure provides functions named:
 * - <type>_size(const <type>*)
 * - <type>_iterator(const <type>*)
 *
 * @param _data_structure pointer to the data structure
 *
 * @return a Collection view over _data_structure
 *
 * @pre _data_structure != nullptr
 */
#define collection_from(_data_structure)                                        \
    (Collection) {                                                              \
        .data_structure = _data_structure,                                      \
        .size = (int (*)(const void*)) _data_structure##_size,                  \
        .iterator = (Iterator* (*)(const void*)) _data_structure##_iterator     \
    }

/**
 * @brief Retrieves the size of the given Collection.
 *
 * @param collection a Collection
 *
 * @return number of elements in the collection
 */
#define collection_size(collection) collection.size(collection.data_structure)

/**
 * @brief Creates an iterator for the given Collection.
 *
 * @param collection a Collection
 *
 * @return pointer to a newly created Iterator
 */
#define collection_iterator(collection) collection.iterator(collection.data_structure)

#endif