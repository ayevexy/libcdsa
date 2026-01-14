#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"
#include "util/optional.h"

#include <stdlib.h>
#include <stddef.h>

#define DEFAULT_LINKED_LIST_OPTIONS &(LinkedListOptions) {      \
    .equals = pointer_equals,                                   \
    .to_string = pointer_to_string,                             \
    .memory_alloc = malloc,                                     \
    .memory_realloc = realloc,                                  \
    .memory_free = free                                         \
}

typedef struct LinkedListOptions {
    bool (*equals)(const void*, const void*);
    int (*to_string)(const void*, char*, size_t);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} LinkedListOptions;

typedef struct LinkedList LinkedList;

LinkedList* linked_list_new(const LinkedListOptions*);

LinkedList* linked_list_from(Collection, const LinkedListOptions*);

void linked_list_delete(LinkedList**);

void linked_list_destroy(LinkedList**, void (*delete)(void*));

bool linked_list_add(LinkedList*, int index, const void* element);

bool linked_list_add_first(LinkedList*, const void* element);

bool linked_list_add_last(LinkedList*, const void* element);

bool linked_list_add_all(LinkedList*, int index, Collection);

bool linked_list_add_all_first(LinkedList*, Collection);

bool linked_list_add_all_last(LinkedList*, Collection);

void* linked_list_get(const LinkedList*, int index);

void* linked_list_get_first(const LinkedList*);

void* linked_list_get_last(const LinkedList*);

void* linked_list_set(LinkedList*, int index, const void* element);

bool linked_list_swap(LinkedList*, int index_a, int index_b);

void* linked_list_remove(LinkedList*, int index);

void* linked_list_remove_first(LinkedList*);

void* linked_list_remove_last(LinkedList*);

bool linked_list_remove_element(LinkedList*, const void* element);

int linked_list_remove_all(LinkedList*, Collection);

int linked_list_remove_range(LinkedList*, int start_index, int end_index);

int linked_list_remove_if(LinkedList*, Predicate);

void linked_list_replace_all(LinkedList*, UnaryOperator);

int linked_list_retain_all(LinkedList*, Collection);

int linked_list_size(const LinkedList*);

bool linked_list_is_empty(const LinkedList*);

Iterator* linked_list_iterator(const LinkedList*);

bool linked_list_equals(const LinkedList*, const LinkedList*);

void linked_list_for_each(LinkedList*, Consumer);

void linked_list_sort(LinkedList*, Comparator, SortingAlgorithm);

void linked_list_shuffle(LinkedList*, int (*random)(void), ShufflingAlgorithm);

void linked_list_reverse(LinkedList*);

void linked_list_rotate(LinkedList*, int distance);

void linked_list_clear(LinkedList*);

void linked_list_clear_data(LinkedList*, void (*delete)(void*));

Optional linked_list_find(const LinkedList*, Predicate);

Optional linked_list_find_last(const LinkedList*, Predicate);

int linked_list_index_where(const LinkedList*, Predicate);

int linked_list_last_index_where(const LinkedList*, Predicate);

bool linked_list_contains(const LinkedList*, const void* element);

bool linked_list_contains_all(const LinkedList*, Collection);

int linked_list_occurrences_of(const LinkedList*, const void* element);

int linked_list_index_of(const LinkedList*, const void* element);

int linked_list_last_index_of(const LinkedList*, const void* element);

LinkedList* linked_list_clone(const LinkedList*);

LinkedList* linked_list_sub_list(const LinkedList*, int start_index, int end_index);

Collection linked_list_to_collection(const LinkedList*);

void** linked_list_to_array(const LinkedList*);

char* linked_list_to_string(const LinkedList*);

#endif