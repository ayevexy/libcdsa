#include "linked_list.h"

#include "util/errors.h"
#include "util/constraints.h"
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
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_dealloc)(void*);
    };
    int modification_count;
};

static size_t calculate_string_size(const LinkedList*);

static Node* create_node(const LinkedList*, const void*);

static Node* get_node(const LinkedList*, int);

static Node* find_node(const LinkedList*, const void*);

static void* remove_node(LinkedList*, Node*);

static void destroy_nodes(LinkedList*);

static Iterator* create_iterator(const LinkedList*, int);

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static bool iterator_has_previous_internal(const void*);

static void* iterator_previous_internal(void*);

static void iterator_add_internal(void*, const void*);

static void iterator_set_internal(void*, const void*);

static void iterator_remove_internal(void*);

static void iterator_reset_internal(void*);

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

static bool durstenfeld_shuffle(LinkedList*, int (*random)(void));

static bool sattolo_shuffle(LinkedList*, int (*random)(void));

static bool naive_shuffle(LinkedList*, int (*random)(void));

static void array_to_linked_list(void**, LinkedList*);

static void swap(void**, void**);

static int collection_size_internal(const void*);

static Iterator* collection_iterator_internal(const void*);

static bool collection_contains_internal(const void*, const void*);

LinkedList* linked_list_new(const LinkedListOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (!options->destruct || !options->equals || !options->to_string || !options->memory_alloc || !options->memory_dealloc) {
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
    linked_list->destruct = options->destruct;
    linked_list->equals = options->equals;
    linked_list->to_string = options->to_string;
    linked_list->memory_alloc = options->memory_alloc;
    linked_list->memory_dealloc = options->memory_dealloc;
    linked_list->modification_count = 0;
    return linked_list;
}

LinkedList* linked_list_from(Collection collection, const LinkedListOptions* options) {
    if (require_non_null(options)) return nullptr;
    LinkedList* linked_list; Error error;

    if ((error = attempt(linked_list = linked_list_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(linked_list_add_all_last(linked_list, collection)))) {
        linked_list_destroy(&linked_list);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return linked_list;
}

void linked_list_destroy(LinkedList** linked_list_pointer) {
    if (require_non_null(linked_list_pointer, *linked_list_pointer)) return;
    LinkedList* linked_list = *linked_list_pointer;
    destroy_nodes(linked_list);
    linked_list->memory_dealloc(linked_list);
    *linked_list_pointer = nullptr;
}

void linked_list_change_destructor(LinkedList* linked_list, void (*destruct)(void*)) {
    if (require_non_null(linked_list, destruct)) return;
    linked_list->destruct = destruct;
}

void linked_list_add(LinkedList* linked_list, int index, const void* element) {
    if (require_non_null(linked_list)) return;
    if (index < 0 || index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return;
    }
    Node* node = create_node(linked_list, element);
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new node'");
        return;
    }
    if (linked_list->size == 0) {
        linked_list->head = node;
        linked_list->tail = node;
    } else if (index == 0) {
        node->next = linked_list->head;
        linked_list->head->prev = node;
        linked_list->head = node;
    } else if (index == linked_list->size) {
        node->prev = linked_list->tail;
        linked_list->tail->next = node;
        linked_list->tail = node;
    } else {
        Node* current = get_node(linked_list, index);
        node->prev = current->prev;
        node->prev->next = node;
        node->next = current;
        current->prev = node;
    }
    linked_list->size++;
    linked_list->modification_count++;
}

void linked_list_add_first(LinkedList* linked_list, const void* element) {
    linked_list_add(linked_list, 0, element);
}

void linked_list_add_last(LinkedList* linked_list, const void* element) {
    linked_list_add(linked_list, linked_list ? linked_list->size : -1, element);
}

void linked_list_add_all(LinkedList* linked_list, int index, Collection collection) {
    if (require_non_null(linked_list)) return;
    if (index < 0 || index > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return;
    }
    Iterator* iterator; Error error;
    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    Node* head = nullptr, * tail = nullptr;
    while (iterator_has_next(iterator)) {
        Node* node = create_node(linked_list, iterator_next(iterator));
        if (!node) {
            while (head) {
                Node* temporary = head;
                head = head->next;
                linked_list->memory_dealloc(temporary);
            }
            iterator_destroy(&iterator);
            set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new node'");
            return;
        }
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = tail->next;
        }
    }
    iterator_destroy(&iterator);
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
    linked_list->modification_count++;
}

void linked_list_add_all_first(LinkedList* linked_list, Collection collection) {
    linked_list_add_all(linked_list, 0, collection);
}

void linked_list_add_all_last(LinkedList* linked_list, Collection collection) {
    linked_list_add_all(linked_list, linked_list ? linked_list->size : -1, collection);
}

void* linked_list_get(const LinkedList* linked_list, int index) {
    if (require_non_null(linked_list)) return nullptr;
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    return get_node(linked_list, index)->element;
}

void* linked_list_get_first(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    if (!linked_list->head) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return linked_list->head->element;
}

void* linked_list_get_last(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    if (!linked_list->tail) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return linked_list->tail->element;
}

void* linked_list_set(LinkedList* linked_list, int index, const void* element) {
    if (require_non_null(linked_list)) return nullptr;
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    void* old_element = node->element;
    if (old_element != element) {
        linked_list->destruct(old_element);
    }
    node->element = (void*) element;
    return old_element;
}

void linked_list_swap(LinkedList* linked_list, int index_a, int index_b) {
    if (require_non_null(linked_list)) return;
    if (index_a < 0 || index_a >= linked_list->size || index_b < 0 || index_b >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid indexes first = %d, second = %d; size = %d", index_a, index_b, linked_list->size);
        return;
    }
    swap(&get_node(linked_list, index_a)->element, &get_node(linked_list, index_b)->element);
}

void* linked_list_remove(LinkedList* linked_list, int index) {
    if (require_non_null(linked_list)) return nullptr;
    if (index < 0 || index >= linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, linked_list->size);
        return nullptr;
    }
    Node* node = get_node(linked_list, index);
    return remove_node(linked_list, node);
}

