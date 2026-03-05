#include "hash_map.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <limits.h>
#include <string.h>

constexpr int MIN_CAPACITY = 10;
constexpr int MAX_CAPACITY = (INT_MAX - 1);
constexpr float MIN_LOAD_FACTOR = 0.5;

struct Entry {
    void* key;
    void* value;
    uint64_t hash;
    Entry* next;
};

const void* entry_key(const Entry* entry) {
    return entry->key;
}

void* entry_value(const Entry* entry) {
    return entry->value;
}

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

static size_t calculate_string_size(const HashMap*);

static Entry* create_entry(HashMap*, const void*, const void*);

static Entry* get_entry(const HashMap*, const void*);

typedef struct IterationContext IterationContext;

static Iterator* internal_iterator_new(const HashMap*, void* (*)(void*));

static bool internal_iterator_has_next(const void*);

static void* internal_iterator_next(void*);

static void internal_iterator_reset(void*);

static int hash_map_size_wrapper(const void*);

static Iterator* hash_map_entry_iterator(const void*);

static Iterator* hash_map_key_iterator(const void*);

static Iterator* hash_map_value_iterator(const void*);

static bool hash_map_contains_wrapper(const void*, const void*);

static bool hash_map_contains_key_wrapper(const void*, const void*);

static bool hash_map_contains_value_wrapper(const void*, const void*);

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

void hash_map_destroy(HashMap** hash_map_pointer) {
    if (set_error_on_null(hash_map_pointer, *hash_map_pointer)) return;
    HashMap* hash_map = *hash_map_pointer;
    for (int i = 0; i < hash_map->capacity; i++) {
        Entry* current = hash_map->buckets[i];
        while (current) {
            hash_map->key_destruct(current->key);
            hash_map->value_destruct(current->value);

            Entry* temporary = current->next;
            hash_map->memory_free(current);
            current = temporary;
        }
    }
    hash_map->memory_free(hash_map->buckets);
    hash_map->memory_free(hash_map);
    *hash_map_pointer = nullptr;
}

void hash_map_set_destructors(HashMap* hash_map, void (*key_destructor)(void*), void (*value_destructor)(void*)) {
    if (set_error_on_null(hash_map, key_destructor, value_destructor)) return;
    hash_map->key_destruct = key_destructor;
    hash_map->value_destruct = value_destructor;
}

void* hash_map_compute(HashMap* hash_map, const void* key, BiOperator remapper) {
    if (set_error_on_null(hash_map, remapper)) return nullptr;
    void* old_value = hash_map_get(hash_map, key);
    void* new_value = remapper((void*) key, old_value);
    if (new_value) {
        hash_map_put(hash_map, key, new_value);
    } else if (old_value || hash_map_contains_key(hash_map, key)) {
        hash_map_remove(hash_map, key);
    }
    return new_value;
}

void* hash_map_compute_if_absent(HashMap* hash_map, const void* key, Operator mapper) {
    if (set_error_on_null(hash_map, mapper)) return nullptr;
    if (!hash_map_get(hash_map, key)) {
        void* new_value = mapper((void*) key);
        if (new_value) {
            hash_map_put(hash_map, key, new_value);
        }
        return new_value;
    }
    return nullptr;
}

void* hash_map_compute_if_present(HashMap* hash_map, const void* key, BiOperator remapper) {
    if (set_error_on_null(hash_map, remapper)) return nullptr;
    if (hash_map_get(hash_map, key)) {
        return hash_map_compute(hash_map, key, remapper);
    }
    return nullptr;
}

void* hash_map_merge(HashMap* hash_map, const void* key, const void* value, BiOperator remapper) {
    if (set_error_on_null(hash_map, remapper)) return nullptr;
    void* old_value = hash_map_get(hash_map, key);
    void* new_value = !old_value ? (void*) value : remapper(old_value, (void*) value);
    if (!new_value) {
        hash_map_remove(hash_map, key);
    } else {
        hash_map_put(hash_map, key, new_value);
    }
    return new_value;
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
    if (current) {
        entry->next = current;
    }
    hash_map->buckets[entry->hash % hash_map->capacity] = entry;
    hash_map->size++;
    hash_map->modification_count++;
    return nullptr;
}

