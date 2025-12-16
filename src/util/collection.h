#ifndef COLLECTION_H
#define COLLECTION_H

#include "iterator.h"

typedef struct {
    void* data_structure;
    int (*size)(void*);
    Iterator* (*iterator)(void*);
} Collection;

#define collection_from(ds)                                 \
    (Collection) {                                          \
        .data_structure = ds,                               \
        .size = (int (*)(void*)) ds##_size,                 \
        .iterator = (Iterator* (*)(void*)) ds##_iterator    \
    }

#define collection_size(collection) collection.size(collection.data_structure)

#define collection_iterator(collection) collection.iterator(collection.data_structure)

#endif