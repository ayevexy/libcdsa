#include "tree_set.h"

#include "sets.h"
#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

typedef enum {
    RED, BLACK
} Color;

typedef struct Node {
    void* element;
    Color color;
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

struct TreeSet {
    Node* root;
    Node* sentinel;
    int size;
    Comparator compare;
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
    SetView view;
};

static Node* create_node(const TreeSet*, const void*);

static Node* get_node(const TreeSet*, const void*);

static void destroy_nodes(TreeSet*, Node*);

TreeSet* tree_set_new(const TreeSetOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (!options->compare || !options->destruct || !options->equals
        || !options->to_string || !options->memory_alloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    TreeSet* tree_set = options->memory_alloc(sizeof(TreeSet));
    if (!tree_set) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'tree_set'");
        return nullptr;
    }
    Node* sentinel = options->memory_alloc(sizeof(Node));
    if (!sentinel) {
        options->memory_free(tree_set);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'tree_set'");
        return nullptr;
    }
    memset(sentinel, 0, sizeof(Node));
    sentinel->color = BLACK;

    tree_set->root = sentinel;
    tree_set->sentinel = sentinel;
    tree_set->size = 0;
    tree_set->compare = options->compare;
    tree_set->destruct = options->destruct;
    tree_set->equals = options->equals;
    tree_set->to_string = options->to_string;
    tree_set->memory_alloc = options->memory_alloc;
    tree_set->memory_free = options->memory_free;
    return tree_set;
}

void tree_set_destroy(TreeSet** tree_set_pointer) {
    if (require_non_null(tree_set_pointer, *tree_set_pointer)) return;
    TreeSet* tree_set = *tree_set_pointer;
    destroy_nodes(tree_set, tree_set->root);
    tree_set->memory_free(tree_set->sentinel);
    tree_set->memory_free(tree_set);
    *tree_set_pointer = nullptr;
}

void tree_set_set_destructor(TreeSet* tree_set, void(*destructor)(void*)) {
    if (require_non_null(tree_set, destructor)) return;
    tree_set->destruct = destructor;
}

bool tree_set_add(TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return false;
    Node* current = tree_set->root, * previous = tree_set->sentinel;
    while (current != tree_set->sentinel) {
        const int result = tree_set->compare(element, current->element);
        previous = current;
        if (result < 0) {
            current = current->left;
        } else if (result > 0) {
            current = current->right;
        } else {
            return false;
        }
    }
    current = previous;
    Node* node = create_node(tree_set, element);
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'node'");
        return false;
    }
    node->parent = current;
    if (current == tree_set->sentinel) {
        node->color = BLACK;
        tree_set->root = node;
    } else {
        node->color = RED;
        if (tree_set->compare(element, current->element) < 0) {
            current->left = node;
        } else {
            current->right = node;
        }
        //rebalance_after_insert(tree_set, node);
    }
    tree_set->size++;
    tree_set->modification_count++;
    return true;
}

int tree_set_size(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return 0;
    return tree_set->size;
}

bool tree_set_is_empty(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return false;
    return tree_set->size == 0;
}

bool tree_set_contains(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return false;
    return get_node(tree_set, element) != nullptr;
}

static Node* create_node(const TreeSet* tree_set, const void* element) {
    Node* node = tree_set->memory_alloc(sizeof(Node));
    if (!node) {
        return nullptr;
    }
    node->element = (void*) element;
    node->parent = tree_set->sentinel;
    node->left = tree_set->sentinel;
    node->right = tree_set->sentinel;
    return node;
}

static Node* get_node(const TreeSet* tree_set, const void* element) {
    Node* current = tree_set->root;
    while (current != tree_set->sentinel) {
        const int result = tree_set->compare(element, current->element);
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

static void destroy_nodes(TreeSet* tree_set, Node* node) {
    if (node == tree_set->sentinel) {
        return;
    }
    destroy_nodes(tree_set, node->left);
    destroy_nodes(tree_set, node->right);
    tree_set->destruct(node->element);
    tree_set->memory_free(node);
}