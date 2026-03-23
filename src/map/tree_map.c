#include "tree_map.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

typedef enum {
    RED, BLACK
} Color;

typedef struct Entry {
    union {
        MapEntry view;
        struct { void* key; void* value; };
    };
    Color color;
    struct Entry* parent;
    struct Entry* left;
    struct Entry* right;
} Entry;

struct TreeMap {
    Entry* root;
    Entry* sentinel;
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

static size_t calculate_string_size(const TreeMap*);

static Entry* create_entry(const TreeMap*, const void*, const void*);

static Entry* get_entry(const TreeMap*, const void*);

static Entry* get_successor_entry(const TreeMap*, Entry*);

static Entry* get_predecessor_entry(const TreeMap*, Entry*);

static Entry* get_lower_entry(const TreeMap*, Entry*);

static Entry* get_higher_entry(const TreeMap*, Entry*);

static void remove_entry(TreeMap*, Entry*);

static void transplant(TreeMap*, Entry*, Entry*);

static void destroy_entries(TreeMap*, Entry*);

static void rebalance_after_insert(TreeMap*, Entry*);

static void rebalance_after_delete(TreeMap*, Entry*);

static void rotate_left(TreeMap*, Entry*);

static void rotate_right(TreeMap*, Entry*);

static Iterator* create_iterator(const TreeMap*, void* (*)(void*), void* (*)(void*));

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static bool iterator_has_previous_internal(const void*);

static void* iterator_previous_internal(void*);

static void iterator_add_internal(void*, const void*);

static void iterator_set_internal(void*, const void*);

static void iterator_remove_internal(void*);

static void iterator_reset_internal(void*);

static int collection_size_internal(const void*);

static Iterator* entry_collection_iterator_internal(const void*);

static Iterator* key_collection_iterator_internal(const void*);

static Iterator* value_collection_iterator_internal(const void*);

static bool entry_collection_contains_internal(const void*, const void*);

static bool key_collection_contains_internal(const void*, const void*);

static bool value_collection_contains_internal(const void*, const void*);

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
    Entry* sentinel = options->memory_alloc(sizeof(Entry));
    if (!sentinel) {
        options->memory_free(tree_map);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'tree_map'");
        return nullptr;
    }
    memset(sentinel, 0, sizeof(Entry));
    sentinel->color = BLACK;

