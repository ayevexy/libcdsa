#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"

#include <stddef.h>

#define DEFAULT_ARRAY_LIST_OPTIONS &(ArrayListOptions) {    \
    .initial_capacity = 10,                                 \
    .grow_factor = 2.0,                                     \
    .equals = pointer_equals,                               \
    .to_string = pointer_to_string,                         \
    .memory_alloc = malloc,                                 \
    .memory_realloc = realloc,                              \
    .memory_free = free                                     \
}

typedef struct ArrayListOptions {
    int initial_capacity;
    double grow_factor;
    bool (*equals)(const void*, const void*);
    char* (*to_string)(const void*);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} ArrayListOptions;

typedef struct ArrayList ArrayList;

ArrayList* array_list_new(const ArrayListOptions*);

ArrayList* array_list_from(Collection, const ArrayListOptions*);

void array_list_delete(ArrayList**);

void array_list_destroy(ArrayList**, void (*delete)(void*));

bool array_list_add(ArrayList*, int index, const void* element);

void array_list_add_first(ArrayList*, const void* element);

void array_list_add_last(ArrayList*, const void* element);

bool array_list_add_all(ArrayList*, int index, Collection);

bool array_list_add_all_first(ArrayList*, Collection);

bool array_list_add_all_last(ArrayList*, Collection);

void* array_list_get(const ArrayList*, int index);

void* array_list_get_first(const ArrayList*);

void* array_list_get_last(const ArrayList*);

void* array_list_set(ArrayList*, int index, const void* element);

bool array_list_swap(ArrayList*, int index_a, int index_b);

void* array_list_remove(ArrayList*, int index);

void* array_list_remove_first(ArrayList*);

void* array_list_remove_last(ArrayList*);

bool array_list_remove_element(ArrayList*, const void* element);

int array_list_remove_all(ArrayList*, Collection);

int array_list_remove_range(ArrayList*, int start_index, int end_index);

int array_list_remove_if(ArrayList*, Predicate);

void array_list_replace_all(ArrayList*, UnaryOperator);

int array_list_retain_all(ArrayList*, Collection);

int array_list_size(const ArrayList*);

void array_list_trim_to_size(ArrayList*);

int array_list_capacity(const ArrayList*);

void array_list_ensure_capacity(ArrayList*, int capacity);

bool array_list_is_empty(const ArrayList*);

Iterator* array_list_iterator(const ArrayList*);

void array_list_for_each(ArrayList*, Consumer);

void array_list_sort(ArrayList*, Comparator, SortingAlgorithm);

void array_list_reverse(ArrayList*);

void array_list_clear(ArrayList*);

void array_list_clear_data(ArrayList*, void (*delete)(void*));

void* array_list_find(const ArrayList*, Predicate);

void* array_list_find_last(const ArrayList*, Predicate);

int array_list_index_where(const ArrayList*, Predicate);

int array_list_last_index_where(const ArrayList*, Predicate);

bool array_list_contains(const ArrayList*, const void* element);

bool array_list_contains_all(const ArrayList*, Collection);

int array_list_occurrences_of(const ArrayList*, const void* element);

int array_list_index_of(const ArrayList*, const void* element);

int array_list_last_index_of(const ArrayList*, const void* element);

int array_list_binary_search(const ArrayList*, const void* element, Comparator);

ArrayList* array_list_clone(const ArrayList*);

ArrayList* array_list_sub_list(const ArrayList*, int start_index, int end_index);

Collection array_list_to_collection(const ArrayList*);

void** array_list_to_array(const ArrayList*);

char* array_list_to_string(const ArrayList*);

#endif