void* linked_list_remove_first(LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return remove_node(linked_list, linked_list->head);
}

void* linked_list_remove_last(LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    if (linked_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'linked_list' is empty");
        return nullptr;
    }
    return remove_node(linked_list, linked_list->tail);
}

bool linked_list_remove_element(LinkedList* linked_list, const void* element) {
    if (require_non_null(linked_list)) return false;
    Node* node = find_node(linked_list, element);
    if (node) {
        remove_node(linked_list, node);
        return true;
    }
    return false;
}

int linked_list_remove_all(LinkedList* linked_list, Collection collection) {
    if (require_non_null(linked_list)) return 0;
    int count = 0;
    for (Node* node = linked_list->head, * next; node; node = next) {
        next = node->next;
        if (collection_contains(collection, node->element)) {
            remove_node(linked_list, node);
            count++;
        }
    }
    return count;
}

int linked_list_remove_range(LinkedList* linked_list, int start_index, int end_index) {
    if (require_non_null(linked_list)) return 0;
    if (start_index < 0 || end_index > linked_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, end = %d; size = %d", start_index, end_index, linked_list->size);
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
    if (require_non_null(linked_list, condition)) return 0;
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
    if (require_non_null(linked_list, operator)) return;
    for (Node* node = linked_list->head; node; node = node->next) {
        void* old_element = node->element;
        void* new_element = operator(old_element);
        if (old_element != new_element) {
            linked_list->destruct(old_element);
        }
        node->element = new_element;
    }
}

int linked_list_retain_all(LinkedList* linked_list, Collection collection) {
    if (require_non_null(linked_list)) return 0;
    int count = 0;
    for (Node* node = linked_list->head, * next; node; node = next) {
        next = node->next;
        if (!collection_contains(collection, node->element)) {
            remove_node(linked_list, node);
            count++;
        }
    }
    return count;
}

int linked_list_size(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return 0;
    return linked_list->size;
}

bool linked_list_is_empty(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return false;
    return linked_list->size == 0;
}

Iterator* linked_list_iterator(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    Iterator* iterator = create_iterator(linked_list, 0);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

Iterator* linked_list_iterator_at(const LinkedList* linked_list, int position) {
    if (require_non_null(linked_list)) return nullptr;
    if (position < 0 || position > linked_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "position %d out of bounds for size %d", position, linked_list->size);
        return nullptr;
    }
    Iterator* iterator = create_iterator(linked_list, position);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool linked_list_equals(const LinkedList* linked_list, const LinkedList* other_linked_list) {
    if (require_non_null(linked_list, other_linked_list)) return false;
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
    if (require_non_null(linked_list, action)) return;
    for (const Node* node = linked_list->head; node; node = node->next) {
        action(node->element);
    }
}

void linked_list_sort(LinkedList* linked_list, Comparator comparator, SortingAlgorithm algorithm) {
    if (require_non_null(linked_list, comparator)) return;
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
    if (require_non_null(linked_list, random)) return;
    bool shuffled = false;
    switch (algorithm) {
        case DURSTENFELD_SHUFFLE: { shuffled = durstenfeld_shuffle(linked_list, random); break; }
        case SATTOLO_SHUFFLE: { shuffled = sattolo_shuffle(linked_list, random); break; }
        case NAIVE_SHUFFLE: { shuffled = naive_shuffle(linked_list, random); break; }
    }
    if (!shuffled) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory while converting 'linked_list' to array");
    }
}

void linked_list_reverse(LinkedList* linked_list) {
    if (require_non_null(linked_list)) return;
    Node* first = linked_list->head, * second = linked_list->tail;
    while (first != second && first->prev != second) {
        swap(&first->element, &second->element);
        first = first->next;
        second = second->prev;
    }
}

void linked_list_rotate(LinkedList* linked_list, int distance) {
    if (require_non_null(linked_list)) return;
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
    if (require_non_null(linked_list)) return;
    destroy_nodes(linked_list);
    linked_list->head = linked_list->tail = nullptr;
    linked_list->size = 0;
    linked_list->modification_count++;
}

Optional linked_list_find(const LinkedList* linked_list, Predicate condition) {
    if (require_non_null(linked_list, condition)) return optional_empty();
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (condition(node->element)) {
            return optional_of(node->element);
        }
    }
    return optional_empty();
}

