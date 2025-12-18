#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"

#define DEFAULT_OPTIONS (Options) { \
    .initial_capacity = 10,         \
    .grow_factor = 2.0,             \
    .equals = default_equals,       \
    .to_string = default_to_string  \
}

typedef struct Options {
    int initial_capacity;
    double grow_factor;
    bool (*equals)(void*, void*);
    char* (*to_string)(void*);
} Options;

typedef struct ArrayList ArrayList;

ArrayList* array_list_new(Options);

ArrayList* array_list_from(Collection, Options);

void array_list_delete(ArrayList*);

void array_list_add(ArrayList*, void* element);

void array_list_add_at(ArrayList*, int index, void* element);

void array_list_add_all(ArrayList*, Collection);

void array_list_add_all_at(ArrayList*, int index, Collection);

void* array_list_get(ArrayList*, int index);

void array_list_set(ArrayList*, int index, void* element);

void array_list_remove(ArrayList*, int index);

void array_list_remove_element(ArrayList*, void* element);

void array_list_remove_all(ArrayList*, Collection);

void array_list_remove_range(ArrayList*, int start_index, int end_index);

void array_list_remove_if(ArrayList*, Predicate);

int array_list_size(ArrayList*);

int array_list_capacity(ArrayList*);

bool array_list_is_empty(ArrayList*);

Iterator* array_list_iterator(ArrayList*);

void array_list_for_each(ArrayList*, Consumer);

void array_list_sort(ArrayList*, Comparator, SortingAlgorithm);

void array_list_clear(ArrayList*);

bool array_list_contains(ArrayList*, void* element);

int array_list_index_of(ArrayList*, void* element);

int array_list_last_index_of(ArrayList*, void* element);

ArrayList* array_list_clone(ArrayList*);

ArrayList* array_list_sub_list(ArrayList*, int start_index, int end_index);

Collection array_list_to_collection(ArrayList*);

char* array_list_to_string(ArrayList*);

#endif