#ifndef LIBCDSA_COLLECTION_H
#define LIBCDSA_COLLECTION_H

#include "iterator.h"
#include "errors.h"

/**
 * @brief A generic collection view abstraction that provides a common interface over data structures.
 */
typedef struct {
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
static inline int collection_size(Collection collection) {
    return collection.size(collection.data_structure);
}

/**
 * @brief Checks whether the collection is empty.
 *
 * @param collection the collection
 *
 * @return true if empty, false otherwise
 */
static inline bool collection_is_empty(Collection collection) {
    return collection_size(collection) == 0;
}

/**
 * @brief Creates an iterator for the collection.
 *
 * @param collection the collection
 *
 * @return pointer to a newly created Iterator
 */
static inline Iterator* collection_iterator(Collection collection) {
    return collection.iterator(collection.data_structure);
}

/**
 * @brief Checks whether two collections are equals.
 *
 * @param collection first collection
 * @param other_collection second collection
 *
 * @return true if equals, false otherwise
 */
static inline bool collection_equals(Collection collection, Collection other_collection) {
    return collection.data_structure == other_collection.data_structure;
}

/**
 * @brief Checks whether an element is present in the collection.
 *
 * @param collection the collection
 * @param element the element to check
 *
 * @return true if the element is present, false otherwise
 */
static inline bool collection_contains(Collection collection, const void* element) {
    return collection.contains(collection.data_structure, element);
}

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
static inline bool collection_contains_all(Collection collection, Collection other_collection) {
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(other_collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!collection_contains(collection, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

#endif