Optional linked_list_find_last(const LinkedList* linked_list, Predicate condition) {
    if (require_non_null(linked_list, condition)) return optional_empty();
    for (const Node* node = linked_list->tail; node; node = node->prev) {
        if (condition(node->element)) {
            return optional_of(node->element);
        }
    }
    return optional_empty();
}

int linked_list_index_where(const LinkedList* linked_list, Predicate condition) {
    if (require_non_null(linked_list, condition)) return 0;
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
    if (require_non_null(linked_list, condition)) return 0;
    int index = linked_list->size - 1;
    for (const Node* node = linked_list->tail; node; node = node->prev) {
        if (condition(node->element)) {
            return index;
        }
        index--;
    }
    return -1;
}

bool linked_list_contains(const LinkedList* linked_list, const void* element) {
    if (require_non_null(linked_list)) return false;
    return linked_list_index_of(linked_list, element) >= 0;
}

bool linked_list_contains_all(const LinkedList* linked_list, Collection collection) {
    if (require_non_null(linked_list)) return false;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
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
    iterator_destroy(&iterator);
    return contains;
}

void* linked_list_reduce(const LinkedList* linked_list, void* identity, BiOperator accumulator) {
    if (require_non_null(linked_list, accumulator)) return nullptr;
    void* result = identity;
    for (const Node* node = linked_list->head; node; node = node->next) {
        result = accumulator(result, node->element);
    }
    return result;
}

int linked_list_occurrences_of(const LinkedList* linked_list, const void* element) {
    if (require_non_null(linked_list)) return 0;
    int count = 0;
    for (const Node* node = linked_list->head; node; node = node->next) {
        if (linked_list->equals(node->element, element)) {
            count++;
        }
    }
    return count;
}

int linked_list_index_of(const LinkedList* linked_list, const void* element) {
    if (require_non_null(linked_list)) return 0;
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
    if (require_non_null(linked_list)) return 0;
    int index = linked_list->size - 1;
    for (const Node* node = linked_list->tail; node; node = node->prev) {
        if (linked_list->equals(node->element, element)) {
            return index;
        }
        index--;
    }
    return -1;
}

