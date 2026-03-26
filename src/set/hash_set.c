#include "hash_set.h"

#include "sets.h"
#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

constexpr int MIN_CAPACITY = 8;
constexpr int MAX_CAPACITY = 1'073'741'824;
constexpr float MIN_LOAD_FACTOR = 0.5;
constexpr int GROWN_FACTOR = 2;

typedef struct Node {
    void* element;
    struct Node* next;
} Node;

struct HashSet {
    Node** buckets;
    int size;
    int capacity;
    int threshold;
    float load_factor;
    uint64_t (*hash)(const void* element);
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

static size_t calculate_string_size(const HashSet*);

static int next_power_of_two(int x);

static bool ensure_capacity(HashSet*);

static Node* create_node(const HashSet*, const void*);

static void* remove_node(HashSet*, int, Node*, Node*);

static void destroy_nodes(HashSet*);

static Iterator* create_iterator(const HashSet*);

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

HashSet* hash_set_new(const HashSetOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->load_factor < MIN_LOAD_FACTOR || !options->hash || !options->destruct
        || !options->equals || !options->to_string || !options->memory_alloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    HashSet* hash_set = options->memory_alloc(sizeof(HashSet));
    if (!hash_set) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_set'");
        return nullptr;
    }
    hash_set->capacity = next_power_of_two(options->initial_capacity);
    hash_set->buckets = options->memory_alloc(hash_set->capacity * sizeof(Node*));
    if (!hash_set->buckets) {
        options->memory_free(hash_set);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'hash_set'");
        return nullptr;
    }
    memset(hash_set->buckets, 0, hash_set->capacity * sizeof(Node*));
    hash_set->size = 0;
    hash_set->threshold = hash_set->capacity * options->load_factor;
    hash_set->load_factor = options->load_factor;
    hash_set->hash = options->hash;
    hash_set->destruct = options->destruct;
    hash_set->equals = options->equals;
    hash_set->to_string = options->to_string;
    hash_set->memory_alloc = options->memory_alloc;
    hash_set->memory_free = options->memory_free;
    hash_set->modification_count = 0;
    hash_set->view.sets.first = hash_set;
    hash_set->view.sets.second = nullptr;
    hash_set->view.size = set_view_size_internal;
    hash_set->view.iterator = set_view_iterator_internal;
    hash_set->view.contains = set_view_contains_internal;
    return hash_set;
}

