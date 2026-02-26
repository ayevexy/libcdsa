#include "hash_map.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <limits.h>
#include <string.h>

constexpr int MIN_CAPACITY = 10;
constexpr int MAX_CAPACITY = (INT_MAX - 1);
constexpr float MIN_LOAD_FACTOR = 0.5;

typedef struct Entry {
    void* key;
    void* value;
    uint64_t hash;
    struct Entry* next;
} Entry;

struct HashMap {
    Entry** buckets;
    int size;
    int capacity;
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

static Entry* create_entry(HashMap*, const void*, const void*);

static Entry* get_entry(const HashMap*, const void*);

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
    memset(hash_map->buckets, 0, options->initial_capacity * sizeof(Entry*));

    hash_map->size = 0;
    hash_map->capacity = options->initial_capacity;
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
    for (int i = 0; i < hash_map->size; i++) {
        Entry* current = hash_map->buckets[i];
        while (current) {
            if (destruct_entries && hash_map->key_destruct) hash_map->key_destruct(current->key);
            if (destruct_entries && hash_map->value_destruct) hash_map->value_destruct(current->value);

            Entry* temporary = current->next;
            hash_map->memory_free(current);
            current = temporary;
        }
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
    if (set_error_on_null(hash_map)) return nullptr;

    Entry* current = get_entry(hash_map, key);
    if (current) {
        void* temporary = current->value;
        current->value = (void*) value;
        return temporary;
    }

    Entry* entry = create_entry(hash_map, key, value);
    if (!entry) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new entry'");
        return nullptr;
    }

    current = hash_map->buckets[entry->hash % hash_map->capacity];
    if (current) entry->next = current;

    hash_map->buckets[entry->hash % hash_map->capacity] = entry;
    hash_map->size++;
    hash_map->modification_count++;
    return nullptr;
}

void* hash_map_get(const HashMap* hash_map, const void* key) {
    if (set_error_on_null(hash_map)) return nullptr;
    Entry* entry = get_entry(hash_map, key);
    if (!entry) {
        set_error(NO_SUCH_ELEMENT_ERROR, "no value found for given key");
        return nullptr;
    }
    return entry->value;
}

static void* hash_map_remove_internal(HashMap* hash_map, const void* key, bool destruct_entry) {
    if (set_error_on_null(hash_map)) return nullptr;

    Entry* prev_entry = nullptr, * entry = hash_map->buckets[hash_map->hash(key) % hash_map->capacity];
    while (entry && !hash_map->key_equals(entry->key, key)) {
        prev_entry = entry;
        entry = entry->next;
    }
    if (!entry) {
        set_error(NO_SUCH_ELEMENT_ERROR, "no value found for given key");
        return nullptr;
    }
    if (prev_entry) {
        prev_entry->next = entry->next;
    } else {
        hash_map->buckets[hash_map->hash(key) % hash_map->capacity] = nullptr;
    }
    if (destruct_entry) {
        if (hash_map->key_destruct) hash_map->key_destruct(entry->key);
        if (hash_map->value_destruct) hash_map->value_destruct(entry->value);
    }
    void* value = entry->value;
    hash_map->size--;
    hash_map->modification_count++;
    hash_map->memory_free(entry);
    return value;
}

void* hash_map_remove(HashMap* hash_map, const void* key) {
    return hash_map_remove_internal(hash_map, key, false);
}

void hash_map_delete(HashMap* hash_map, const void* key) {
    hash_map_remove_internal(hash_map, key, true);
}

int hash_map_size(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return 0;
    return hash_map->size;
}

bool hash_map_is_empty(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return false;
    return hash_map->size == 0;
}

void hash_map_for_each(HashMap* hash_map, BiConsumer action) {
    if (set_error_on_null(hash_map)) return;
    for (int i = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            action(entry->key, entry->value);
            entry = entry->next;
        }
    }
}

static void hash_map_clear_internal(HashMap* hash_map, bool destruct_entries) {
    if (set_error_on_null(hash_map)) return;
    if (destruct_entries && (!hash_map->key_destruct || !hash_map->value_destruct)) {
        set_error(UNSUPPORTED_OPERATION_ERROR, "No 'destruct' functions assigned");
        return;
    }
    for (int i = 0; i < hash_map->size; i++) {
        Entry* current = hash_map->buckets[i];
        while (current) {
            if (destruct_entries) hash_map->key_destruct(current->key);
            if (destruct_entries) hash_map->value_destruct(current->value);

            Entry* temporary = current->next;
            hash_map->memory_free(current);
            current = temporary;
        }
    }
    memset(hash_map->buckets, 0, hash_map->capacity * sizeof(Entry*));
    hash_map->size = 0;
    hash_map->modification_count++;
}

void hash_map_clear(HashMap* hash_map) {
    hash_map_clear_internal(hash_map, false);
}

void hash_map_purge(HashMap* hash_map) {
    hash_map_clear_internal(hash_map, true);
}

bool hash_map_contains(const HashMap* hash_map, const void* key, const void* value) {
    if (set_error_on_null(hash_map)) return false;
    const Entry* entry = get_entry(hash_map, key);
    return hash_map->value_equals(entry->value, value);
}

bool hash_map_contains_key(const HashMap* hash_map, const void* key) {
    if (set_error_on_null(hash_map)) return false;
    return get_entry(hash_map, key) != nullptr;
}

bool hash_map_contains_value(const HashMap* hash_map, const void* value) {
    if (set_error_on_null(hash_map)) return false;
    for (int i = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            if (hash_map->value_equals(entry->value, value)) {
                return true;
            }
            entry = entry->next;
        }
    }
    return false;
}

static Entry* create_entry(HashMap* hash_map, const void* key, const void* value) {
    Entry* entry = hash_map->memory_alloc(sizeof(Entry));
    if (!entry) {
        return nullptr;
    }
    entry->key = (void*) key;
    entry->value = (void*) value;
    entry->hash = hash_map->hash(key);
    entry->next = nullptr;
    return entry;
}

static Entry* get_entry(const HashMap* hash_map, const void* key) {
    Entry* entry = hash_map->buckets[hash_map->hash(key) % hash_map->capacity];
    while (entry) {
        if (hash_map->key_equals(entry->key, key)) {
            return entry;
        }
        entry = entry->next;
    }
    return nullptr;
}