void* hash_map_put_if_absent(HashMap* hash_map, const void* key, const void* value) {
    if (set_error_on_null(hash_map)) return nullptr;
    void* old_value = hash_map_get(hash_map, key);
    if (!old_value) {
        old_value = hash_map_put(hash_map, key, value);
    }
    return old_value;
}

void* hash_map_get(const HashMap* hash_map, const void* key) {
    if (set_error_on_null(hash_map)) return nullptr;
    const Entry* entry = get_entry(hash_map, key);
    return entry ? entry->value : nullptr;
}

void* hash_map_get_or_default(const HashMap* hash_map, const void* key, const void* default_value) {
    if (set_error_on_null(hash_map)) return nullptr;
    const Entry* entry = get_entry(hash_map, key);
    return entry ? entry->value : (void*) default_value;
}

void* hash_map_replace(HashMap* hash_map, const void* key, const void* value) {
    if (set_error_on_null(hash_map)) return nullptr;
    if (hash_map_contains_key(hash_map, key)) {
        void* old_value = hash_map_put(hash_map, key, value);
        hash_map->value_destruct(old_value);
        return old_value;
    }
    return nullptr;
}

bool hash_map_replace_if_equals(HashMap* hash_map, const void* key, const void* old_value, const void* value) {
    if (set_error_on_null(hash_map)) return false;
    if (hash_map_contains(hash_map, key, old_value)) {
        hash_map->value_destruct(hash_map_put(hash_map, key, value));
        return true;
    }
    return false;
}

void* hash_map_remove(HashMap* hash_map, const void* key) {
    if (set_error_on_null(hash_map)) return nullptr;
    Entry* prev_entry = nullptr, * entry = hash_map->buckets[hash_map->hash(key) % hash_map->capacity];
    while (entry && !hash_map->key_equals(entry->key, key)) {
        prev_entry = entry;
        entry = entry->next;
    }
    if (!entry) {
        return nullptr;
    }
    if (prev_entry) {
        prev_entry->next = entry->next;
    } else {
        hash_map->buckets[hash_map->hash(key) % hash_map->capacity] = nullptr;
    }
    hash_map->key_destruct(entry->key);
    hash_map->value_destruct(entry->value);
    void* value = entry->value;
    hash_map->size--;
    hash_map->modification_count++;
    hash_map->memory_free(entry);
    return value;
}

bool hash_map_remove_if_equals(HashMap* hash_map, const void* key, const void* value) {
    if (set_error_on_null(hash_map)) return false;
    if (hash_map_contains(hash_map, key, value)) {
        hash_map_remove(hash_map, key);
        return true;
    }
    return false;
}

void hash_map_replace_all(HashMap* hash_map, BiOperator remapper) {
    if (set_error_on_null(hash_map, remapper)) return;
    for (int i = 0; i < hash_map->capacity; i++) {
        Entry* entry = hash_map->buckets[i];
        while (entry) {
            void* temporary = entry->value;
            entry->value = remapper(entry->key, entry->value);
            hash_map->value_destruct(temporary);
            entry = entry->next;
        }
    }
}

int hash_map_size(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return 0;
    return hash_map->size;
}

bool hash_map_is_empty(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return false;
    return hash_map->size == 0;
}

Iterator* hash_map_iterator(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return nullptr;
    Iterator* iterator = internal_iterator_new(hash_map, internal_iterator_next);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
    }
    return iterator;
}

bool hash_map_equals(const HashMap* hash_map, const HashMap* other_hash_map) {
    if (set_error_on_null(hash_map, other_hash_map)) return false;
    if (hash_map == other_hash_map) {
        return true;
    }
    if (hash_map->size != other_hash_map->size) {
        return false;
    }
    for (int i = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            if (!hash_map_contains(other_hash_map, entry->key, entry->value)) {
                return false;
            }
            entry = entry->next;
        }
    }
    return true;
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

