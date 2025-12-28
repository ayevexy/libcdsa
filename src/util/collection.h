#ifndef COLLECTION_H
#define COLLECTION_H

#include "iterator.h"

typedef struct {
    const void* const data_structure;
    int (*const size)(const void*);
    Iterator* (*const iterator)(const void*);
} Collection;

#define collection_from(ds)                                         \
    (Collection) {                                                  \
        .data_structure = ds,                                       \
        .size = (int (*)(const void*)) ds##_size,                   \
        .iterator = (Iterator* (*)(const void*)) ds##_iterator      \
    }

#define collection_size(collection) collection.size(collection.data_structure)

#define collection_iterator(collection) collection.iterator(collection.data_structure)

#endif