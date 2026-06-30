#include "collection.h"

#include "core/errors.h"

int collection_size(Collection collection) {
    return collection.size(collection.data_structure);
}

bool collection_is_empty(Collection collection) {
    return collection_size(collection) == 0;
}

Iterator* collection_iterator(Collection collection) {
    return collection.iterator(collection.data_structure);
}

bool collection_equals(Collection collection, Collection other_collection) {
    return collection.data_structure == other_collection.data_structure;
}

void collection_for_each(Collection collection, Consumer action) {
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    while (iterator_has_next(iterator)) {
        action(iterator_next(iterator));
    }
    iterator_destroy(&iterator);
}

bool collection_contains(Collection collection, const void* element) {
    return collection.contains(collection.data_structure, element);
}

bool collection_contains_all(Collection collection, Collection other_collection) {
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

Array(void*) collection_to_array(Collection collection) {
    Array(void*) elements = array_new(collection_size(collection), void*);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        array_destroy(&elements);
        set_error(error, "%s of 'collection'", plain_error_message());
        return nullptr;
    }
    for (int i = 0; iterator_has_next(iterator); i++) {
        elements[i] = iterator_next(iterator);
    }
    return elements;
}