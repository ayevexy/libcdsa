#include "linked_list.h"

#include "util/error.h"

typedef struct Node {
    void* element;
    struct Node* next;
    struct Node* prev;
} Node;

struct LinkedList {
    Node* head;
    Node* tail;
    int size;
    bool (*equals)(const void*, const void*);
    int (*to_string)(const void*, char*, size_t);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
};

static Node* create_node(const LinkedList*, void*);

static Node* get_node(const LinkedList*, int);

typedef struct IterationContext IterationContext;

static Iterator* iterator(const LinkedList*);

static bool has_next(const IterationContext*);

static void* next(IterationContext*);

static void reset(IterationContext*);

LinkedList* linked_list_new(const LinkedListOptions* options) {
    if (!options->equals || !options->to_string || !options->memory_alloc || !options->memory_realloc || !options->memory_free) {
        set_error(INVALID_ARGUMENTS_ERROR, "Error at %s(): invalid argument(s)", __func__);
        return nullptr;
    }
    LinkedList* linked_list = options->memory_alloc(sizeof(LinkedList));
    if (!linked_list) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at %s(): memory allocation failed", __func__);
        return nullptr;
    }
    linked_list->head = nullptr;
    linked_list->tail = nullptr;
    linked_list->size = 0;
    linked_list->equals = options->equals;
    linked_list->to_string = options->to_string;
    linked_list->memory_alloc = options->memory_alloc;
    linked_list->memory_realloc = options->memory_realloc;
    linked_list->memory_free = options->memory_free;
    return linked_list;
}

void linked_list_delete(LinkedList** linked_list_pointer) {
    if (!*linked_list_pointer) {
        set_error(NULL_POINTER_ERROR, "Error at %s(): null pointer", __func__);
        return;
    }
    LinkedList* linked_list = *linked_list_pointer;

    Node* current = linked_list->head;
    while (current) {
        Node* temporary = current;
        current = current->next;
        linked_list->memory_free(temporary);
    }

    linked_list->memory_free(linked_list);
    *linked_list_pointer = nullptr;
}

void linked_list_destroy(LinkedList** linked_list_pointer, void (*delete)(void*)) {
    if (!*linked_list_pointer) {
        set_error(NULL_POINTER_ERROR, "Error at %s(): null pointer", __func__);
        return;
    }
    LinkedList* linked_list = *linked_list_pointer;

    Node* current = linked_list->head;
    while (current) {
        delete(current->element);
        Node* temp = current;

        current = current->next;
        linked_list->memory_free(temp);
    }

    linked_list->memory_free(linked_list);
    *linked_list_pointer = nullptr;
}

bool linked_list_add(LinkedList* linked_list, int index, const void* element) {
    if (index < 0 || index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return false;
    }

    if (index == 0) {
        return linked_list_add_first(linked_list, element);
    }
    if (index == linked_list->size) {
        return linked_list_add_last(linked_list, element);
    }

    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) return false;

    Node* node = get_node(linked_list, index);

    new_node->prev = node->prev;
    new_node->prev->next = new_node;
    new_node->next = node;
    node->prev = new_node;

    linked_list->size++;

    return true;
}

bool linked_list_add_first(LinkedList* linked_list, const void* element) {
    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) return false;

    if (linked_list->size == 0) {
        linked_list->head = new_node;
        linked_list->tail = new_node;
    } else {
        new_node->next = linked_list->head;
        linked_list->head->prev = new_node;
        linked_list->head = new_node;
    }
    linked_list->size++;

    return true;
}

bool linked_list_add_last(LinkedList* linked_list, const void* element) {
    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) return false;

    if (linked_list->size == 0) {
        linked_list->head = new_node;
        linked_list->tail = new_node;
    } else {
        new_node->prev = linked_list->tail;
        linked_list->tail->next = new_node;
        linked_list->tail = new_node;
    }
    linked_list->size++;

    return true;
}

void* linked_list_get(const LinkedList* linked_list, int index) {
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return nullptr;
    }
    return get_node(linked_list, index)->element;
}

void* linked_list_get_first(const LinkedList* linked_list) {
    if (!linked_list->head) {
        set_error(NO_SUCH_ELEMENT_ERROR, "Error at %s(): no such element", __func__);
        return nullptr;
    }
    return linked_list->head->element;
}

void* linked_list_get_last(const LinkedList* linked_list) {
    if (!linked_list->tail) {
        set_error(NO_SUCH_ELEMENT_ERROR, "Error at %s(): no such element", __func__);
        return nullptr;
    }
    return linked_list->tail->element;
}

int linked_list_size(const LinkedList* linked_list) {
    return linked_list->size;
}

Iterator* linked_list_iterator(const LinkedList* linked_list) {
    return iterator(linked_list);
}

static Node* create_node(const LinkedList* linked_list, void* element) {
    Node* node = linked_list->memory_alloc(sizeof(Node));
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at linked_list %s(): memory allocation failed", __func__);
        return nullptr;
    }
    node->element = element;
    node->next = nullptr;
    node->prev = nullptr;
    return node;
}

static Node* get_node(const LinkedList* linked_list, int index) {
    Node* node;
    if (index < linked_list->size / 2) {
        node = linked_list->head;
        for (int i = 0; i < index; i++) {
            node = node->next;
        }
    } else {
        node = linked_list->tail;
        for (int i = linked_list->size - 1; i > index; i--) {
            node = node->prev;
        }
    }
    return node;
}

struct IterationContext {
    Node* head;
    Node* current;
};

static Iterator* iterator(const LinkedList* linked_list) {
    IterationContext* iteration_context = linked_list->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at linked_list %s(): memory allocation failed", __func__);
        return nullptr;
    }
    iteration_context->head = linked_list->head;
    iteration_context->current = linked_list->head;

    Iterator* iterator = iterator_from(linked_list, iteration_context, has_next, next, reset);

    if (!iterator) {
        linked_list->memory_free(iteration_context);
        set_error(MEMORY_ALLOCATION_ERROR, "Error at linked_list %s(): memory allocation failed", __func__);
        return nullptr;
    }

    return iterator;
}

static bool has_next(const IterationContext* iteration_context) {
    return iteration_context->current;
}

static void* next(IterationContext* iteration_context) {
    if (has_next(iteration_context)) {
        void* element = iteration_context->current->element;
        iteration_context->current = iteration_context->current->next;
        return element;
    }
    return nullptr;
}

static void reset(IterationContext* iteration_context) {
    iteration_context->current = iteration_context->head;
}