void hash_map_clear(HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return;
    for (int i = 0; i < hash_map->capacity; i++) {
        Entry* current = hash_map->buckets[i];
        while (current) {
            hash_map->key_destruct(current->key);
            hash_map->value_destruct(current->value);

            Entry* temporary = current->next;
            hash_map->memory_free(current);
            current = temporary;
        }
    }
    memset(hash_map->buckets, 0, hash_map->capacity * sizeof(Entry*));
    hash_map->size = 0;
    hash_map->modification_count++;
}

bool hash_map_contains(const HashMap* hash_map, const void* key, const void* value) {
    if (set_error_on_null(hash_map)) return false;
    const Entry* entry = get_entry(hash_map, key);
    return entry ? hash_map->value_equals(entry->value, value) : false;
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

Collection hash_map_keys(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return (Collection) {};
    return (Collection) {
        .data_structure = hash_map,
        .size = hash_map_size_wrapper,
        .iterator = hash_map_key_iterator,
        .contains = hash_map_contains_key_wrapper
    };
}

Collection hash_map_values(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return (Collection) {};
    return (Collection) {
        .data_structure = hash_map,
        .size = hash_map_size_wrapper,
        .iterator = hash_map_value_iterator,
        .contains = hash_map_contains_value_wrapper
    };
}

Collection hash_map_entries(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return (Collection) {};
    return (Collection) {
        .data_structure = hash_map,
        .size = hash_map_size_wrapper,
        .iterator = hash_map_entry_iterator,
        .contains = hash_map_contains_wrapper
    };
}

HashMap* hash_map_clone(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return nullptr;
    HashMap* new_hash_map; Error error = attempt(new_hash_map = hash_map_new(&(HashMapOptions){
        .initial_capacity = hash_map->capacity,
        .load_factor = hash_map->load_factor,
        .hash = hash_map->hash,
        .key_destruct = hash_map->key_destruct,
        .key_equals = hash_map->key_equals,
        .key_to_string = hash_map->key_to_string,
        .value_destruct = hash_map->value_destruct,
        .value_equals = hash_map->value_equals,
        .value_to_string = hash_map->value_to_string,
        .memory_alloc = hash_map->memory_alloc,
        .memory_realloc = hash_map->memory_realloc,
        .memory_free = hash_map->memory_free
    }));
    if (error) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            if ((error = attempt(hash_map_put(new_hash_map, entry->key, entry->value)))) {
                hash_map_destroy(&new_hash_map);
                set_error(error, "%s", plain_error_message());
                return nullptr;
            }
            entry = entry->next;
        }
    }
    return new_hash_map;
}

char* hash_map_to_string(const HashMap* hash_map) {
    if (set_error_on_null(hash_map)) return nullptr;
    char* string = hash_map->memory_alloc(calculate_string_size(hash_map));

    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "no additional details available");
        return nullptr;
    }

    string[0] = '\0'; // initialize string to clear trash data

    strcat(string, hash_map->size == 0 ? "[" : "[ ");
    for (int i = 0, count = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            constexpr int SEPARATOR = 3; constexpr int NULL_TERMINATOR = 1;

            const int key_length = hash_map->key_to_string(entry->key, nullptr, 0);
            const int value_length = hash_map->value_to_string(entry->value, nullptr, 0);

            char* element_string = hash_map->memory_alloc(key_length + value_length + SEPARATOR + NULL_TERMINATOR);
            if (!element_string) {
                hash_map->memory_free(string);
                set_error(MEMORY_ALLOCATION_ERROR, "no additional details available");
                return nullptr;
            }

            hash_map->key_to_string(entry->key, element_string, key_length + NULL_TERMINATOR);
            strcat(element_string, " = ");
            hash_map->value_to_string(entry->value, element_string + key_length + SEPARATOR, value_length + NULL_TERMINATOR);

            strcat(string, element_string);
            if (count < hash_map->size - 1) {
                strcat(string, ", ");
            }
            count++;

            hash_map->memory_free(element_string);
            entry = entry->next;
        }
    }
    strcat(string, hash_map->size == 0 ? "]" : " ]");

    return string;
}