    tree_map->root = sentinel;
    tree_map->sentinel = sentinel;
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

TreeMap* tree_map_from(Collection entry_collection, const TreeMapOptions* options) {
    if (require_non_null(options)) return nullptr;
    TreeMap* tree_map; Error error;

    if ((error = attempt(tree_map = tree_map_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(tree_map_put_all(tree_map, entry_collection)))) {
        tree_map_destroy(&tree_map);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return tree_map;
}

void tree_map_destroy(TreeMap** tree_map_pointer) {
    if (require_non_null(tree_map_pointer, *tree_map_pointer)) return;
    TreeMap* tree_map = *tree_map_pointer;
    destroy_entries(tree_map, tree_map->root);
    tree_map->memory_free(tree_map->sentinel);
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

void* tree_map_compute(TreeMap* tree_map, const void* key, BiOperator remapper) {
    if (require_non_null(tree_map, remapper)) return nullptr;
    void* old_value = tree_map_get(tree_map, key);
    void* new_value = remapper((void*) key, old_value);
    if (new_value) {
        tree_map_put(tree_map, key, new_value);
    } else if (old_value || tree_map_contains_key(tree_map, key)) {
        tree_map_remove(tree_map, key);
    }
    return new_value;
}

void* tree_map_compute_if_absent(TreeMap* tree_map, const void* key, Operator mapper) {
    if (require_non_null(tree_map, mapper)) return nullptr;
    if (!tree_map_contains_key(tree_map, key)) {
        void* new_value = mapper((void*) key);
        if (new_value) {
            tree_map_put(tree_map, key, new_value);
        }
        return new_value;
    }
    return nullptr;
}

void* tree_map_compute_if_present(TreeMap* tree_map, const void* key, BiOperator remapper) {
    if (require_non_null(tree_map, remapper)) return nullptr;
    if (tree_map_contains_key(tree_map, key)) {
        return tree_map_compute(tree_map, key, remapper);
    }
    return nullptr;
}

void* tree_map_merge(TreeMap* tree_map, const void* key, const void* value, BiOperator remapper) {
    if (require_non_null(tree_map, remapper)) return nullptr;
    void* old_value = tree_map_get(tree_map, key);
    void* new_value = !old_value ? (void*) value : remapper(old_value, (void*) value);
    if (!new_value) {
        tree_map_remove(tree_map, key);
    } else {
        tree_map_put(tree_map, key, new_value);
    }
    return new_value;
}

void* tree_map_put(TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return nullptr;
    Entry* current = tree_map->root, * previous = tree_map->sentinel;
    while (current != tree_map->sentinel) {
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
    if (current == tree_map->sentinel) {
        entry->color = BLACK;
        tree_map->root = entry;
    } else {
        entry->color = RED;
        if (tree_map->compare_keys(key, current->key) < 0) {
            current->left = entry;
        } else {
            current->right = entry;
        }
        rebalance_after_insert(tree_map, entry);
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

void tree_map_put_all(TreeMap* tree_map, Collection entry_collection) {
    if (require_non_null(tree_map)) return;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(entry_collection)))) {
        set_error(error, "%s of 'entry collection'", plain_error_message());
        return;
    }
    while (iterator_has_next(iterator)) {
        const MapEntry* entry = iterator_next(iterator);
        if ((error = attempt(tree_map_put(tree_map, entry->key, entry->value)))) {
            set_error(error, "%s", plain_error_message());
            break;
        }
    }
    iterator_destroy(&iterator);
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

MapEntry tree_map_first_entry(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    const Entry* entry = get_lower_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    return entry->view;
}

MapEntry tree_map_last_entry(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    const Entry* entry = get_higher_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    return entry->view;
}

void* tree_map_first_key(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_lower_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return nullptr;
    }
    return entry->key;
}

void* tree_map_last_key(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_higher_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return nullptr;
    }
    return entry->key;
}

void* tree_map_replace(TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return nullptr;
    if (tree_map_contains_key(tree_map, key)) {
        return tree_map_put(tree_map, key, value);
    }
    return nullptr;
}

bool tree_map_replace_if_equals(TreeMap* tree_map, const void* key, const void* old_value, const void* value) {
    if (require_non_null(tree_map)) return false;
    if (tree_map_contains_entry(tree_map, key, old_value)) {
        tree_map_put(tree_map, key, value);
        return true;
    }
    return false;
}

void* tree_map_remove(TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return nullptr;
    Entry* entry = get_entry(tree_map, key);
    if (!entry) {
        return nullptr;
    }
    void* value = entry->value;
    tree_map->value_destruct(value);
    remove_entry(tree_map, entry);
    return value;
}

bool tree_map_remove_if_equals(TreeMap* tree_map, const void* key, const void* value) {
    if (require_non_null(tree_map)) return false;
    if (tree_map_contains_entry(tree_map, key, value)) {
        tree_map_remove(tree_map, key);
        return true;
    }
    return false;
}

MapEntry tree_map_poll_first_entry(TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    const MapEntry view = entry->view;
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    remove_entry(tree_map, entry);
    return view;
}

MapEntry tree_map_poll_last_entry(TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* entry = get_higher_entry(tree_map, tree_map->root);
    if (entry == tree_map->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    const MapEntry view = entry->view;
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    remove_entry(tree_map, entry);
    return view;
}

void tree_map_replace_all(TreeMap* tree_map, BiOperator bi_operator) {
    if (require_non_null(tree_map, bi_operator)) return;
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    while (entry != tree_map->sentinel) {
        void* old_value = entry->value;
        void* new_value = bi_operator(entry->key, entry->value);
        if (old_value != new_value) {
            tree_map->value_destruct(old_value);
        }
        entry->value = new_value;
        entry = get_successor_entry(tree_map, entry);
    }
}

int tree_map_size(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return 0;
    return tree_map->size;
}

bool tree_map_is_empty(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return false;
    return tree_map->size == 0;
}

Iterator* tree_map_iterator(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    Iterator* iterator = create_iterator(tree_map, iterator_next_internal, iterator_previous_internal);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool tree_map_equals(const TreeMap* tree_map, const TreeMap* other_tree_map) {
    if (require_non_null(tree_map, other_tree_map)) return false;
    if (tree_map == other_tree_map) {
        return true;
    }
    if (tree_map->size != other_tree_map->size) {
        return false;
    }
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    Entry* other_entry = get_lower_entry(other_tree_map, other_tree_map->root);

    while (entry != tree_map->sentinel && other_entry != tree_map->sentinel) {
        if (!(tree_map->key_equals(other_entry->key, entry->key)
            && tree_map->value_equals(entry->value, other_entry->value))
        ) {
            return false;
        }
        entry = get_successor_entry(tree_map, entry);
        other_entry = get_successor_entry(other_tree_map, other_entry);
    }
    return true;
}

void tree_map_for_each(TreeMap* tree_map, BiConsumer action) {
    if (require_non_null(tree_map, action)) return;
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    while (entry != tree_map->sentinel) {
        action(entry->key, entry->value);
        entry = get_successor_entry(tree_map, entry);
    }
}

void tree_map_clear(TreeMap* tree_map) {
    if (require_non_null(tree_map)) return;
    destroy_entries(tree_map, tree_map->root);
    tree_map->root = tree_map->sentinel;
    tree_map->size = 0;
    tree_map->modification_count++;
}

MapEntry tree_map_higher_entry(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* current = get_entry(tree_map, key);
    if (!current) {
        return (MapEntry) {};
    }
    const Entry* entry = get_successor_entry(tree_map, current);
    return entry != tree_map->sentinel ? entry->view : (MapEntry) {};
}

MapEntry tree_map_ceiling_entry(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* current = get_entry(tree_map, key);
    if (!current) {
        return (MapEntry) {};
    }
    const Entry* entry = get_successor_entry(tree_map, current);
    return entry != tree_map->sentinel ? entry->view : current->view;
}

MapEntry tree_map_floor_entry(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* current = get_entry(tree_map, key);
    if (!current) {
        return (MapEntry) {};
    }
    const Entry* entry = get_predecessor_entry(tree_map, current);
    return entry != tree_map->sentinel ? entry->view : current->view;
}

MapEntry tree_map_lower_entry(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* current = get_entry(tree_map, key);
    if (!current) {
        return (MapEntry) {};
    }
    const Entry* entry = get_predecessor_entry(tree_map, current);
    return entry != tree_map->sentinel ? entry->view : (MapEntry) {};
}

void* tree_map_higher_key(const TreeMap* tree_map, const void* key) {
    return tree_map_higher_entry(tree_map, key).key;
}

void* tree_map_ceiling_key(const TreeMap* tree_map, const void* key) {
    return tree_map_ceiling_entry(tree_map, key).key;
}

void* tree_map_floor_key(const TreeMap* tree_map, const void* key) {
    return tree_map_floor_entry(tree_map, key).key;
}

void* tree_map_lower_key(const TreeMap* tree_map, const void* key) {
    return tree_map_lower_entry(tree_map, key).key;
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
    Entry* current = get_lower_entry(tree_map, tree_map->root);
    while (current != tree_map->sentinel) {
        if (tree_map->value_equals(value, current->value)) {
            return true;
        }
        current = get_successor_entry(tree_map, current);
    }
    return false;
}

Collection tree_map_keys(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (Collection) {};
    return (Collection) {
        .data_structure = tree_map,
        .size = collection_size_internal,
        .iterator = key_collection_iterator_internal,
        .contains = key_collection_contains_internal
    };
}

Collection tree_map_values(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (Collection) {};
    return (Collection) {
        .data_structure = tree_map,
        .size = collection_size_internal,
        .iterator = value_collection_iterator_internal,
        .contains = value_collection_contains_internal
    };
}

Collection tree_map_entries(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (Collection) {};
    return (Collection) {
        .data_structure = tree_map,
        .size = collection_size_internal,
        .iterator = entry_collection_iterator_internal,
        .contains = entry_collection_contains_internal
    };
}

// TODO: refactor
TreeMap* tree_map_head_map(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return nullptr;
    if (!tree_map_contains_key(tree_map, key)) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "inexistent 'key");
        return nullptr;
    }
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(&(TreeMapOptions){
       .compare_keys = tree_map->compare_keys,
       .key_destruct = noop_destruct,
       .key_equals = tree_map->key_equals,
       .key_to_string = tree_map->key_to_string,
       .value_destruct = noop_destruct,
       .value_equals = tree_map->value_equals,
       .value_to_string = tree_map->value_to_string,
       .memory_alloc = tree_map->memory_alloc,
       .memory_free = tree_map->memory_free
   }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    Entry* entry = get_entry(tree_map, key);
    while (entry != tree_map->sentinel) {
        if ((error = attempt(tree_map_put(new_tree_map, entry->key, entry->value)))) {
            tree_map_destroy(&new_tree_map);
            set_error(error, "%s", plain_error_message());
            return nullptr;
        }
        entry = get_predecessor_entry(tree_map, entry);
    }
    return new_tree_map;
}

TreeMap* tree_map_tail_map(const TreeMap* tree_map, const void* key) {
    if (require_non_null(tree_map)) return nullptr;
    if (!tree_map_contains_key(tree_map, key)) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "inexistent 'key");
        return nullptr;
    }
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(&(TreeMapOptions){
       .compare_keys = tree_map->compare_keys,
       .key_destruct = noop_destruct,
       .key_equals = tree_map->key_equals,
       .key_to_string = tree_map->key_to_string,
       .value_destruct = noop_destruct,
       .value_equals = tree_map->value_equals,
       .value_to_string = tree_map->value_to_string,
       .memory_alloc = tree_map->memory_alloc,
       .memory_free = tree_map->memory_free
   }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    Entry* entry = get_entry(tree_map, key);
    while (entry != tree_map->sentinel) {
        if ((error = attempt(tree_map_put(new_tree_map, entry->key, entry->value)))) {
            tree_map_destroy(&new_tree_map);
            set_error(error, "%s", plain_error_message());
            return nullptr;
        }
        entry = get_successor_entry(tree_map, entry);
    }
    return new_tree_map;
}

TreeMap* tree_map_sub_map(const TreeMap* tree_map, const void* start_key, const void* end_key) {
    if (require_non_null(tree_map)) return nullptr;
    if (!tree_map_contains_key(tree_map, start_key)
        || !tree_map_contains_key(tree_map, end_key)
        || tree_map->compare_keys(start_key, end_key) > 0
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'start_key' or 'end_key' are inexistent or 'start_key' is greater than 'end_key'");
        return nullptr;
    }
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(&(TreeMapOptions){
       .compare_keys = tree_map->compare_keys,
       .key_destruct = noop_destruct,
       .key_equals = tree_map->key_equals,
       .key_to_string = tree_map->key_to_string,
       .value_destruct = noop_destruct,
       .value_equals = tree_map->value_equals,
       .value_to_string = tree_map->value_to_string,
       .memory_alloc = tree_map->memory_alloc,
       .memory_free = tree_map->memory_free
   }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    Entry* entry = get_entry(tree_map, start_key);
    while (entry != tree_map->sentinel && !tree_map->key_equals(entry->key, end_key)) {
        if ((error = attempt(tree_map_put(new_tree_map, entry->key, entry->value)))) {
            tree_map_destroy(&new_tree_map);
            set_error(error, "%s", plain_error_message());
            return nullptr;
        }
        entry = get_successor_entry(tree_map, entry);
    }
    return new_tree_map;
}

TreeMap* tree_map_clone(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    TreeMap* new_tree_map; Error error = attempt(new_tree_map = tree_map_new(&(TreeMapOptions){
        .compare_keys = tree_map->compare_keys,
        .key_destruct = noop_destruct,
        .key_equals = tree_map->key_equals,
        .key_to_string = tree_map->key_to_string,
        .value_destruct = noop_destruct,
        .value_equals = tree_map->value_equals,
        .value_to_string = tree_map->value_to_string,
        .memory_alloc = tree_map->memory_alloc,
        .memory_free = tree_map->memory_free
    }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    while (entry != tree_map->sentinel) {
        if ((error = attempt(tree_map_put(new_tree_map, entry->key, entry->value)))) {
            tree_map_destroy(&new_tree_map);
            set_error(error, "%s", plain_error_message());
            return nullptr;
        }
        entry = get_successor_entry(tree_map, entry);
    }
    return new_tree_map;
}

char* tree_map_to_string(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;

    char* string = tree_map->memory_alloc(calculate_string_size(tree_map));
    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "no additional details available");
        return nullptr;
    }
    string[0] = '\0'; // initialize string to clear trash data
    strcat(string, tree_map->size == 0 ? "[" : "[ ");

    int count = 0;
    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    while (entry != tree_map->sentinel) {
        constexpr int SEPARATOR = 3; constexpr int NULL_TERMINATOR = 1;

        const int key_length = tree_map->key_to_string(entry->key, nullptr, 0);
        const int value_length = tree_map->value_to_string(entry->value, nullptr, 0);

        char* element_string = tree_map->memory_alloc(key_length + value_length + SEPARATOR + NULL_TERMINATOR);
        if (!element_string) {
            tree_map->memory_free(string);
            set_error(MEMORY_ALLOCATION_ERROR, "no additional details available");
            return nullptr;
        }
        tree_map->key_to_string(entry->key, element_string, key_length + NULL_TERMINATOR);
        strcat(element_string, " = ");
        tree_map->value_to_string(entry->value, element_string + key_length + SEPARATOR, value_length + NULL_TERMINATOR);

        strcat(string, element_string);
        if (count < tree_map->size - 1) {
            strcat(string, ", ");
        }
        count++;
        tree_map->memory_free(element_string);
        entry = get_successor_entry(tree_map, entry);
    }

    strcat(string, tree_map->size == 0 ? "]" : " ]");
    return string;
}

static size_t calculate_string_size(const TreeMap* tree_map) {
    constexpr int BRACKETS = 2; constexpr int SEPARATOR = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;
    int count = 0;

    Entry* entry = get_lower_entry(tree_map, tree_map->root);
    while (entry != tree_map->sentinel) {
        length += tree_map->key_to_string(entry->key, nullptr, 0);
        length += tree_map->value_to_string(entry->value, nullptr, 0);
        length += 3; // ' = ' separator

        if (count == 0) length += 1; // space after opening bracket
        if (count < tree_map->size - 1) length += SEPARATOR; // prevent separator on the last element
        if (count == tree_map->size - 1) length += 1; // space before closing bracket

        entry = get_successor_entry(tree_map, entry);
        count++;
    }
    return length + BRACKETS + NULL_TERMINATOR;
}

static Entry* create_entry(const TreeMap* tree_map, const void* key, const void* value) {
    Entry* entry = tree_map->memory_alloc(sizeof(Entry));
    if (!entry) {
        return nullptr;
    }
    entry->key = (void*) key;
    entry->value = (void*) value;
    entry->parent = tree_map->sentinel;
    entry->left = tree_map->sentinel;
    entry->right = tree_map->sentinel;
    return entry;
}

static Entry* get_entry(const TreeMap* tree_map, const void* key) {
    Entry* current = tree_map->root;
    while (current != tree_map->sentinel) {
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

static Entry* get_successor_entry(const TreeMap* tree_map, Entry* entry) {
    if (entry->right != tree_map->sentinel) {
        return get_lower_entry(tree_map, entry->right);
    }
    Entry* parent = entry->parent;
    while (parent != tree_map->sentinel && entry == parent->right) {
        entry = parent;
        parent = parent->parent;
    }
    return parent;
}

static Entry* get_predecessor_entry(const TreeMap* tree_map, Entry* entry) {
    if (entry->left != tree_map->sentinel) {
        return get_higher_entry(tree_map, entry->left);
    }
    Entry* parent = entry->parent;
    while (parent != tree_map->sentinel && entry == parent->left) {
        entry = parent;
        parent = parent->parent;
    }
    return parent;
}

static Entry* get_lower_entry(const TreeMap* tree_map, Entry* entry) {
    if (entry == tree_map->sentinel) {
        return entry;
    }
    while (entry->left != tree_map->sentinel) {
        entry = entry->left;
    }
    return entry;
}

static Entry* get_higher_entry(const TreeMap* tree_map, Entry* entry) {
    if (entry == tree_map->sentinel) {
        return entry;
    }
    while (entry->right != tree_map->sentinel) {
        entry = entry->right;
    }
    return entry;
}

static void remove_entry(TreeMap* tree_map, Entry* entry) {
    Entry* current = entry, * auxiliar;
    Color current_color = current->color;
    if (entry->left == tree_map->sentinel) {
        auxiliar = entry->right;
        transplant(tree_map, entry, entry->right);
    } else if (entry->right == tree_map->sentinel) {
        auxiliar = entry->left;
        transplant(tree_map, entry, entry->left);
    } else {
        current = get_lower_entry(tree_map, entry->right);
        current_color = current->color;
        auxiliar = current->right;
        if (current->parent == entry) {
            auxiliar->parent = current;
        } else {
            transplant(tree_map, current, current->right);
            current->right = entry->right;
            current->right->parent = current;
        }
        transplant(tree_map, entry, current);
        current->left = entry->left;
        current->left->parent = current;
        current->color = entry->color;
    }
    if (current_color == BLACK) {
        rebalance_after_delete(tree_map, auxiliar);
    }
    tree_map->memory_free(entry);
    tree_map->size--;
    tree_map->modification_count++;
}

static void transplant(TreeMap* tree_map, Entry* first, Entry* second) {
    if (first->parent == tree_map->sentinel) {
        tree_map->root = second;
    } else if (first == first->parent->left) {
        first->parent->left = second;
    } else {
        first->parent->right = second;
    }
    second->parent = first->parent;
}

static void destroy_entries(TreeMap* tree_map, Entry* entry) {
    if (entry == tree_map->sentinel) {
        return;
    }
    destroy_entries(tree_map, entry->left);
    destroy_entries(tree_map, entry->right);
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    tree_map->memory_free(entry);
}

static void rebalance_after_insert(TreeMap* tree_map, Entry* entry) {
    Entry* current;
    while (entry->parent->color == RED) {
        if (entry->parent == entry->parent->parent->left) {
            current = entry->parent->parent->right;
            if (current->color == RED) {
                entry->parent->color = BLACK;
                current->color = BLACK;
                entry->parent->parent->color = RED;
                entry = entry->parent->parent;
            } else {
                if (entry == entry->parent->right) {
                    entry = entry->parent;
                    rotate_left(tree_map, entry);
                }
                entry->parent->color = BLACK;
                entry->parent->parent->color = RED;
                rotate_right(tree_map, entry->parent->parent);
            }
        } else {
            current = entry->parent->parent->left;
            if (current->color == RED) {
                entry->parent->color = BLACK;
                current->color = BLACK;
                entry->parent->parent->color = RED;
                entry = entry->parent->parent;
            } else {
                if (entry == entry->parent->left) {
                    entry = entry->parent;
                    rotate_right(tree_map, entry);
                }
                entry->parent->color = BLACK;
                entry->parent->parent->color = RED;
                rotate_left(tree_map, entry->parent->parent);
            }
        }
    }
    tree_map->root->color = BLACK;
}

static void rebalance_after_delete(TreeMap* tree_map, Entry* entry) {
    Entry* current;
    while (entry != tree_map->root && entry->color == BLACK) {
        if (entry == entry->parent->left) {
            current = entry->parent->right;
            if (current->color == RED) {
                current->color = BLACK;
                entry->parent->color = RED;
                rotate_left(tree_map, entry->parent);
                current = entry->parent->right;
            }
            if (current->left->color == BLACK && current->right->color == BLACK) {
                current->color = RED;
                entry = entry->parent;
            } else {
                if (current->right->color == BLACK) {
                    current->left->color = BLACK;
                    current->color = RED;
                    rotate_right(tree_map, current);
                    current = entry->parent->right;
                }
                current->color = entry->parent->color;
                entry->parent->color = BLACK;
                current->right->color = BLACK;
                rotate_left(tree_map, entry->parent);
                entry = tree_map->root;
            }
        } else {
            current = entry->parent->left;
            if (current->color == RED) {
                current->color = BLACK;
                entry->parent->color = RED;
                rotate_right(tree_map, entry->parent);
                current = entry->parent->left;
            }
            if (current->right->color == BLACK && current->left->color == BLACK) {
                current->color = RED;
                entry = entry->parent;
            } else {
                if (current->left->color == BLACK) {
                    current->right->color = BLACK;
                    current->color = RED;
                    rotate_left(tree_map, current);
                    current = entry->parent->left;
                }
                current->color = entry->parent->color;
                entry->parent->color = BLACK;
                current->left->color = BLACK;
                rotate_right(tree_map, entry->parent);
                entry = tree_map->root;
            }
        }
    }
    entry->color = BLACK;
}

static void rotate_left(TreeMap* tree_map, Entry* entry) {
    Entry* current = entry->right;
    entry->right = current->left;
    if (current->left != tree_map->sentinel) {
        current->left->parent = entry;
    }
    current->parent = entry->parent;
    if (entry->parent == tree_map->sentinel) {
        tree_map->root = current;
    } else if (entry == entry->parent->left) {
        entry->parent->left = current;
    } else {
        entry->parent->right = current;
    }
    current->left = entry;
    entry->parent = current;
}

static void rotate_right(TreeMap* tree_map, Entry* entry) {
    Entry* current = entry->left;
    entry->left = current->right;
    if (current->right != tree_map->sentinel) {
        current->right->parent = entry;
    }
    current->parent = entry->parent;
    if (entry->parent == tree_map->sentinel) {
        tree_map->root = current;
    } else if (entry == entry->parent->right) {
        entry->parent->right = current;
    } else {
        entry->parent->left = current;
    }
    current->right = entry;
    entry->parent = current;
}

typedef struct {
    Iterator iterator;
    TreeMap* tree_map;
    Entry* entry;
    int count;
    bool last_returned;
    bool last_removed;
    int modification_count;
} IterationContext;

static Iterator* create_iterator(const TreeMap* tree_map, void* next_function(void*), void* previous_function(void*)) {
    IterationContext* iteration_context = tree_map->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator.iteration_context = iteration_context;
    iteration_context->iterator.has_next = iterator_has_next_internal;
    iteration_context->iterator.next = next_function;
    iteration_context->iterator.has_previous = iterator_has_previous_internal;
    iteration_context->iterator.previous = previous_function;

    iteration_context->iterator.add = iterator_add_internal;
    iteration_context->iterator.set = iterator_set_internal;
    iteration_context->iterator.remove = iterator_remove_internal;
    iteration_context->iterator.reset = iterator_reset_internal;
    iteration_context->iterator.memory_free = tree_map->memory_free;

    iteration_context->tree_map = (TreeMap*) tree_map;
    iteration_context->entry = nullptr;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = tree_map->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count < iteration_context->tree_map->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_map->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    if (!iteration_context->entry) {
        iteration_context->entry = get_lower_entry(iteration_context->tree_map, iteration_context->tree_map->root);
    } else if (!iteration_context->last_removed)  {
        iteration_context->entry = get_successor_entry(iteration_context->tree_map, iteration_context->entry);
    }
    iteration_context->last_returned = true;
    iteration_context->last_removed = false;
    iteration_context->count++;
    return &iteration_context->entry->view;
}

static void* iterator_next_key_internal(void* raw_iteration_context) {
    const MapEntry* entry = iterator_next_internal(raw_iteration_context);
    return entry ? entry->key : nullptr;
}

static void* iterator_next_value_internal(void* raw_iteration_context) {
    const MapEntry* entry = iterator_next_internal(raw_iteration_context);
    return entry ? entry->value : nullptr;
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count > 0;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_map->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_previous_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    MapEntry* entry_view = nullptr;
    if (!iteration_context->entry) {
        iteration_context->entry = get_higher_entry(iteration_context->tree_map, iteration_context->tree_map->root);
        entry_view = &iteration_context->entry->view;
    } else if (!iteration_context->last_removed) {
        entry_view = &iteration_context->entry->view;
        iteration_context->entry = get_predecessor_entry(iteration_context->tree_map, iteration_context->entry);
    }
    iteration_context->last_returned = true;
    iteration_context->last_removed = false;
    iteration_context->count--;
    return entry_view;
}

static void* iterator_previous_key_internal(void* raw_iteration_context) {
    const MapEntry* entry = iterator_previous_internal(raw_iteration_context);
    return entry ? entry->key : nullptr;
}

static void* iterator_previous_value_internal(void* raw_iteration_context) {
    const MapEntry* entry = iterator_previous_internal(raw_iteration_context);
    return entry ? entry->value : nullptr;
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "tree map iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "tree map iterators doesn't support setting elements");
}

static void iterator_remove_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_map->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    if (!iteration_context->last_returned) {
        set_error(ILLEGAL_STATE_ERROR, "remove() called twice or before any next() or previous() call");
        return;
    }
    Entry* successor = get_successor_entry(iteration_context->tree_map, iteration_context->entry);
    tree_map_remove(iteration_context->tree_map, iteration_context->entry->key);
    iteration_context->entry = successor;
    iteration_context->last_returned = false;
    iteration_context->last_removed = true;
    iteration_context->count--;
    iteration_context->modification_count = iteration_context->tree_map->modification_count;
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->entry = nullptr;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = iteration_context->tree_map->modification_count;
}

static int collection_size_internal(const void* tree_map) {
    return tree_map_size(tree_map);
}

static Iterator* entry_collection_iterator_internal(const void* tree_map) {
    return tree_map_iterator(tree_map);
}

static Iterator* key_collection_iterator_internal(const void* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    Iterator* iterator = create_iterator(tree_map, iterator_next_key_internal, iterator_previous_key_internal);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

static Iterator* value_collection_iterator_internal(const void* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    Iterator* iterator = create_iterator(tree_map, iterator_next_value_internal, iterator_previous_value_internal);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

static bool entry_collection_contains_internal(const void* tree_map, const void* entry) {
    return tree_map_contains_entry(tree_map, ((MapEntry*) entry)->key, ((MapEntry*) entry)->value);
}

static bool key_collection_contains_internal(const void* tree_map, const void* key) {
    return tree_map_contains_key(tree_map, key);
}

static bool value_collection_contains_internal(const void* tree_map, const void* value) {
    return tree_map_contains_value(tree_map, value);
}