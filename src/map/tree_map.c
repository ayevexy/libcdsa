#include "tree_map.h"

#include "util/errors.h"
#include "util/constraints.h"

typedef enum {
    RED,
    BLACK
} Color;

typedef struct Node {
    union {
        MapEntry entry;
        struct { void* key; void* value; };
    };
    Color color;
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

struct TreeMap {
    Node* root;
    Node* sentinel;
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
    tree_map->root = nullptr;
    tree_map->sentinel = nullptr;
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