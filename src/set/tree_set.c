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

static Node* get_successor_node(const TreeSet*, Node*);

static Node* get_predecessor_node(const TreeSet*, Node*);

static Node* get_higher_node(const TreeSet*, Node*);

static Node* get_lower_node(const TreeSet*, Node*);

static void remove_node(TreeSet*, Node*);

static void transplant(TreeSet*, Node*, Node*);

static void destroy_nodes(TreeSet*, Node*);

static Iterator* create_iterator(const TreeSet*);

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static bool iterator_has_previous_internal(const void*);

static void* iterator_previous_internal(void*);

static void iterator_add_internal(void*, const void*);

static void iterator_set_internal(void*, const void*);

static void iterator_remove_internal(void*);

static void iterator_reset_internal(void*);

static int collection_size_internal(const void*);

static Iterator* collection_iterator_internal(const void*);

static bool collection_contains_internal(const void*, const void*);

static int set_view_size_internal(const void*);

static Iterator* set_view_iterator_internal(const void*);

static bool set_view_contains_internal(const void*, const void*);

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
    tree_set->modification_count = 0;
    tree_set->view.sets.first = tree_set;
    tree_set->view.sets.second = nullptr;
    tree_set->view.size = set_view_size_internal;
    tree_set->view.iterator = set_view_iterator_internal;
    tree_set->view.contains = set_view_contains_internal;
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

void* tree_set_get_first(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return nullptr;
    const Node* node = get_lower_node(tree_set, tree_set->root);
    if (node == tree_set->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree set' is empty");
        return nullptr;
    }
    return node->element;
}

void* tree_set_get_last(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return nullptr;
    const Node* node = get_higher_node(tree_set, tree_set->root);
    if (node == tree_set->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree set' is empty");
        return nullptr;
    }
    return node->element;
}

bool tree_set_remove(TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return false;
    Node* node = get_node(tree_set, element);
    if (!node) {
        return false;
    }
    tree_set->destruct(node->element);
    remove_node(tree_set, node);
    return true;
}

void* tree_set_remove_first(TreeSet* tree_set) {
    if (require_non_null(tree_set)) return nullptr;
    Node* node = get_lower_node(tree_set, tree_set->root);
    if (node == tree_set->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree set' is empty");
        return nullptr;
    }
    void* element = node->element;
    tree_set->destruct(node->element);
    remove_node(tree_set, node);
    return element;
}

void* tree_set_remove_last(TreeSet* tree_set) {
    if (require_non_null(tree_set)) return nullptr;
    Node* node = get_higher_node(tree_set, tree_set->root);
    if (node == tree_set->sentinel) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'tree set' is empty");
        return nullptr;
    }
    void* element = node->element;
    tree_set->destruct(node->element);
    remove_node(tree_set, node);
    return element;
}

int tree_set_size(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return 0;
    return tree_set->size;
}

bool tree_set_is_empty(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return false;
    return tree_set->size == 0;
}

