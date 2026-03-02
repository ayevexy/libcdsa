#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

typedef struct Entry Entry;

typedef struct HashMap HashMap;

typedef struct {
    int initial_capacity;
    float load_factor;
    uint64_t (*hash)(const void* key);
    struct {
        void (*key_destruct)(void*);
        bool (*key_equals)(const void*, const void*);
        int (*key_to_string)(const void*, char*, size_t);
    };
    struct {
        void (*value_destruct)(void*);
        bool (*value_equals)(const void*, const void*);
        int (*value_to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} HashMapOptions;

#define DEFAULT_HASH_MAP_OPTIONS(...) &(HashMapOptions) {   \
    .initial_capacity = 11,                                 \
    .load_factor = 0.75f,                                   \
    .hash = pointer_hash,                                   \
    .key_destruct = nullptr,                                \
    .key_equals = pointer_equals,                           \
    .key_to_string = pointer_to_string,                     \
    .value_destruct = nullptr,                              \
    .value_equals = pointer_equals,                         \
    .value_to_string = pointer_to_string,                   \
    .memory_alloc = malloc,                                 \
    .memory_realloc = realloc,                              \
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

const void* entry_key(const Entry* entry);

void* entry_value(const Entry* entry);


HashMap* hash_map_new(const HashMapOptions* options);

HashMap* hash_map_from(Collection entry_collection, const HashMapOptions* options);

void hash_map_destroy(HashMap** hash_map_pointer);

void hash_map_obliterate(HashMap** hash_map_pointer);


void* hash_map_compute(HashMap* hash_map, const void* key, BiOperator remapper);

void* hash_map_compute_if_absent(HashMap* hash_map, const void* key, Operator mapper);

void* hash_map_compute_if_present(HashMap* hash_map, const void* key, BiOperator remapper);

void* hash_map_merge(HashMap* hash_map, const void* key, const void* value, BiOperator remapper);


void* hash_map_put(HashMap* hash_map, const void* key, const void* value);

void* hash_map_put_if_absent(HashMap* hash_map, const void* key, const void* value);

void hash_map_put_all(HashMap* hash_map, Collection entry_collection);


void* hash_map_replace(HashMap* hash_map, const void* key, const void* value);

void hash_map_update(HashMap* hash_map, const void* key, const void* value);

void* hash_map_replace_if_equals(HashMap* hash_map, const void* key, const void* old_value, const void* value);

void hash_map_update_if_equals(HashMap* hash_map, const void* key, const void* old_value, const void* value);


void* hash_map_get(const HashMap* hash_map, const void* key);

void* hash_map_get_or_default(const HashMap* hash_map, const void* key, const void* default_value);


void* hash_map_remove(HashMap* hash_map, const void* key);

bool hash_map_remove_if_equals(HashMap* hash_map, const void* key, const void* value);

void hash_map_delete(HashMap* hash_map, const void* key);

bool hash_map_delete_if_equals(HashMap* hash_map, const void* key, const void* value);

int hash_map_replace_all(HashMap* hash_map, BiOperator remapper);

int hash_map_update_all(HashMap* hash_map, BiOperator remapper);


int hash_map_size(const HashMap* hash_map);

bool hash_map_is_empty(const HashMap* hash_map);

Iterator* hash_map_iterator(const HashMap* hash_map);

bool hash_map_equals(const HashMap* hash_map, const HashMap* other_hash_map);


void hash_map_for_each(HashMap* hash_map, BiConsumer action);

void hash_map_clear(HashMap* hash_map);

void hash_map_purge(HashMap* hash_map);


bool hash_map_contains(const HashMap* hash_map, const void* key, const void* value);

bool hash_map_contains_key(const HashMap* hash_map, const void* key);

bool hash_map_contains_value(const HashMap* hash_map, const void* value);


Collection hash_map_keys(const HashMap* hash_map);

Collection hash_map_values(const HashMap* hash_map);

Collection hash_map_entries(const HashMap* hash_map);


HashMap* hash_map_clone(const HashMap* hash_map);

Entry* hash_map_to_entry_array(const HashMap* hash_map);

char* hash_map_to_string(const HashMap* hash_map);

#endif