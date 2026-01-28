#include "linked_list.h"

#include "util/errors.h"
#include "util/constraints.h"
#include "util/pair.h"
#include <string.h>

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

static size_t calculate_string_size(const LinkedList*);

static Node* create_node(const LinkedList*, void*);

static Node* get_node(const LinkedList*, int);

static Node* find_node(const LinkedList*, const void*);

static void* remove_node(LinkedList*, Node*);

static Pair segment_from(LinkedList*, Collection);

typedef struct IterationContext IterationContext;

static Iterator* create_iterator(const LinkedList*);

static bool has_next(const IterationContext*);

static void* next(IterationContext*);

static void reset(IterationContext*);

static void bubble_sort(LinkedList*, Comparator);

static void selection_sort(LinkedList*, Comparator);

static void insertion_sort(LinkedList*, Comparator);

static void merge_sort(LinkedList*, Comparator);

static Node* merge_sort_internal(Node*, Comparator);

static Node* split(Node*);

static Node* merge(Node*, Node*, Comparator);

static void quick_sort(LinkedList*, Comparator);

static void quick_sort_internal(Node*, Node*, Comparator);

static Node* partition(Node*, Node*, Comparator);

static void durstenfeld_shuffle(LinkedList*, int (*random)(void));

static void sattolo_shuffle(LinkedList*, int (*random)(void));

static void naive_shuffle(LinkedList*, int (*random)(void));

static void array_to_linked_list(void**, LinkedList*);

static void swap(void**, void**);

