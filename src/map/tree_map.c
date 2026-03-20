#include "tree_map.h"

#include "util/errors.h"
#include "util/constraints.h"

typedef enum {
    RED,
    BLACK
} Color;

typedef struct Entry {
    union {
        MapEntry entry;
        struct { void* key; void* value; };
    };
    Color color;
    struct Entry* parent;
    struct Entry* left;
    struct Entry* right;
} Entry;

static Entry sentinel = { .color = BLACK };

struct TreeMap {
    Entry* root;
    int size;
    Comparator compare_keys;
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
        void (*memory_free)(void*);
    };
    int modification_count;
};

static Entry* create_entry(const TreeMap*, const void*, const void*);

static Entry* get_entry(const TreeMap*, const void*);

static Entry* get_successor_entry(Entry*);

static Entry* get_lower_entry(Entry*);

TreeMap* tree_map_new(const TreeMapOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (!options->compare_keys || !options->key_destruct || !options->key_equals
        || !options->key_to_string || !options->value_destruct || !options->value_equals
        || !options->value_to_string || !options->memory_alloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    TreeMap* tree_map = options->memory_alloc(sizeof(TreeMap));
    if (!tree_map) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'tree_map'");
        return nullptr;
    }
    tree_map->root = &sentinel;
    tree_map->size = 0;
    tree_map->compare_keys = options->compare_keys;
    tree_map->key_destruct = options->key_destruct;
    tree_map->key_equals = options->key_equals;
    tree_map->key_to_string = options->key_to_string;
    tree_map->value_destruct = options->value_destruct;
    tree_map->value_equals = options->value_equals;
    tree_map->value_to_string = options->value_to_string;
    tree_map->memory_alloc = options->memory_alloc;
    tree_map->memory_free = options->memory_free;
    return tree_map;
}

void tree_map_destroy(TreeMap** tree_map_pointer) {
    if (require_non_null(tree_map_pointer, *tree_map_pointer)) return;
    TreeMap* tree_map = *tree_map_pointer;
    // Todo: deletion logic
    tree_map->memory_free(tree_map);
    *tree_map_pointer = nullptr;
}

void tree_map_set_key_destructor(TreeMap* tree_map, void(*destructor)(void*)) {
    if (require_non_null(tree_map)) return;
    tree_map->key_destruct = destructor;
}

void tree_map_set_value_destructor(TreeMap* tree_map, void(*destructor)(void*)) {
    if (require_non_null(tree_map)) return;
    tree_map->value_destruct = destructor;
}

void* tree_map_put(TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return nullptr;
    Entry* current = tree_map->root, * previous = &sentinel;
    while (current != &sentinel) {
        const int result = tree_map->compare_keys(key, current->key);
        previous = current;
        if (result < 0) {
            current = current->left;
        } else if (result > 0) {
            current = current->right;
        } else {
            void* old_value = current->value;
            current->value = (void*) value;
            if (old_value != value) {
                tree_map->value_destruct(old_value);
            }
            return old_value;
        }
    }
    current = previous;
    Entry* entry = create_entry(tree_map, key, value);
    if (!entry) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new entry'");
        return nullptr;
    }
    entry->parent = current;
    if (current == &sentinel) {
        entry->color = BLACK;
        tree_map->root = entry;
    } else {
        entry->color = RED;
        if (tree_map->compare_keys(key, current->key) < 0) {
            current->left = entry;
        } else {
            current->right = entry;
        }
        // TODO: rebalance
    }
    tree_map->size++;
    tree_map->modification_count++;
    return nullptr;
}

void* tree_map_put_if_absent(TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return nullptr;
    void* old_value = tree_map_get(tree_map, key);
    if (!old_value) {
        old_value = tree_map_put(tree_map, key, value);
    }
    return old_value;
}

void* tree_map_get(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_entry(tree_map, key);
    return entry ? entry->value : nullptr;
}

void* tree_map_get_or_default(const TreeMap* tree_map, const void* key, const void* default_value) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_entry(tree_map, key);
    return entry ? entry->value : (void*) default_value;
}

int tree_map_size(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return 0;
    return tree_map->size;
}

bool tree_map_is_empty(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return false;
    return tree_map->size == 0;
}

bool tree_map_contains_entry(const TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return false;
    const Entry* entry = get_entry(tree_map, key);
    return entry ? tree_map->value_equals(entry->value, value) : false;
}

bool tree_map_contains_key(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return false;
    return get_entry(tree_map, key) != nullptr;
}

bool tree_map_contains_value(const TreeMap* tree_map, const void* value) {
    if (require_non_null(tree_map)) return false;
    Entry* current = get_lower_entry(tree_map->root);
    while (current != &sentinel) {
        if (tree_map->value_equals(value, current->value)) {
            return true;
        }
        current = get_successor_entry(current);
    }
    return false;
}

static Entry* create_entry(const TreeMap* tree_map, const void* key, const void* value) {
    Entry* entry = tree_map->memory_alloc(sizeof(Entry));
    if (!entry) {
        return nullptr;
    }
    entry->key = (void*) key;
    entry->value = (void*) value;
    entry->parent = nullptr;
    entry->left = &sentinel;
    entry->right = &sentinel;
    return entry;
}

static Entry* get_entry(const TreeMap* tree_map, const void* key) {
    Entry* current = tree_map->root;
    while (current != &sentinel) {
        const int result = tree_map->compare_keys(key, current->key);
        if (result < 0) {
            current = current->left;
        } else if (result > 0) {
            current = current->right;
        } else {
            return current;
        }
    }
    return nullptr;
}

static Entry* get_successor_entry(Entry* entry) {
    if (entry->right != &sentinel) {
        return get_lower_entry(entry->right);
    }
    Entry* parent = entry->parent;
    while (parent != &sentinel && entry == parent->right) {
        entry = parent;
        parent = parent->parent;
    }
    return parent;
}

static Entry* get_lower_entry(Entry* entry) {
    if (entry == &sentinel) {
        return entry;
    }
    while (entry->left != &sentinel) {
        entry = entry->left;
    }
    return entry;
}