LinkedList* linked_list_clone(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    LinkedList* new_linked_list; Error error;

    if ((error = attempt(new_linked_list = linked_list_sub_list(linked_list, 0, linked_list->size)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return new_linked_list;
}

LinkedList* linked_list_sub_list(const LinkedList* linked_list, int start_index, int end_index) {
    if (require_non_null(linked_list)) return nullptr;
    if (start_index < 0 || end_index > linked_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, end = %d; size = %d", start_index, end_index, linked_list->size);
        return nullptr;
    }
    LinkedList* new_linked_list; Error error;
    if ((error = attempt(new_linked_list = linked_list_new(&(LinkedListOptions) {
        .destruct = noop_destruct,
        .equals = linked_list->equals,
        .to_string = linked_list->to_string,
        .memory_alloc = linked_list->memory_alloc,
        .memory_dealloc = linked_list->memory_dealloc
    })))) {
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
    if (require_non_null(linked_list)) return (Collection) {};
    return (Collection) {
        .data_structure = linked_list,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
}

void** linked_list_to_array(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return nullptr;
    void** elements = linked_list->memory_alloc(linked_list->size * sizeof(void*));
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    const Node* node = linked_list->head;
    for (int i = 0; i < linked_list->size; i++) {
        elements[i] = node->element;
        node = node->next;
    }
    return elements;
}

StringOwned linked_list_to_string(const LinkedList* linked_list) {
    if (require_non_null(linked_list)) return string_null();

    char* raw_string = linked_list->memory_alloc(calculate_string_size(linked_list));
    if (!raw_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
        return string_null();
    }
    raw_string[0] = '\0'; // initialize string to ignore memory garbage
    strcat(raw_string, linked_list->size == 0 ? "[" : "[ ");

    for (const Node* node = linked_list->head; node; node = node->next) {
        constexpr int NULL_TERMINATOR = 1;
        const int length = linked_list->to_string(node->element, nullptr, 0) + NULL_TERMINATOR;

        char* raw_element_string = linked_list->memory_alloc(length);
        if (!raw_element_string) {
            linked_list->memory_dealloc(raw_string);
            set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
            return string_null();
        }
        linked_list->to_string(node->element, raw_element_string, length);
        strcat(raw_string, raw_element_string);

        if (node->next) {
            strcat(raw_string, ", ");
        }
        linked_list->memory_dealloc(raw_element_string);
    }

    strcat(raw_string, linked_list->size == 0 ? "]" : " ]");
    return string_view(raw_string);
}

static size_t calculate_string_size(const LinkedList* linked_list) {
    constexpr int BRACKETS = 2; constexpr int COMMA_SPACE = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (const Node* node = linked_list->head; node; node = node->next) {
        length += linked_list->to_string(node->element, nullptr, 0);

        if (node == linked_list->head) length += 1; // space after opening bracket
        if (node != linked_list->tail) length += COMMA_SPACE; // prevent ", " on the last element
        if (node == linked_list->tail) length += 1; // space before closing bracket
    }
    return length + BRACKETS + NULL_TERMINATOR;
}

static Node* create_node(const LinkedList* linked_list, const void* element) {
    Node* node = linked_list->memory_alloc(sizeof(Node));
    if (!node) {
        return nullptr;
    }
    node->element = (void*) element;
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
    for (Node* node = linked_list->head; node; node = node->next) {
        if (linked_list->equals(node->element, element)) {
            return node;
        }
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
    linked_list->destruct(node->element);
    linked_list->memory_dealloc(node);
    linked_list->size--;
    linked_list->modification_count++;
    return element;
}

static void destroy_nodes(LinkedList* linked_list) {
    for (Node* current = linked_list->head, * next; current; current = next) {
        next = current->next;
        linked_list->destruct(current->element);
        linked_list->memory_dealloc(current);
    }
}

typedef struct {
    Iterator iterator;
    LinkedList* linked_list;
    Node* current;
    Node* last_returned;
    int cursor;
    int modification_count;
} IterationContext;

static Iterator* create_iterator(const LinkedList* linked_list, int position) {
    IterationContext* iteration_context = linked_list->memory_alloc(sizeof(IterationContext));

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
    iteration_context->iterator.memory_dealloc = linked_list->memory_dealloc;

    iteration_context->linked_list = (LinkedList*) linked_list;
    iteration_context->current = position == linked_list->size ? nullptr : get_node(linked_list, position);
    iteration_context->last_returned = nullptr;
    iteration_context->cursor = position;
    iteration_context->modification_count = linked_list->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->cursor < iteration_context->linked_list->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->linked_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    void* element = iteration_context->current->element;
    iteration_context->last_returned = iteration_context->current;
    iteration_context->current = iteration_context->current->next;
    iteration_context->cursor++;
    return element;
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->cursor > 0;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->linked_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_previous_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    if (!iteration_context->current) {
       iteration_context->current = iteration_context->linked_list->tail;
    } else {
        iteration_context->current = iteration_context->current->prev;
    }
    iteration_context->last_returned = iteration_context->current;
    iteration_context->cursor--;
    return iteration_context->last_returned->element;
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->linked_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    Node* node = create_node(iteration_context->linked_list, element);
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new node'");
        return;
    }
    LinkedList* linked_list = iteration_context->linked_list;
    if (linked_list->size == 0) {
        linked_list->head = node;
        linked_list->tail = node;
    } else if (iteration_context->cursor == 0) {
        node->next = linked_list->head;
        linked_list->head->prev = node;
        linked_list->head = node;
    } else if (iteration_context->cursor == linked_list->size) {
        node->prev = linked_list->tail;
        linked_list->tail->next = node;
        linked_list->tail = node;
    } else {
        Node* current = iteration_context->current;
        node->prev = current->prev;
        node->prev->next = node;
        node->next = current;
        current->prev = node;
    }
    iteration_context->linked_list->size++;
    iteration_context->cursor++;
    iteration_context->last_returned = nullptr;
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->linked_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    if (!iteration_context->last_returned) {
        set_error(ILLEGAL_STATE_ERROR, "set() called before any next() or previous() call");
        return;
    }
    void* old_element = iteration_context->last_returned->element;
    if (old_element != element) {
        iteration_context->linked_list->destruct(old_element);
    }
    iteration_context->last_returned->element = (void*) element;
}

static void iterator_remove_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->linked_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    if (!iteration_context->last_returned) {
        set_error(ILLEGAL_STATE_ERROR, "remove() called twice or before any next() or previous() call");
        return;
    }
    Node* node_to_remove = iteration_context->last_returned;
    if (iteration_context->current == node_to_remove) {
        iteration_context->current = node_to_remove->next;
    } else {
        iteration_context->cursor--;
    }
    remove_node(iteration_context->linked_list, node_to_remove);
    iteration_context->last_returned = nullptr;
    iteration_context->modification_count = iteration_context->linked_list->modification_count;
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->current = iteration_context->linked_list->head;
    iteration_context->last_returned = nullptr;
    iteration_context->cursor = 0;
    iteration_context->modification_count = iteration_context->linked_list->modification_count;
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

static bool durstenfeld_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        return false;
    }
    for (int i = linked_list->size - 1; i > 0; i--) {
        const int j = random() % (i + 1);
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_dealloc(elements);
    return true;
}

static bool sattolo_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        return false;
    }
    for (int i = linked_list->size - 1; i > 0; i--) {
        const int j = random() % i;
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_dealloc(elements);
    return true;
}

static bool naive_shuffle(LinkedList* linked_list, int (*random)(void)) {
    void** elements = linked_list_to_array(linked_list);
    if (!elements) {
        return false;
    }
    for (int i = 0; i < linked_list->size; i++) {
        const int j = random() % linked_list->size;
        swap(&elements[i], &elements[j]);
    }
    array_to_linked_list(elements, linked_list);
    linked_list->memory_dealloc(elements);
    return true;
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

static int collection_size_internal(const void* linked_list) {
    return linked_list_size(linked_list);
}

static Iterator* collection_iterator_internal(const void* linked_list) {
    return linked_list_iterator(linked_list);
}

static bool collection_contains_internal(const void* linked_list, const void* element) {
    return linked_list_contains(linked_list, element);
}