LinkedList* linked_list_new(const LinkedListOptions* options) {
    require_non_null(options);
    if (!options->equals || !options->to_string || !options->memory_alloc || !options->memory_realloc || !options->memory_free) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    LinkedList* linked_list = options->memory_alloc(sizeof(LinkedList));
    if (!linked_list) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'linked_list'");
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
    require_non_null(options);
    LinkedList* linked_list; Error error;

    if ((error = attempt(linked_list = linked_list_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }

    if ((error = attempt(linked_list_add_all_last(linked_list, collection)))) {
        linked_list_delete(&linked_list);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }

    return linked_list;
}

void linked_list_delete(LinkedList** linked_list_pointer) {
    require_non_null(linked_list_pointer);
    if (!*linked_list_pointer) {
        set_error(NULL_POINTER_ERROR, "'linked_list' must not be null");
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
    require_non_null(linked_list_pointer, delete);
    if (!*linked_list_pointer) {
        set_error(NULL_POINTER_ERROR, "'array_list' must not be null");
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

void linked_list_add(LinkedList* linked_list, int index, const void* element) {
    require_non_null(linked_list);
    if (index < 0 || index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return;
    }

    if (index == 0) {
        linked_list_add_first(linked_list, element);
        return;
    }
    if (index == linked_list->size) {
        linked_list_add_last(linked_list, element);
        return;
    }

    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new_node'");
        return;
    }

    Node* node = get_node(linked_list, index);

    new_node->prev = node->prev;
    new_node->prev->next = new_node;
    new_node->next = node;
    node->prev = new_node;

    linked_list->size++;
}

void linked_list_add_first(LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new_node'");
        return;
    }
    if (linked_list->size == 0) {
        linked_list->head = new_node;
        linked_list->tail = new_node;
    } else {
        new_node->next = linked_list->head;
        linked_list->head->prev = new_node;
        linked_list->head = new_node;
    }
    linked_list->size++;
}

void linked_list_add_last(LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    Node* new_node = create_node(linked_list, (void*) element);
    if (!new_node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new_node'");
        return;
    }
    if (linked_list->size == 0) {
        linked_list->head = new_node;
        linked_list->tail = new_node;
    } else {
        new_node->prev = linked_list->tail;
        linked_list->tail->next = new_node;
        linked_list->tail = new_node;
    }
    linked_list->size++;
}

void linked_list_add_all(LinkedList* linked_list, int index, Collection collection) {
    require_non_null(linked_list);
    if (index < 0 || index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return;
    }

    if (collection_size(collection) == 0) return;

    const Pair segment = segment_from(linked_list, collection);

    Node* head = segment.first, * tail = segment.second;
    if (!(head && tail)) return;

    if (linked_list->size == 0) {
        linked_list->head = head;
        linked_list->tail = tail;
    } else if (index == 0) {
        tail->next = linked_list->head;
        linked_list->head->prev = head;
        linked_list->head = head;
    } else if (index == linked_list->size) {
        head->prev = linked_list->tail;
        linked_list->tail->next = head;
        linked_list->tail = tail;
    } else {
        Node* node = get_node(linked_list, index);
        head->prev = node->prev;
        head->prev->next = head;
        tail->next = node;
        node->prev = tail;
    }
    linked_list->size += collection_size(collection);
}

void linked_list_add_all_first(LinkedList* linked_list, Collection collection) {
    linked_list_add_all(linked_list, 0, collection);
}

void linked_list_add_all_last(LinkedList* linked_list, Collection collection) {
    linked_list_add_all(linked_list, linked_list->size, collection);
}

void* linked_list_get(const LinkedList* linked_list, int index) {
    require_non_null(linked_list);
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    return get_node(linked_list, index)->element;
}

void* linked_list_get_first(const LinkedList* linked_list) {
    require_non_null(linked_list);
    if (!linked_list->head) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return linked_list->head->element;
}

void* linked_list_get_last(const LinkedList* linked_list) {
    require_non_null(linked_list);
    if (!linked_list->tail) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return linked_list->tail->element;
}

void* linked_list_set(LinkedList* linked_list, int index, const void* element) {
    require_non_null(linked_list);
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    void* old_element = node->element;
    node->element = (void*) element;
    return old_element;
}

void linked_list_swap(LinkedList* linked_list, int index_a, int index_b) {
    require_non_null(linked_list);
    if (index_a < 0 || index_a >= linked_list->size || index_b < 0 || index_b >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index_a = %d, index_b = %d, size = %d", index_a, index_b, linked_list->size);
        return;
    }
    swap(&get_node(linked_list, index_a)->element, &get_node(linked_list, index_b)->element);
}

void* linked_list_remove(LinkedList* linked_list, int index) {
    require_non_null(linked_list);
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    return remove_node(linked_list, node);
}

void* linked_list_remove_first(LinkedList* linked_list) {
    require_non_null(linked_list);
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return remove_node(linked_list, linked_list->head);
}

void* linked_list_remove_last(LinkedList* linked_list) {
    require_non_null(linked_list);
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return remove_node(linked_list, linked_list->tail);
}

bool linked_list_remove_element(LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    Node* node = find_node(linked_list, element);
    if (node) {
        remove_node(linked_list, node);
        return true;
    }
    return false;
}

int linked_list_remove_all(LinkedList* linked_list, Collection collection) {
    require_non_null(linked_list);


    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
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
    require_non_null(linked_list);
    if (start_index < 0 || end_index > linked_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "start_index = %d, end_index = %d, size = %d", start_index, end_index, linked_list->size);
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

int linked_list_remove_if(LinkedList* linked_list, Predicate condition) {
    require_non_null(linked_list, condition);
    int count = 0;
    for (Node* node = linked_list->head, * next; node; node = next) {
        next = node->next;
        if (condition(node->element)) {
            remove_node(linked_list, node);
            count++;
        }
    }
    return count;
}

void linked_list_replace_all(LinkedList* linked_list, Operator operator) {
    require_non_null(linked_list, operator);
    for (Node* node = linked_list->head; node; node = node->next) {
        node->element = operator(node->element);
    }
}

int linked_list_retain_all(LinkedList* linked_list, Collection collection) {
    require_non_null(linked_list);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return 0;
    }

    int count = 0;

    for (Node* node = linked_list->head, * next; node; node = next) {
        bool found = false;
        next = node->next;

        while (iterator_has_next(iterator)) {
            if (linked_list->equals(node->element, iterator_next(iterator))) {
                found = true;
                break;
            }
        }
        if (!found) {
            remove_node(linked_list, node);
            count++;
        }
        iterator_reset(iterator);
    }
    iterator_delete(&iterator);

    return count;
}

int linked_list_size(const LinkedList* linked_list) {
    require_non_null(linked_list);
    return linked_list->size;
}

bool linked_list_is_empty(const LinkedList* linked_list) {
    require_non_null(linked_list);
    return linked_list->size == 0;
}

Iterator* linked_list_iterator(const LinkedList* linked_list) {
    require_non_null(linked_list);
    Iterator* iterator = create_iterator(linked_list);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool linked_list_equals(const LinkedList* linked_list, const LinkedList* other_linked_list) {
    require_non_null(linked_list, other_linked_list);
    if (linked_list == other_linked_list) {
        return true;
    }
    if (linked_list->size != other_linked_list->size) {
        return false;
    }
    const Node* node = linked_list->head, * other_node = other_linked_list->head;
    while (node) {
        if (!linked_list->equals(node->element, other_node->element)) {
            return false;
        }
        node = node->next;
        other_node = other_node->next;
    }
    return true;
}

void linked_list_for_each(LinkedList* linked_list, Consumer action) {
    require_non_null(linked_list, action);
    for (const Node* node = linked_list->head; node; node = node->next) {
        action(node->element);
    }
}

void linked_list_sort(LinkedList* linked_list, Comparator comparator, SortingAlgorithm algorithm) {
    require_non_null(linked_list, comparator);
    if (linked_list->size < 2) {
        return;
    }
    switch (algorithm) {
        case BUBBLE_SORT: { bubble_sort(linked_list, comparator); return; }
        case SELECTION_SORT: { selection_sort(linked_list, comparator); return; }
        case INSERTION_SORT: { insertion_sort(linked_list, comparator); return; }
        case MERGE_SORT: { merge_sort(linked_list, comparator); return; }
        case QUICK_SORT: { quick_sort(linked_list, comparator); }
    }
}

void linked_list_shuffle(LinkedList* linked_list, int (*random)(void), ShufflingAlgorithm algorithm) {
    require_non_null(linked_list, random);
    switch (algorithm) {
        case DURSTENFELD_SHUFFLE: { durstenfeld_shuffle(linked_list, random); return; }
        case SATTOLO_SHUFFLE: { sattolo_shuffle(linked_list, random); return; }
        case NAIVE_SHUFFLE: { naive_shuffle(linked_list, random); }
    }
}

void linked_list_reverse(LinkedList* linked_list) {
    require_non_null(linked_list);
    Node* first = linked_list->head, * second = linked_list->tail;
    while (first != second && first->prev != second) {
        swap(&first->element, &second->element);
        first = first->next;
        second = second->prev;
    }
}

void linked_list_rotate(LinkedList* linked_list, int distance) {
    require_non_null(linked_list);
    if (linked_list->size <= 1) return;

    distance %= linked_list->size;
    if (distance < 0) {
        distance += linked_list->size;
    }
    if (distance == 0) return;

    linked_list->tail->next = linked_list->head;
    linked_list->head->prev = linked_list->tail;

    Node* new_tail = linked_list->head;
    for (int i = 0; i < linked_list->size - distance - 1; i++) {
        new_tail = new_tail->next;
    }
    Node* new_head = new_tail->next;

    new_tail->next = nullptr;
    new_head->prev = nullptr;

    linked_list->head = new_head;
    linked_list->tail = new_tail;
}

void linked_list_clear(LinkedList* linked_list) {
    require_non_null(linked_list);
    Node* current = linked_list->head;
    while (current) {
        Node* temporary = current;
        current = current->next;
        linked_list->memory_free(temporary);
    }
    linked_list->head = linked_list->tail = nullptr;
    linked_list->size = 0;
}

void linked_list_clear_data(LinkedList* linked_list, void (*delete)(void*)) {
    require_non_null(linked_list, delete);
    Node* current = linked_list->head;
    while (current) {
        delete(current->element);
        Node* temporary = current;

        current = current->next;
        linked_list->memory_free(temporary);
    }
    linked_list->head = linked_list->tail = nullptr;
    linked_list->size = 0;
}

Optional linked_list_find(const LinkedList* linked_list, Predicate condition) {
    require_non_null(linked_list, condition);
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (condition(node->element)) {
            return optional_of(node->element);
        }
    }
    return optional_empty();
}

Optional linked_list_find_last(const LinkedList* linked_list, Predicate condition) {
    require_non_null(linked_list, condition);
    void* element = nullptr;
    bool found = false;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (condition(node->element)) {
            found = true;
            element = node->element;
        }
    }
    return found ? optional_of(element) : optional_empty();
}

int linked_list_index_where(const LinkedList* linked_list, Predicate condition) {
    require_non_null(linked_list, condition);
    int index = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (condition(node->element)) {
            return index;
        }
        index++;
    }
    return -1;
}

int linked_list_last_index_where(const LinkedList* linked_list, Predicate condition) {
    require_non_null(linked_list, condition);
    int index = -1, count = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (condition(node->element)) {
            index = count;
        }
        count++;
    }
    return index;
}

