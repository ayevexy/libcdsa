#ifndef HASH_SET_H
#define HASH_SET_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

typedef struct HashSet HashSet;

typedef struct {
    int initial_capacity;
    float load_factor;
    uint64_t (*hash)(const void* element);
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
} HashSetOptions;

#define DEFAULT_HASH_SET_OPTIONS(...) &(HashSetOptions) {   \
    .initial_capacity = 16,                                 \
    .load_factor = 0.75f,                                   \
    .hash = pointer_hash,                                   \
    .destruct = noop_destruct,                              \
    .equals = pointer_equals,                               \
    .to_string = pointer_to_string,                         \
    .memory_alloc = malloc,                                 \
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

HashSet* hash_set_new(const HashSetOptions* options);

HashSet* hash_set_from(Collection collection, const HashSetOptions* options);

void hash_set_destroy(HashSet** hash_set_pointer);

void (*hash_set_get_destructor(const HashSet* hash_set))(void*);

void hash_set_set_destructor(HashSet* hash_set, void (*destructor)(void*));

bool hash_set_add(HashSet* hash_set, const void* element);

bool hash_set_add_all(HashSet* hash_set, Collection collection);

bool hash_set_remove(HashSet* hash_set, const void* element);

int hash_set_remove_all(HashSet* hash_set, Collection collection);

int hash_set_remove_if(HashSet* hash_set, Predicate condition);

int hash_set_retain_all(HashSet* hash_set, Collection collection);

int hash_set_size(const HashSet* hash_set);

bool hash_set_is_empty(const HashSet* hash_set);

Iterator* hash_set_iterator(const HashSet* hash_set);

bool hash_set_equals(const HashSet* hash_set, const HashSet* other_hash_set);

void hash_set_for_each(HashSet* hash_set, Consumer action);

void hash_set_clear(HashSet* hash_set);

bool hash_set_contains(const HashSet* hash_set, const void* element);

bool hash_set_contains_all(const HashSet* hash_set, Collection collection);

HashSet* hash_set_clone(const HashSet* hash_set);

Collection hash_set_to_collection(const HashSet* hash_set);

void** hash_set_to_array(const HashSet* hash_set);

char* hash_set_to_string(const HashSet* hash_set);

#endif