Iterator* tree_set_iterator(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return nullptr;
    Iterator* iterator = create_iterator(tree_set);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

void tree_set_clear(TreeSet* tree_set) {
    if (require_non_null(tree_set)) return;
    destroy_nodes(tree_set, tree_set->root);
    tree_set->root = tree_set->sentinel;
    tree_set->size = 0;
    tree_set->modification_count++;
}

void* tree_set_higher(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return nullptr;
    Node* current = get_node(tree_set, element);
    if (!current) {
        return nullptr;
    }
    const Node* node = get_successor_node(tree_set, current);
    return node != tree_set->sentinel ? node->element : nullptr;
}

void* tree_set_ceiling(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return nullptr;
    Node* current = get_node(tree_set, element);
    if (!current) {
        return nullptr;
    }
    const Node* node = get_successor_node(tree_set, current);
    return node != tree_set->sentinel ? node->element : current->element;
}

void* tree_set_floor(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return nullptr;
    Node* current = get_node(tree_set, element);
    if (!current) {
        return nullptr;
    }
    const Node* node = get_predecessor_node(tree_set, current);
    return node != tree_set->sentinel ? node->element : current->element;
}

void* tree_set_lower(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return nullptr;
    Node* current = get_node(tree_set, element);
    if (!current) {
        return nullptr;
    }
    const Node* node = get_predecessor_node(tree_set, current);
    return node != tree_set->sentinel ? node->element : nullptr;
}

bool tree_set_contains(const TreeSet* tree_set, const void* element) {
    if (require_non_null(tree_set)) return false;
    return get_node(tree_set, element) != nullptr;
}

Collection tree_set_to_collection(const TreeSet* tree_set) {
    if (require_non_null(tree_set)) return (Collection) {};
    return (Collection) {
        .data_structure = tree_set,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
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

static Node* get_successor_node(const TreeSet* tree_set, Node* node) {
    if (node->right != tree_set->sentinel) {
        return get_lower_node(tree_set, node->right);
    }
    Node* parent = node->parent;
    while (parent != tree_set->sentinel && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

static Node* get_predecessor_node(const TreeSet* tree_set, Node* node) {
    if (node->left != tree_set->sentinel) {
        return get_higher_node(tree_set, node->left);
    }
    Node* parent = node->parent;
    while (parent != tree_set->sentinel && node == parent->left) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

static Node* get_higher_node(const TreeSet* tree_set, Node* node) {
    if (node == tree_set->sentinel) {
        return node;
    }
    while (node->right != tree_set->sentinel) {
        node = node->right;
    }
    return node;
}

static Node* get_lower_node(const TreeSet* tree_set, Node* node) {
    if (node == tree_set->sentinel) {
        return node;
    }
    while (node->left != tree_set->sentinel) {
        node = node->left;
    }
    return node;
}

static void remove_node(TreeSet* tree_set, Node* node) {
    Node* current = node, * auxiliar;
    Color current_color = current->color;
    if (node->left == tree_set->sentinel) {
        auxiliar = node->right;
        transplant(tree_set, node, node->right);
    } else if (node->right == tree_set->sentinel) {
        auxiliar = node->left;
        transplant(tree_set, node, node->left);
    } else {
        current = get_lower_node(tree_set, node->right);
        current_color = current->color;
        auxiliar = current->right;
        if (current->parent == node) {
            auxiliar->parent = current;
        } else {
            transplant(tree_set, current, current->right);
            current->right = node->right;
            current->right->parent = current;
        }
        transplant(tree_set, node, current);
        current->left = node->left;
        current->left->parent = current;
        current->color = node->color;
    }
    if (current_color == BLACK) {
        // rebalance_after_delete(tree_set, auxiliar);
    }
    tree_set->memory_free(node);
    tree_set->size--;
    tree_set->modification_count++;
}

static void transplant(TreeSet* tree_set, Node* first, Node* second) {
    if (first->parent == tree_set->sentinel) {
        tree_set->root = second;
    } else if (first == first->parent->left) {
        first->parent->left = second;
    } else {
        first->parent->right = second;
    }
    second->parent = first->parent;
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

typedef struct {
    Iterator iterator;
    TreeSet* tree_set;
    Node* node;
    int count;
    bool last_returned;
    bool last_removed;
    int modification_count;
} IterationContext;

static Iterator* create_iterator(const TreeSet* tree_set) {
    IterationContext* iteration_context = tree_set->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator.iteration_context = iteration_context;
    iteration_context->iterator.has_next = iterator_has_next_internal;
    iteration_context->iterator.next = iterator_next_internal;
    iteration_context->iterator.has_previous = iterator_has_previous_internal;
    iteration_context->iterator.previous = iterator_previous_internal;

    iteration_context->iterator.add = iterator_add_internal;
    iteration_context->iterator.set = iterator_set_internal;
    iteration_context->iterator.remove = iterator_remove_internal;
    iteration_context->iterator.reset = iterator_reset_internal;
    iteration_context->iterator.memory_free = tree_set->memory_free;

    iteration_context->tree_set = (TreeSet*) tree_set;
    iteration_context->node = nullptr;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = tree_set->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count < iteration_context->tree_set->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_set->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    if (!iteration_context->node) {
        iteration_context->node = get_lower_node(iteration_context->tree_set, iteration_context->tree_set->root);
    } else if (!iteration_context->last_removed)  {
        iteration_context->node = get_successor_node(iteration_context->tree_set, iteration_context->node);
    }
    iteration_context->last_returned = true;
    iteration_context->last_removed = false;
    iteration_context->count++;
    return iteration_context->node->element;
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count > 0;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_set->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_previous_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    void* element = nullptr;
    if (!iteration_context->node) {
        iteration_context->node = get_higher_node(iteration_context->tree_set, iteration_context->tree_set->root);
        element = iteration_context->node->element;
    } else if (!iteration_context->last_removed) {
        element = iteration_context->node->element;
        iteration_context->node = get_predecessor_node(iteration_context->tree_set, iteration_context->node);
    }
    iteration_context->last_returned = true;
    iteration_context->last_removed = false;
    iteration_context->count--;
    return element;
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "TreeSet iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "TreeSet iterators doesn't support setting elements");
}

static void iterator_remove_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->tree_set->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    if (!iteration_context->last_returned) {
        set_error(ILLEGAL_STATE_ERROR, "remove() called twice or before any next() or previous() call");
        return;
    }
    Node* successor = get_successor_node(iteration_context->tree_set, iteration_context->node);
    tree_set_remove(iteration_context->tree_set, iteration_context->node->element);
    iteration_context->node = successor;
    iteration_context->last_returned = false;
    iteration_context->last_removed = true;
    iteration_context->count--;
    iteration_context->modification_count = iteration_context->tree_set->modification_count;
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->node = nullptr;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = iteration_context->tree_set->modification_count;
}

static int collection_size_internal(const void* tree_set) {
    return tree_set_size(tree_set);
}

static Iterator* collection_iterator_internal(const void* tree_set) {
    return tree_set_iterator(tree_set);
}

static bool collection_contains_internal(const void* tree_set, const void* element) {
    return tree_set_contains(tree_set, element);
}

static int set_view_size_internal(const void* tree_set) {
    return tree_set_size(((Pair*) tree_set)->first);
}

static Iterator* set_view_iterator_internal(const void* tree_set) {
    return tree_set_iterator(((Pair*) tree_set)->first);
}

static bool set_view_contains_internal(const void* tree_set, const void* element) {
    return tree_set_contains(((Pair*) tree_set)->first, element);
}

SetView* _tree_set_view(const TreeSet* tree_set) {
    return tree_set ? (SetView*) &tree_set->view : nullptr;
}