bool linked_list_contains(const LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    return linked_list_index_of(linked_list, element) != -1;
}

bool linked_list_contains_all(const LinkedList* linked_list, Collection collection) {
    require_non_null(linked_list);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!linked_list_contains(linked_list, element)) {
            contains = false;
            break;
        }
    }
    iterator_delete(&iterator);
    return contains;
}

int linked_list_occurrences_of(const LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    int count = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (linked_list->equals(node->element, element)) {
            count++;
        }
    }
    return count;
}

int linked_list_index_of(const LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    int index = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (linked_list->equals(node->element, element)) {
            return index;
        }
        index++;
    }
    return -1;
}

int linked_list_last_index_of(const LinkedList* linked_list, const void* element) {
    require_non_null(linked_list);
    int index = -1, count = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (linked_list->equals(node->element, element)) {
            index = count;
        }
        count++;
    }
    return index;
}

LinkedList* linked_list_clone(const LinkedList* linked_list) {
    require_non_null(linked_list);
    LinkedList* new_linked_list; const Error error = attempt(new_linked_list = linked_list_sub_list(linked_list, 0, linked_list->size));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return new_linked_list;
}

// TODO: handle linked_list_add_last failure
LinkedList* linked_list_sub_list(const LinkedList* linked_list, int start_index, int end_index) {
    require_non_null(linked_list);
    if (start_index < 0 || end_index > linked_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "start_index = %d, end_index = %d, size = %d", start_index, end_index, linked_list->size);
        return nullptr;
    }
    LinkedList* new_linked_list; const Error error = attempt(new_linked_list = linked_list_new(&(LinkedListOptions) {
        .equals = linked_list->equals,
        .to_string = linked_list->to_string,
        .memory_alloc = linked_list->memory_alloc,
        .memory_realloc = linked_list->memory_realloc,
        .memory_free = linked_list->memory_free
    }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    const Node* node = get_node(linked_list, start_index);
    for (int i = start_index; i < end_index; i++) {
        linked_list_add_last(new_linked_list, node->element);
        node = node->next;
    }
    return new_linked_list;
}

Collection linked_list_to_collection(const LinkedList* linked_list) {
    require_non_null(linked_list);
    return collection_from(linked_list);
}

void** linked_list_to_array(const LinkedList* linked_list) {
    require_non_null(linked_list);
    void** elements = linked_list->memory_alloc(sizeof(void*) * linked_list->size);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "");
        return nullptr;
    }
    const Node* node = linked_list->head;
    for (int i = 0; i < linked_list->size; i++) {
        elements[i] = node->element;
        node = node->next;
    }
    return elements;
}