static size_t calculate_string_size(const HashMap* hash_map) {
    constexpr int BRACKETS = 2; constexpr int SEPARATOR = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0, count = 0; i < hash_map->capacity; i++) {
        const Entry* entry = hash_map->buckets[i];
        while (entry) {
            length += hash_map->key_to_string(entry->key, nullptr, 0);
            length += hash_map->value_to_string(entry->value, nullptr, 0);
            length += 3; // ' = ' separator

            if (count == 0) length += 1; // space after opening bracket
            if (count < hash_map->size - 1) length += SEPARATOR; // prevent separator on the last element
            if (count == hash_map->size - 1) length += 1; // space before closing bracket

            entry = entry->next;
            count++;
        }
    }
    return length + BRACKETS + NULL_TERMINATOR;
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

struct IterationContext {
    const HashMap* hash_map;
    Entry* entry;
    int cursor;
    int count;
    int modification_count;
};

static Iterator* internal_iterator_new(const HashMap* hash_map, void* next_function(void*)) {
    IterationContext* iteration_context = hash_map->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->hash_map = hash_map;
    iteration_context->entry = nullptr;
    iteration_context->cursor = 0;
    iteration_context->count = 0;
    iteration_context->modification_count = hash_map->modification_count;

    Iterator* iterator = iterator_new(iteration_context, internal_iterator_has_next,
        next_function, internal_iterator_reset, hash_map->memory_alloc, hash_map->memory_free);

    if (!iterator) {
        hash_map->memory_free(iteration_context);
        return nullptr;
    }
    return iterator;
}

static bool internal_iterator_has_next(const void* internal_state) {
    const IterationContext* iteration_context = internal_state;
    return iteration_context->count < iteration_context->hash_map->size;
}

static void* internal_iterator_next(void* internal_state) {
    IterationContext* iteration_context = internal_state;
    if (iteration_context->modification_count != iteration_context->hash_map->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!internal_iterator_has_next(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    while (true) {
        if (!iteration_context->entry) {
            iteration_context->entry = iteration_context->hash_map->buckets[iteration_context->cursor++];
        } else {
            iteration_context->entry = iteration_context->entry->next;
        }
        if (iteration_context->entry) {
            iteration_context->count++;
            return iteration_context->entry;
        }
    }
}

static void* internal_iterator_next_key(void* internal_state) {
    const Entry* entry = internal_iterator_next(internal_state);
    return entry ? entry->key : nullptr;
}

static void* internal_iterator_next_value(void* internal_state) {
    const Entry* entry = internal_iterator_next(internal_state);
    return entry ? entry->value : nullptr;
}

static void internal_iterator_reset(void* internal_state) {
    IterationContext* iteration_context = internal_state;
    iteration_context->entry = nullptr;
    iteration_context->cursor = 0;
    iteration_context->count = 0;
}

static int hash_map_size_wrapper(const void* hash_map) {
    return hash_map_size(hash_map);
}

static Iterator* hash_map_entry_iterator(const void* hash_map) {
    return hash_map_iterator(hash_map);
}

static Iterator* hash_map_key_iterator(const void* hash_map) {
    if (set_error_on_null(hash_map)) return nullptr;
    Iterator* iterator = internal_iterator_new(hash_map, internal_iterator_next_key);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
    }
    return iterator;
}

static Iterator* hash_map_value_iterator(const void* hash_map) {
    if (set_error_on_null(hash_map)) return nullptr;
    Iterator* iterator = internal_iterator_new(hash_map, internal_iterator_next_value);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
    }
    return iterator;
}

// TODO: maybe fix this?
static bool hash_map_contains_wrapper(const void* hash_map, const void* entry) {
    (void) hash_map, (void) entry;
    set_error(UNSUPPORTED_OPERATION_ERROR, "hash_map_contains is not supported for this iterator");
    return false; // hash_map_contains(hash_map, ((Entry*) entry)->key, ((Entry*) entry)->value);
}

static bool hash_map_contains_key_wrapper(const void* hash_map, const void* key) {
    return hash_map_contains_key(hash_map, key);
}

static bool hash_map_contains_value_wrapper(const void* hash_map, const void* value) {
    return hash_map_contains_value(hash_map, value);
}