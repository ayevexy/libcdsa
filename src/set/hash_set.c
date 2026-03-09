#include "hash_set.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

constexpr int MIN_CAPACITY = 8;
constexpr int MAX_CAPACITY = 1'073'741'824;
constexpr float MIN_LOAD_FACTOR = 0.5;

typedef struct Node {
    void* element;
    struct Node* next;
} Node;

struct HashSet {
    Node** buckets;
    int size;
    int capacity;
    int threshold;
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
    int modification_count;
};

static int next_power_of_two(int x);

static Node* create_node(const HashSet*, const void*);

HashSet* hash_set_new(const HashSetOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->load_factor < MIN_LOAD_FACTOR || !options->hash || !options->destruct
        || !options->equals || !options->to_string || !options->memory_alloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    HashSet* hash_set = options->memory_alloc(sizeof(HashSet));
    if (!hash_set) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_set'");
        return nullptr;
    }
    hash_set->capacity = next_power_of_two(options->initial_capacity);
    hash_set->buckets = options->memory_alloc(hash_set->capacity * sizeof(Node*));
    if (!hash_set->buckets) {
        options->memory_free(hash_set);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_set'");
        return nullptr;
    }
    memset(hash_set->buckets, 0, hash_set->capacity * sizeof(Node*));
    hash_set->size = 0;
    hash_set->threshold = hash_set->capacity * options->load_factor;
    hash_set->load_factor = options->load_factor;
    hash_set->hash = options->hash;
    hash_set->destruct = options->destruct;
    hash_set->equals = options->equals;
    hash_set->to_string = options->to_string;
    hash_set->memory_alloc = options->memory_alloc;
    hash_set->memory_free = options->memory_free;
    hash_set->modification_count = 0;
    return hash_set;
}

void hash_set_destroy(HashSet** hash_set_pointer) {
    if (require_non_null(hash_set_pointer, *hash_set_pointer)) return;
    HashSet* hash_set = *hash_set_pointer;
    for (int i = 0; i < hash_set->capacity; i++) {
        Node* current = hash_set->buckets[i];
        while (current) {
            hash_set->destruct(current->element);

            Node* temporary = current->next;
            hash_set->memory_free(current);
            current = temporary;
        }
    }
    hash_set->memory_free(hash_set->buckets);
    hash_set->memory_free(hash_set);
    *hash_set_pointer = nullptr;
}

void (*hash_set_get_destructor(const HashSet* hash_set))(void*) {
    if (require_non_null(hash_set)) return nullptr;
    return hash_set->destruct;
}

void hash_set_set_destructor(HashSet* hash_set, void (*destructor)(void*)) {
    if (require_non_null(hash_set, destructor)) return;
    hash_set->destruct = destructor;
}

bool hash_set_add(HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    if (hash_set_contains(hash_set, element)) {
        return false;
    }
    Node* node = create_node(hash_set, element);
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new node'");
        return false;
    }
    const int index = hash_set->hash(element) & (hash_set->capacity - 1);
    Node* current = hash_set->buckets[index];
    if (current) {
        node->next = current;
    }
    hash_set->buckets[index] = node;
    hash_set->size++;
    hash_set->modification_count++;
    return true;
}

bool hash_set_remove(HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    const int index = hash_set->hash(element) & (hash_set->capacity - 1);
    Node* prev_node = nullptr, * node = hash_set->buckets[index];
    while (node && !hash_set->equals(node->element, element)) {
        prev_node = node;
        node = node->next;
    }
    if (!node) {
        return false;
    }
    if (prev_node) {
        prev_node->next = node->next;
    } else {
        hash_set->buckets[index] = node->next;
    }
    hash_set->destruct(node->element);
    const void* value = node->element;
    hash_set->size--;
    hash_set->modification_count++;
    hash_set->memory_free(node);
    return value;
}

int hash_set_size(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return 0;
    return hash_set->size;
}

bool hash_set_contains(const HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    const Node* node = hash_set->buckets[hash_set->hash(element) & (hash_set->capacity - 1)];
    while (node) {
        if (hash_set->equals(node->element, element)) {
            return true;
        }
        node = node->next;
    }
    return false;
}

static int next_power_of_two(int x) {
    int power = 1;
    while (power < x) {
        power <<= 1;
    }
    return power;
}

static Node* create_node(const HashSet* hash_set, const void* element) {
    Node* node = hash_set->memory_alloc(sizeof(Node));
    if (!node) {
        return nullptr;
    }
    node->element = (void*) element;
    node->next = nullptr;
    return node;
}