char* linked_list_to_string(const LinkedList* linked_list) {
    require_non_null(linked_list);
    char* string = linked_list->memory_alloc(calculate_string_size(linked_list));

    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "");
        return nullptr;
    }

    string[0] = '\0'; // initialize string to clear trash data

    strcat(string, linked_list->size == 0 ? "{" : "{ ");
    for (const Node* node = linked_list->head; node; node = node->next) {
        constexpr int NULL_TERMINATOR = 1;

        const int length = linked_list->to_string(node->element, nullptr, 0) + NULL_TERMINATOR;
        char* element_string = linked_list->memory_alloc(length);

        if (!element_string) {
            linked_list->memory_free(string);
            set_error(MEMORY_ALLOCATION_ERROR, "");
            return nullptr;
        }

        linked_list->to_string(node->element, element_string, length);
        strcat(string, element_string);

        if (node->next) {
            strcat(string, " -> ");
        }
        linked_list->memory_free(element_string);
    }
    strcat(string, linked_list->size == 0 ? "}" : " }");

    return string;
}

static size_t calculate_string_size(const LinkedList* linked_list) {
    constexpr int BRACES = 2; constexpr int SEPARATOR = 4; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (const Node* node = linked_list->head; node; node = node->next) {
        length += linked_list->to_string(node->element, nullptr, 0);

        if (node == linked_list->head) length += 1; // space after opening brace
        if (node != linked_list->tail) length += SEPARATOR; // prevent separator on the last element
        if (node == linked_list->tail) length += 1; // space before closing brace
    }
    return length + BRACES + NULL_TERMINATOR;
}

