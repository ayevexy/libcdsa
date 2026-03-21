#include "tree_map.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

typedef enum {
    RED,
    BLACK
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

static size_t calculate_string_size(const TreeMap*);

static Entry* create_entry(const TreeMap*, const void*, const void*);

static Entry* get_entry(const TreeMap*, const void*);

static Entry* get_successor_entry(Entry*);

static Entry* get_predecessor_entry(Entry*);

static Entry* get_lower_entry(Entry*);

static Entry* get_higher_entry(Entry*);

static void remove_node(TreeMap*, Entry*);

static void transplant(TreeMap*, Entry*, Entry*);

static void destroy_entries(TreeMap*, Entry*);

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

void tree_map_put_all(TreeMap* tree_map, Collection entry_collection) {
    if (require_non_null(tree_map)) return;

    Iterator* iterator; Error error = attempt(iterator = collection_iterator(entry_collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
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
    const Entry* entry = get_lower_entry(tree_map->root);
    if (entry == &sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    return entry->view;
}

MapEntry tree_map_last_entry(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    const Entry* entry = get_higher_entry(tree_map->root);
    if (entry == &sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    return entry->view;
}

void* tree_map_first_key(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_lower_entry(tree_map->root);
    if (entry == &sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return nullptr;
    }
    return entry->key;
}

void* tree_map_last_key(const TreeMap* tree_map) {
    if (require_non_null(tree_map)) return nullptr;
    const Entry* entry = get_higher_entry(tree_map->root);
    if (entry == &sentinel) {
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
    remove_node(tree_map, entry);
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
    Entry* entry = get_lower_entry(tree_map->root);
    if (entry == &sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    const MapEntry view = entry->view;
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    remove_node(tree_map, entry);
    return view;
}

MapEntry tree_map_poll_last_entry(TreeMap* tree_map) {
    if (require_non_null(tree_map)) return (MapEntry) {};
    Entry* entry = get_higher_entry(tree_map->root);
    if (entry == &sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree map' is empty");
        return (MapEntry) {};
    }
    const MapEntry view = entry->view;
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    remove_node(tree_map, entry);
    return view;
}

void tree_map_replace_all(TreeMap* tree_map, BiOperator bi_operator) {
    if (require_non_null(tree_map, bi_operator)) return;
    Entry* entry = get_lower_entry(tree_map->root);
    while (entry != &sentinel) {
        void* old_value = entry->value;
        void* new_value = bi_operator(entry->key, entry->value);
        if (old_value != new_value) {
            tree_map->value_destruct(old_value);
        }
        entry->value = new_value;
        entry = get_successor_entry(entry);
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
    Entry * entry = get_lower_entry(tree_map->root), * other_entry = get_lower_entry(other_tree_map->root);
    while (entry != &sentinel && other_entry != &sentinel) {
        if (!(tree_map->key_equals(other_entry->key, entry->key) && tree_map->value_equals(entry->value, other_entry->value))) {
            return false;
        }
        entry = get_successor_entry(entry);
        other_entry = get_successor_entry(other_entry);
    }
    return true;
}

void tree_map_for_each(TreeMap* tree_map, BiConsumer action) {
    if (require_non_null(tree_map, action)) return;
    Entry* entry = get_lower_entry(tree_map->root);
    while (entry != &sentinel) {
        action(entry->key, entry->value);
        entry = get_successor_entry(entry);
    }
}

void tree_map_clear(TreeMap* tree_map) {
    if (require_non_null(tree_map)) return;
    destroy_entries(tree_map, tree_map->root);
    tree_map->root = &sentinel;
    tree_map->size = 0;
    tree_map->modification_count++;
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
    Entry* entry = get_lower_entry(tree_map->root);
    while (entry != &sentinel) {
        if ((error = attempt(tree_map_put(new_tree_map, entry->key, entry->value)))) {
            tree_map_destroy(&new_tree_map);
            set_error(error, "%s", plain_error_message());
            return nullptr;
        }
        entry = get_successor_entry(entry);
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
    Entry* entry = get_lower_entry(tree_map->root);
    while (entry != &sentinel) {
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
        entry = get_successor_entry(entry);
    }

    strcat(string, tree_map->size == 0 ? "]" : " ]");
    return string;
}

static size_t calculate_string_size(const TreeMap* tree_map) {
    constexpr int BRACKETS = 2; constexpr int SEPARATOR = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;
    int count = 0;

    Entry* entry = get_lower_entry(tree_map->root);
    while (entry != &sentinel) {
        length += tree_map->key_to_string(entry->key, nullptr, 0);
        length += tree_map->value_to_string(entry->value, nullptr, 0);
        length += 3; // ' = ' separator

        if (count == 0) length += 1; // space after opening bracket
        if (count < tree_map->size - 1) length += SEPARATOR; // prevent separator on the last element
        if (count == tree_map->size - 1) length += 1; // space before closing bracket

        entry = get_successor_entry(entry);
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

static Entry* get_predecessor_entry(Entry* entry) {
    if (entry->left != &sentinel) {
        return get_higher_entry(entry->left);
    }
    Entry* parent = entry->parent;
    while (parent != &sentinel && entry == parent->left) {
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

static Entry* get_higher_entry(Entry* entry) {
    if (entry == &sentinel) {
        return entry;
    }
    while (entry->right != &sentinel) {
        entry = entry->right;
    }
    return entry;
}

static void remove_node(TreeMap* tree_map, Entry* entry) {
    Entry* current = entry;
    Color current_color = current->color;
    if (entry->left == &sentinel) {
        transplant(tree_map, entry, entry->right);
    } else if (entry->right == &sentinel) {
        transplant(tree_map, entry, entry->left);
    } else {
        current = get_lower_entry(entry->right);
        current_color = current->color;
        if (current->parent == entry) {
            current->right->parent = current;
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
        // TODO: rebalance
    }
    tree_map->memory_free(entry);
    tree_map->size--;
    tree_map->modification_count++;
}

static void transplant(TreeMap* tree_map, Entry* first, Entry* second) {
    if (first->parent == &sentinel) {
        tree_map->root = second;
    } else if (first == first->parent->left) {
        first->parent->left = second;
    } else {
        first->parent->right = second;
    }
    second->parent = first->parent;
}

static void destroy_entries(TreeMap* tree_map, Entry* entry) {
    if (entry == &sentinel) {
        return;
    }
    destroy_entries(tree_map, entry->left);
    destroy_entries(tree_map, entry->right);
    tree_map->key_destruct(entry->key);
    tree_map->value_destruct(entry->value);
    tree_map->memory_free(entry);
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
        iteration_context->entry = get_lower_entry(iteration_context->tree_map->root);
    } else if (!iteration_context->last_removed)  {
        iteration_context->entry = get_successor_entry(iteration_context->entry);
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
        iteration_context->entry = get_lower_entry(iteration_context->tree_map->root);
        entry_view = &iteration_context->entry->view;
    } else if (!iteration_context->last_removed) {
        entry_view = &iteration_context->entry->view;
        iteration_context->entry = get_predecessor_entry(iteration_context->entry);
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
    set_error(UNSUPPORTED_OPERATION_ERROR, "TreeMap iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "TreeMap iterators doesn't support setting elements");
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
    Entry* right = iteration_context->entry->right;
    tree_map_remove(iteration_context->tree_map, iteration_context->entry->key);
    iteration_context->entry = right;
    iteration_context->last_returned = false;
    iteration_context->last_removed = true;
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