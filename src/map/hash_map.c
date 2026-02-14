#include "hash_map.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <limits.h>

constexpr int MIN_CAPACITY = 10;
constexpr int MAX_CAPACITY = (INT_MAX - 1);
constexpr float MIN_LOAD_FACTOR = 0.5;

typedef struct Entry {
    void* key;
    void* value;
    int hash;
    struct Entry* next;
} Entry;

struct HashMap {
    Entry** buckets;
    int size;
    int threshold;
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
    int modification_count;
};

HashMap* hash_map_new(const HashMapOptions* options) {
    if (set_error_on_null(options)) return nullptr;

    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->load_factor < MIN_LOAD_FACTOR || !options->key_equals || !options->key_to_string
        || !options->value_equals || !options->value_to_string || !options->memory_alloc
        || !options->memory_realloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }

    HashMap* hash_map = options->memory_alloc(sizeof(HashMap));
    if (!hash_map) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_map'");
        return nullptr;
    }

    hash_map->buckets = options->memory_alloc(options->initial_capacity * sizeof(Entry*));
    if (!hash_map->buckets) {
        options->memory_free(hash_map);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_map'");
        return nullptr;
    }

    hash_map->size = 0;
    hash_map->threshold = options->initial_capacity * options->load_factor;
    hash_map->load_factor = options->load_factor;
    hash_map->hash = options->hash;

    hash_map->key_destruct = options->key_destruct;
    hash_map->key_equals = options->key_equals;
    hash_map->key_to_string = options->key_to_string;

    hash_map->value_destruct = options->value_destruct;
    hash_map->value_equals = options->value_equals;
    hash_map->value_to_string = options->value_to_string;

    hash_map->memory_alloc = options->memory_alloc;
    hash_map->memory_realloc = options->memory_realloc;
    hash_map->memory_free = options->memory_free;
    hash_map->modification_count = 0;

    return hash_map;
}

static void hash_map_destroy_internal(HashMap** hash_map_pointer, bool destruct_entries) {
    if (set_error_on_null(hash_map_pointer, *hash_map_pointer)) return;
    HashMap* hash_map = *hash_map_pointer;
    if (destruct_entries && (!hash_map->key_destruct || !hash_map->value_destruct)) {
        set_error(UNSUPPORTED_OPERATION_ERROR, "No 'destruct' functions assigned");
        return;
    }
    for (int i = 0; destruct_entries && i < hash_map->size; i++) {
        if (hash_map->key_destruct) hash_map->key_destruct(hash_map->buckets[i]->key);
        if (hash_map->value_destruct) hash_map->value_destruct(hash_map->buckets[i]->value);
    }
    hash_map->memory_free(hash_map->buckets);
    hash_map->memory_free(hash_map);
    *hash_map_pointer = nullptr;
}

void hash_map_destroy(HashMap** hash_map_pointer) {
    hash_map_destroy_internal(hash_map_pointer, false);
}

void hash_map_obliterate(HashMap** hash_map_pointer) {
    hash_map_destroy_internal(hash_map_pointer, true);
}

void* hash_map_put(HashMap* hash_map, const void* key, const void* value) {

}