static Node* create_node(const LinkedList* linked_list, void* element) {
    Node* node = linked_list->memory_alloc(sizeof(Node));
    if (!node) {
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

static Pair segment_from(LinkedList* linked_list, Collection collection) {
    assert(collection_size(collection) != 0);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return (Pair) {};
    }

    Node* head = nullptr, * tail = nullptr;
    bool failed = false;
    while (iterator_has_next(iterator)) {
        Node* node = create_node(linked_list, iterator_next(iterator));
        if (!node) {
            failed = true;
            break;
        }
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = tail->next;
        }
    }
    iterator_delete(&iterator);

    if (failed) {
        Node* node = head;
        while (node) {
            Node* temp = node;
            node = node->next;
            linked_list->memory_free(temp);
        }
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for new nodes");
        return (Pair) {};
    }

    return (Pair) { .first = head, .second = tail };
}

struct IterationContext {
    Node* head;
    Node* current;
};

static Iterator* create_iterator(const LinkedList* linked_list) {
    IterationContext* iteration_context = linked_list->memory_alloc(sizeof(IterationContext));
    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->head = linked_list->head;
    iteration_context->current = linked_list->head;

    Iterator* iterator = iterator_from(linked_list, iteration_context, has_next, next, reset);
    if (!iterator) {
        linked_list->memory_free(iteration_context);
        return nullptr;
    }
    return iterator;
}

static bool has_next(const IterationContext* iteration_context) {
    return iteration_context->current;
}

static void* next(IterationContext* iteration_context) {
    if (!has_next(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "");
        return nullptr;
    }
    void* element = iteration_context->current->element;
    iteration_context->current = iteration_context->current->next;
    return element;
}

static void reset(IterationContext* iteration_context) {
    iteration_context->current = iteration_context->head;
}

static void bubble_sort(LinkedList* linked_list, Comparator compare) {
    for (int i = 0; i < linked_list->size - 1; i++) {
        Node* node = linked_list->head;
        for (int j = 0; j < linked_list->size - i - 1; j++) {
            Node* next = node->next;
            if (compare(node->element, next->element) > 0) {
                swap(&node->element, &next->element);
            }
            node = next;
        }
    }
}

