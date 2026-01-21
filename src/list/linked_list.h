#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/algorithms.h"
#include "util/collection.h"
#include "util/optional.h"

#include <stdlib.h>
#include <stddef.h>

typedef struct LinkedList LinkedList;

typedef struct LinkedListOptions {
    bool (*equals)(const void*, const void*);
    int (*to_string)(const void*, char*, size_t);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} LinkedListOptions;

#define DEFAULT_LINKED_LIST_OPTIONS(...) &(LinkedListOptions) {     \
    .equals = pointer_equals,                                       \
    .to_string = pointer_to_string,                                 \
    .memory_alloc = malloc,                                         \
    .memory_realloc = realloc,                                      \
    .memory_free = free,                                            \
    __VA_ARGS__                                                     \
}

LinkedList* linked_list_new(const LinkedListOptions* options);

LinkedList* linked_list_from(Collection collection, const LinkedListOptions* options);

void linked_list_delete(LinkedList** linked_list_pointer);

void linked_list_destroy(LinkedList** linked_list, void (*delete)(void*));

void linked_list_add(LinkedList* linked_list, int index, const void* element);

void linked_list_add_first(LinkedList* linked_list, const void* element);

void linked_list_add_last(LinkedList* linked_list, const void* element);

void linked_list_add_all(LinkedList* linked_list, int index, Collection collection);

void linked_list_add_all_first(LinkedList* linked_list, Collection collection);

void linked_list_add_all_last(LinkedList* linked_list, Collection collection);

void* linked_list_get(const LinkedList* linked_list, int index);

void* linked_list_get_first(const LinkedList* linked_list);

void* linked_list_get_last(const LinkedList* linked_list);

void* linked_list_set(LinkedList* linked_list, int index, const void* element);

void linked_list_swap(LinkedList* linked_list, int index_a, int index_b);

void* linked_list_remove(LinkedList* linked_list, int index);

void* linked_list_remove_first(LinkedList* linked_list);

void* linked_list_remove_last(LinkedList* linked_list);

bool linked_list_remove_element(LinkedList* linked_list, const void* element);

int linked_list_remove_all(LinkedList* linked_list, Collection collection);

int linked_list_remove_range(LinkedList* linked_list, int start_index, int end_index);

int linked_list_remove_if(LinkedList* linked_list, Predicate condition);

void linked_list_replace_all(LinkedList* linked_list, UnaryOperator operator);

int linked_list_retain_all(LinkedList* linked_list, Collection collection);

int linked_list_size(const LinkedList* linked_list);

bool linked_list_is_empty(const LinkedList* linked_list);

Iterator* linked_list_iterator(const LinkedList* linked_list);

bool linked_list_equals(const LinkedList* linked_list, const LinkedList* other_linked_list);

void linked_list_for_each(LinkedList* linked_list, Consumer action);

void linked_list_sort(LinkedList* linked_list, Comparator comparator, SortingAlgorithm algorithm);

void linked_list_shuffle(LinkedList*, int (*random)(void), ShufflingAlgorithm);

void linked_list_reverse(LinkedList* linked_list);

void linked_list_rotate(LinkedList* linked_list, int distance);

void linked_list_clear(LinkedList* linked_list);

void linked_list_clear_data(LinkedList* linked_list, void (*delete)(void*));

Optional linked_list_find(const LinkedList* linked_list, Predicate condition);

Optional linked_list_find_last(const LinkedList* linked_list, Predicate condition);

int linked_list_index_where(const LinkedList* linked_list, Predicate condition);

int linked_list_last_index_where(const LinkedList* linked_list, Predicate condition);

bool linked_list_contains(const LinkedList* linked_list, const void* element);

bool linked_list_contains_all(const LinkedList* linked_list, Collection collection);

int linked_list_occurrences_of(const LinkedList* linked_list, const void* element);

int linked_list_index_of(const LinkedList* linked_list, const void* element);

int linked_list_last_index_of(const LinkedList* linked_list, const void* element);

LinkedList* linked_list_clone(const LinkedList* linked_list);

LinkedList* linked_list_sub_list(const LinkedList* linked_list, int start_index, int end_index);

Collection linked_list_to_collection(const LinkedList* linked_list);

void** linked_list_to_array(const LinkedList* linked_list);

char* linked_list_to_string(const LinkedList* linked_list);

#endif