HashSet* hash_set_from(Collection collection, const HashSetOptions* options) {
    if (require_non_null(options)) return nullptr;
    HashSet* hash_set; Error error;

    if ((error = attempt(hash_set = hash_set_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(hash_set_add_all(hash_set, collection)))) {
        hash_set_destroy(&hash_set);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return hash_set;
}

void hash_set_destroy(HashSet** hash_set_pointer) {
    if (require_non_null(hash_set_pointer, *hash_set_pointer)) return;
    HashSet* hash_set = *hash_set_pointer;
    destroy_nodes(hash_set);
    hash_set->memory_free(hash_set->buckets);
    hash_set->memory_free(hash_set);
    *hash_set_pointer = nullptr;
}

void hash_set_set_destructor(HashSet* hash_set, void (*destructor)(void*)) {
    if (require_non_null(hash_set, destructor)) return;
    hash_set->destruct = destructor;
}

bool hash_set_add(HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    if (hash_set_contains(hash_set, element)) {
        return false;
    }
    if (!ensure_capacity(hash_set)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'hash_set' capacity");
        return false;
    }
    Node* node = create_node(hash_set, element);
    if (!node) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'new node'");
        return false;
    }
    const int index = hash_set->hash(element) & (hash_set->capacity - 1);
    Node* current = hash_set->buckets[index];
    if (current) {
        node->next = current;
    }
    hash_set->buckets[index] = node;
    hash_set->size++;
    hash_set->modification_count++;
    return true;
}

bool hash_set_add_all(HashSet* hash_set, Collection collection) {
    if (require_non_null(hash_set)) return false;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool changed = false;
    while (iterator_has_next(iterator)) {
        bool added = false;
        if ((error = attempt(added = hash_set_add(hash_set, iterator_next(iterator))))) {
            set_error(error, "%s", plain_error_message());
            break;
        }
        if (added) {
            changed = true;
        }
    }
    iterator_destroy(&iterator);
    return changed;
}

bool hash_set_remove(HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    const int index = hash_set->hash(element) & (hash_set->capacity - 1);
    Node* prev_node = nullptr, * node = hash_set->buckets[index];
    while (node && !hash_set->equals(node->element, element)) {
        prev_node = node;
        node = node->next;
    }
    if (!node) {
        return false;
    }
    return remove_node(hash_set, index, prev_node, node);
}

int hash_set_remove_all(HashSet* hash_set, Collection collection) {
    if (require_non_null(hash_set)) return false;
    int count = 0;
    for (int i = 0; i < hash_set->capacity; i++) {
        for (Node* node = hash_set->buckets[i], * prev = nullptr, * next; node; node = next) {
            next = node->next;
            if (collection_contains(collection, node->element)) {
                remove_node(hash_set, i, prev, node);
                count++;
            } else {
                prev = node;
            }
        }
    }
    return count;
}

int hash_set_remove_if(HashSet* hash_set, Predicate condition) {
    if (require_non_null(hash_set, condition)) return false;
    int count = 0;
    for (int i = 0; i < hash_set->capacity; i++) {
        for (Node* node = hash_set->buckets[i], * prev = nullptr, * next; node; node = next) {
            next = node->next;
            if (condition(node->element)) {
                remove_node(hash_set, i, prev, node);
                count++;
            } else {
                prev = node;
            }
        }
    }
    return count;
}

int hash_set_retain_all(HashSet* hash_set, Collection collection) {
    if (require_non_null(hash_set)) return false;
    int count = 0;
    for (int i = 0; i < hash_set->capacity; i++) {
        for (Node* node = hash_set->buckets[i], * prev = nullptr, * next; node; node = next) {
            next = node->next;
            if (!collection_contains(collection, node->element)) {
                remove_node(hash_set, i, prev, node);
                count++;
            } else {
                prev = node;
            }
        }
    }
    return count;
}

int hash_set_size(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return 0;
    return hash_set->size;
}

int hash_set_capacity(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return 0;
    return hash_set->capacity;
}

bool hash_set_is_empty(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return false;
    return hash_set->size == 0;
}

Iterator* hash_set_iterator(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return nullptr;
    Iterator* iterator = create_iterator(hash_set);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool hash_set_equals(const HashSet* hash_set, const HashSet* other_hash_set) {
    if (require_non_null(hash_set, other_hash_set)) return false;
    if (hash_set == other_hash_set) {
        return true;
    }
    if (hash_set->size != other_hash_set->size) {
        return false;
    }
    for (int i = 0; i < other_hash_set->capacity; i++) {
        for (const Node* node = other_hash_set->buckets[i]; node; node = node->next) {
            if (!hash_set_contains(hash_set, node->element)) {
                return false;
            }
        }
    }
    return true;
}

void hash_set_for_each(HashSet* hash_set, Consumer action) {
    if (require_non_null(hash_set, action)) return;
    for (int i = 0; i < hash_set->capacity; i++) {
        for (const Node* node = hash_set->buckets[i]; node; node = node->next) {
            action(node->element);
        }
    }
}

void hash_set_clear(HashSet* hash_set) {
    if (require_non_null(hash_set)) return;
    destroy_nodes(hash_set);
    memset(hash_set->buckets, 0, hash_set->capacity * sizeof(Node*));
    hash_set->size = 0;
    hash_set->modification_count++;
}

bool hash_set_contains(const HashSet* hash_set, const void* element) {
    if (require_non_null(hash_set)) return false;
    const int index = hash_set->hash(element) & (hash_set->capacity - 1);
    for (const Node* node = hash_set->buckets[index]; node; node = node->next) {
        if (hash_set->equals(node->element, element)) {
            return true;
        }
    }
    return false;
}

bool hash_set_contains_all(const HashSet* hash_set, Collection collection) {
    if (require_non_null(hash_set)) return false;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!hash_set_contains(hash_set, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

Collection hash_set_to_collection(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return (Collection) {};
    return (Collection) {
        .data_structure = hash_set,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
}

HashSet* hash_set_clone(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return nullptr;
    HashSet* new_hash_set; Error error;

    if ((error = attempt(new_hash_set = hash_set_new(&(HashSetOptions) {
        .initial_capacity = hash_set->capacity,
        .load_factor = hash_set->load_factor,
        .hash = hash_set->hash,
        .destruct = noop_destruct,
        .equals = hash_set->equals,
        .to_string = hash_set->to_string,
        .memory_alloc = hash_set->memory_alloc,
        .memory_free = hash_set->memory_free
    })))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = 0; i < hash_set->capacity; i++) {
        for (const Node* node = hash_set->buckets[i]; node; node = node->next) {
            if ((error = attempt(hash_set_add(new_hash_set, node->element)))) {
                hash_set_destroy(&new_hash_set);
                set_error(error, "%s", plain_error_message());
                return nullptr;
            }
        }
    }
    return new_hash_set;
}

void** hash_set_to_array(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return nullptr;
    void** elements = hash_set->memory_alloc(hash_set->size * sizeof(void*));
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    for (int i = 0, j = 0; i < hash_set->capacity; i++) {
        for (const Node* node = hash_set->buckets[i]; node; node = node->next) {
            elements[j++] = node->element;
        }
    }
    return elements;
}

char* hash_set_to_string(const HashSet* hash_set) {
    if (require_non_null(hash_set)) return nullptr;

    char* string = hash_set->memory_alloc(calculate_string_size(hash_set));
    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
        return nullptr;
    }
    string[0] = '\0'; // initialize string to ignore memory garbage
    strcat(string, hash_set->size == 0 ? "(" : "( ");

    for (int i = 0; i < hash_set->capacity; i++) {
        for (const Node* node = hash_set->buckets[i]; node; node = node->next) {
            constexpr int NULL_TERMINATOR = 1;
            const int length = hash_set->to_string(node->element, nullptr, 0) + NULL_TERMINATOR;

            char* element_string = hash_set->memory_alloc(length);
            if (!element_string) {
                hash_set->memory_free(string);
                set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
                return nullptr;
            }
            hash_set->to_string(node->element, element_string, length);
            strcat(string, element_string);

            if (i < hash_set->size - 1) {
                strcat(string, ", ");
            }
            hash_set->memory_free(element_string);
        }
    }

    strcat(string, hash_set->size == 0 ? ")" : " )");
    return string;
}

static size_t calculate_string_size(const HashSet* hash_set) {
    constexpr int PARENTHESES = 2; constexpr int COMMA_SPACE = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0; i < hash_set->capacity; i++) {
        for (const Node* node = hash_set->buckets[i]; node; node = node->next) {
            length += hash_set->to_string(node->element, nullptr, 0);

            if (i == 0) length += 1; // space after opening parenthesis
            if (i < hash_set->size - 1) length += COMMA_SPACE; // prevent ", " on the last element
            if (i == hash_set->size - 1) length += 1; // space before closing parenthesis
        }
    }
    return length + PARENTHESES + NULL_TERMINATOR;
}

static int next_power_of_two(int x) {
    int power = 1;
    while (power < x) {
        power <<= 1;
    }
    return power;
}

static bool ensure_capacity(HashSet* hash_set) {
    const int new_capacity = hash_set->capacity > (MAX_CAPACITY / GROWN_FACTOR)
        ? MAX_CAPACITY
        : hash_set->capacity * GROWN_FACTOR;
    if (hash_set->size < hash_set->threshold) {
        return true;
    }
    Node** buckets = hash_set->memory_alloc(new_capacity * sizeof(Node*));
    if (!buckets) {
        return false;
    }
    memset(buckets, 0, new_capacity * sizeof(Node*));
    for (int i = 0; i < hash_set->capacity; i++) {
       for (Node* node = hash_set->buckets[i], * next; node; node = next) {
            const int index = hash_set->hash(node->element) & (new_capacity - 1);
            next = node->next;
            node->next = nullptr;

            Node* current = buckets[index];
            if (current) {
                node->next = current;
            }
            buckets[index] = node;
        }
    }
    hash_set->memory_free(hash_set->buckets);
    hash_set->buckets = buckets;
    hash_set->capacity = new_capacity;
    hash_set->threshold = hash_set->capacity * hash_set->load_factor;
    return true;
}

static Node* create_node(const HashSet* hash_set, const void* element) {
    Node* node = hash_set->memory_alloc(sizeof(Node));
    if (!node) {
        return nullptr;
    }
    node->element = (void*) element;
    node->next = nullptr;
    return node;
}

static void* remove_node(HashSet* hash_set, int bucket, Node* prev_node, Node* node) {
    if (prev_node) {
        prev_node->next = node->next;
    } else {
        hash_set->buckets[bucket] = node->next;
    }
    void* element = node->element;
    hash_set->destruct(node->element);
    hash_set->memory_free(node);
    hash_set->size--;
    hash_set->modification_count++;
    return element;
}

static void destroy_nodes(HashSet* hash_set) {
    for (int i = 0; i < hash_set->capacity; i++) {
        for (Node* current = hash_set->buckets[i], * next; current; current = next) {
            hash_set->destruct(current->element);

            next = current->next;
            hash_set->memory_free(current);
        }
    }
}

typedef struct {
    Iterator iterator;
    HashSet* hash_set;
    Node* node;
    int cursor;
    int count;
    bool last_returned;
    bool last_removed;
    int modification_count;
} IterationContext;

static Iterator* create_iterator(const HashSet* hash_set) {
    IterationContext* iteration_context = hash_set->memory_alloc(sizeof(IterationContext));

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
    iteration_context->iterator.memory_free = hash_set->memory_free;

    iteration_context->hash_set = (HashSet*) hash_set;
    iteration_context->node = nullptr;
    iteration_context->cursor = 0;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = hash_set->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count < iteration_context->hash_set->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->hash_set->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    while (iteration_context->cursor < iteration_context->hash_set->capacity) {
        if (!iteration_context->node) {
            iteration_context->node = iteration_context->hash_set->buckets[iteration_context->cursor++];
        } else if (!iteration_context->last_removed) {
            iteration_context->node = iteration_context->node->next;
        }
        iteration_context->last_removed = false;
        if (iteration_context->node) {
            iteration_context->count++;
            iteration_context->last_returned = true;
            return iteration_context->node->element;
        }
    }
    set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
    return nullptr;
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "hash set iterators doesn't support backward traversal");
    return false;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "hash set iterators doesn't support backward traversal");
    return nullptr;
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "hash set iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "hash set iterators doesn't support setting elements");
}

static void iterator_remove_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->hash_set->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return;
    }
    if (!iteration_context->last_returned) {
        set_error(ILLEGAL_STATE_ERROR, "remove() called twice or before any next() or previous() call");
        return;
    }
    Node* next = iteration_context->node->next;
    hash_set_remove(iteration_context->hash_set, iteration_context->node->element);
    iteration_context->node = next;
    iteration_context->last_returned = false;
    iteration_context->last_removed = true;
    iteration_context->modification_count = iteration_context->hash_set->modification_count;
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->node = nullptr;
    iteration_context->cursor = 0;
    iteration_context->count = 0;
    iteration_context->last_returned = false;
    iteration_context->last_removed = false;
    iteration_context->modification_count = iteration_context->hash_set->modification_count;
}

static int collection_size_internal(const void* hash_set) {
    return hash_set_size(hash_set);
}

static Iterator* collection_iterator_internal(const void* hash_set) {
    return hash_set_iterator(hash_set);
}

static bool collection_contains_internal(const void* hash_set, const void* element) {
    return hash_set_contains(hash_set, element);
}

static int set_view_size_internal(const void* hash_set) {
    return hash_set_size(((Pair*) hash_set)->first);
}

static Iterator* set_view_iterator_internal(const void* hash_set) {
    return hash_set_iterator(((Pair*) hash_set)->first);
}

static bool set_view_contains_internal(const void* hash_set, const void* element) {
    return hash_set_contains(((Pair*) hash_set)->first, element);
}

SetView* _hash_set_view(const HashSet* hash_set) {
    return hash_set ? (SetView*) &hash_set->view : nullptr;
}