static void selection_sort(LinkedList* linked_list, Comparator compare) {
    Node* node = linked_list->head;
    for (int i = 0; i < linked_list->size - 1; i++) {
        Node* min_node = node, * next = node->next;
        for (int j = i + 1; j < linked_list->size; j++) {
            if (compare(next->element, min_node->element) < 0) {
                min_node = next;
            }
            next = next->next;
        }
        swap(&min_node->element, &node->element);
        node = node->next;
    }
}

static void insertion_sort(LinkedList* linked_list, Comparator compare) {
    const Node* node = linked_list->head->next;
    while (node) {
        void* element = node->element;
        const Node* prev = node->prev;

        while (prev && compare(prev->element, element) > 0) {
            prev->next->element = prev->element;
            prev = prev->prev;
        }

        if (prev) prev->next->element = element;
        else linked_list->head->element = element;

        node = node->next;
    }
}

static void merge_sort(LinkedList* linked_list, Comparator compare) {
    linked_list->head = merge_sort_internal(linked_list->head, compare);

    Node* node = linked_list->head;
    while (node && node->next) {
        node = node->next;
    }
    linked_list->tail = node;
}

static Node* merge_sort_internal(Node* head, Comparator compare) {
    if (!head || !head->next) {
        return head;
    }
    Node* second = split(head);

    head = merge_sort_internal(head, compare);
    second = merge_sort_internal(second, compare);

    return merge(head, second, compare);
}


static Node* split(Node* head) {
    Node* slow = head, * fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    Node* second = slow->next;
    slow->next = nullptr;
    if (second) second->prev = nullptr;

    return second;
}

static Node* merge(Node* first, Node* second, Comparator compare) {
    if (!first) return second;
    if (!second) return first;

    if (compare(first->element, second->element) <= 0) {
        first->next = merge(first->next, second, compare);
        if (first->next) first->next->prev = first;
        first->prev = nullptr;
        return first;
    } else {
        second->next = merge(first, second->next, compare);
        if (second->next) second->next->prev = second;
        second->prev = nullptr;
        return second;
    }
}

static void quick_sort(LinkedList* linked_list, Comparator compare) {
    quick_sort_internal(linked_list->head, linked_list->tail, compare);
}

static void quick_sort_internal(Node* low, Node* high, Comparator compare) {
    if (!low || !high || low == high || low == high->next) {
        return;
    }
    const Node* pivot = partition(low, high, compare);

    quick_sort_internal(low, pivot->prev, compare);
    quick_sort_internal(pivot->next, high, compare);
}

static Node* partition(Node* low, Node* high, Comparator compare) {
    Node* i = low->prev;

    for (Node* j = low; j != high; j = j->next) {
        if (compare(j->element, high->element) <= 0) {
            i = (i == nullptr) ? low : i->next;
            swap(&i->element, &j->element);
        }
    }
    i = i == nullptr ? low : i->next;
    swap(&i->element, &high->element);

    return i;
}

static void durstenfeld_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "");
        return;
    }
    for (int i = linked_list->size - 1; i > 0; i--) {
        const int j = random() % (i + 1);
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_free(elements);
}

static void sattolo_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "");
        return;
    }
    for (int i = linked_list->size - 1; i > 0; i--) {
        const int j = random() % i;
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_free(elements);
}

static void naive_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "");
        return;
    }
    for (int i = 0; i < linked_list->size; i++) {
        const int j = random() % linked_list->size;
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_free(elements);
}

static void array_to_linked_list(void** elements, LinkedList* linked_list) {
    Node* node = linked_list->head;
    for (int i = 0; i < linked_list->size; i++) {
        node->element = elements[i];
        node = node->next;
    }
}

static void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}