#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "util/iterator.h"
#include "util/functions.h"

#define DEFAULT_OPTIONS (Options) { \
    .initial_capacity = 10,         \
    .grow_factor = 2.0,             \
    .equals = equals                \
}

typedef struct Options {
    int initial_capacity;
    double grow_factor;
    bool (*equals)(void*, void*);
} Options;

typedef struct ArrayList ArrayList;

ArrayList* array_list_new(Options options);

void array_list_delete(ArrayList*);

void array_list_add(ArrayList*, void* element);

void array_list_add_at(ArrayList*, int index, void* element);

void* array_list_get(ArrayList*, int index);

void array_list_set(ArrayList*, int index, void* element);

void array_list_remove(ArrayList*, int index);

int array_list_size(ArrayList*);

int array_list_capacity(ArrayList*);

bool array_list_is_empty(ArrayList*);

Iterator* array_list_iterator(ArrayList*);

void array_list_clear(ArrayList*);

bool array_list_contains(ArrayList*, void* element);

int array_list_index_of(ArrayList*, void* element);

#endif