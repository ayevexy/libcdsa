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

static Node* find_node(const LinkedList*, const void*);

static void* remove_node(LinkedList*, Node*);

typedef struct IterationContext IterationContext;

static Iterator* iterator(const LinkedList*);

static bool has_next(const IterationContext*);

static void* next(IterationContext*);

static void reset(IterationContext*);

static void swap(void**, void**);

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

LinkedList* linked_list_from(Collection collection, const LinkedListOptions* options) {
    LinkedList* linked_list; Error error = attempt(linked_list = linked_list_new(options));

    if (error == INVALID_ARGUMENTS_ERROR) {
        set_error(error, "Error at %s(): invalid argument(s)", __func__);
        return nullptr;
    }
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "Error at %s(): memory allocation failure", __func__);
        return nullptr;
    }

    error = attempt(linked_list_add_all_last(linked_list, collection));

    if (error == MEMORY_ALLOCATION_ERROR) {
        linked_list_delete(&linked_list);
        set_error(error, "Error at %s(): %s", __func__, error_message());
        return nullptr;
    }

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

bool linked_list_add_all(LinkedList* linked_list, int index, Collection collection) {
    Iterator* iterator = collection_iterator(collection);

    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at %s(): failed to allocate memory for collection_iterator()", __func__);
        return false;
    }

    int count = 0;
    while (iterator_has_next(iterator)) {
        if (linked_list_add(linked_list, index, iterator_next(iterator))) {
            index++;
            count++;
        }
    }
    iterator_delete(&iterator);
    return count == collection_size(collection);
}

bool linked_list_add_all_first(LinkedList* linked_list, Collection collection) {
    return linked_list_add_all(linked_list, 0, collection);
}

bool linked_list_add_all_last(LinkedList* linked_list, Collection collection) {
    return linked_list_add_all(linked_list, linked_list->size, collection);
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

void* linked_list_set(LinkedList* linked_list, int index, const void* element) {
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    void* old_element = node->element;
    node->element = (void*) element;
    return old_element;
}

bool linked_list_swap(LinkedList* linked_list, int index_a, int index_b) {
    if (index_a < 0 || index_a >= linked_list->size || index_b < 0 || index_b >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return false;
    }
    swap(&get_node(linked_list, index_a)->element, &get_node(linked_list, index_b)->element);
    return true;
}

void* linked_list_remove(LinkedList* linked_list, int index) {
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    return remove_node(linked_list, node);
}

void* linked_list_remove_first(LinkedList* linked_list) {
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "Error at %s(): no such element", __func__);
        return nullptr;
    }
    return remove_node(linked_list, linked_list->head);
}

void* linked_list_remove_last(LinkedList* linked_list) {
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "Error at %s(): no such element", __func__);
        return nullptr;
    }
    return remove_node(linked_list, linked_list->tail);
}

bool linked_list_remove_element(LinkedList* linked_list, const void* element) {
    Node* node = find_node(linked_list, element);
    if (node) {
        remove_node(linked_list, node);
        return true;
    }
    return false;
}

int linked_list_remove_all(LinkedList* linked_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);

    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at %s(): failed to allocate memory for collection_iterator()", __func__);
        return 0;
    }

    int count = 0;
    while (iterator_has_next(iterator)) {
        if (linked_list_remove_element(linked_list, iterator_next(iterator))) {
            count++;
        }
    }
    iterator_delete(&iterator);
    return count;
}

int linked_list_remove_range(LinkedList* linked_list, int start_index, int end_index) {
    if (start_index < 0 || end_index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "Error at %s(): index out of bounds", __func__);
        return 0;
    }
    if (start_index > end_index) {
        set_error(INVALID_ARGUMENTS_ERROR, "Error at %s(): invalid arguments", __func__);
        return 0;
    }
    Node* node = get_node(linked_list, start_index);
    for (int i = start_index; i < end_index; i++) {
        Node* next = node->next;
        remove_node(linked_list, node);
        node = next;
    }
    return end_index - start_index;
}

int linked_list_size(const LinkedList* linked_list) {
    return linked_list->size;
}

Iterator* linked_list_iterator(const LinkedList* linked_list) {
    return iterator(linked_list);
}

Collection linked_list_to_collection(const LinkedList* linked_list) {
    return collection_from(linked_list);
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

static Node* find_node(const LinkedList* linked_list, const void* element) {
    Node* node = linked_list->head;
    while (node) {
        if (linked_list->equals(node->element, element)) {
            return node;
        }
        node = node->next;
    }
    return nullptr;
}

static void* remove_node(LinkedList* linked_list, Node* node) {
    void* element = node->element;
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (!node->prev) {
        linked_list->head = node->next;
    }
    if (!node->next) {
        linked_list->tail = node->prev;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    linked_list->memory_free(node);
    linked_list->size--;

    return element;
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

static void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}