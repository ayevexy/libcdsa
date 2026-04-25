#include "priority_queue.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

constexpr int MIN_CAPACITY = 10;
constexpr int MAX_CAPACITY = 1'073'741'824;
constexpr float MIN_GROWTH_FACTOR = 1.10f;

struct PriorityQueue {
    void** elements;
    int size;
    int capacity;
    float growth_factor;
    struct {
        void (*destruct)(void*);
        int (*compare)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_dealloc)(void*);
    };
    int modification_count;
};

static size_t calculate_string_size(const PriorityQueue*);

static bool ensure_capacity(PriorityQueue*, int);

static void heapify_after_insert(PriorityQueue*, int);

static void heapify_after_delete(PriorityQueue*, int);

static Iterator* create_iterator(const PriorityQueue*);

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

PriorityQueue* priority_queue_new(const PriorityQueueOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->growth_factor < MIN_GROWTH_FACTOR || !options->destruct || !options->compare
        || !options->to_string || !options->memory_alloc || !options->memory_dealloc
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    PriorityQueue* priority_queue = options->memory_alloc(sizeof(PriorityQueue));
    if (!priority_queue) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'priority_queue'");
        return nullptr;
    }
    priority_queue->elements = options->memory_alloc(options->initial_capacity * sizeof(void*));
    if (!priority_queue->elements) {
        options->memory_dealloc(priority_queue);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'priority_queue'");
        return nullptr;
    }
    priority_queue->size = 0;
    priority_queue->capacity = options->initial_capacity;
    priority_queue->growth_factor = options->growth_factor;
    priority_queue->destruct = options->destruct;
    priority_queue->compare = options->compare;
    priority_queue->to_string = options->to_string;
    priority_queue->memory_alloc = options->memory_alloc;
    priority_queue->memory_dealloc = options->memory_dealloc;
    priority_queue->modification_count = 0;
    return priority_queue;
}

PriorityQueue* priority_queue_from(Collection collection, const PriorityQueueOptions* options) {
    if (require_non_null(options)) return nullptr;
    PriorityQueue* priority_queue; Error error;

    if ((error = attempt(priority_queue = priority_queue_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(priority_queue_enqueue_all(priority_queue, collection)))) {
        priority_queue_destroy(&priority_queue);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return priority_queue;
}

void priority_queue_destroy(PriorityQueue** priority_queue_pointer) {
    if (require_non_null(priority_queue_pointer, *priority_queue_pointer)) return;
    PriorityQueue* priority_queue = *priority_queue_pointer;
    for (int i = 0; i < priority_queue->size; i++) {
        priority_queue->destruct(priority_queue->elements[i]);
    }
    priority_queue->memory_dealloc(priority_queue->elements);
    priority_queue->memory_dealloc(priority_queue);
    *priority_queue_pointer = nullptr;
}

void priority_queue_change_destructor(PriorityQueue* priority_queue, void (*destruct)(void*)) {
    if (require_non_null(priority_queue, destruct)) return;
    priority_queue->destruct = destruct;
}

void priority_queue_enqueue(PriorityQueue* priority_queue, const void* element) {
    if (require_non_null(priority_queue)) return;

    if (!ensure_capacity(priority_queue, priority_queue->size + 1)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'priority_queue' capacity");
        return;
    }
    priority_queue->elements[priority_queue->size] = (void*) element;
    priority_queue->size++;
    priority_queue->modification_count++;

    heapify_after_insert(priority_queue, priority_queue->size - 1);
}

void priority_queue_enqueue_all(PriorityQueue* priority_queue, Collection collection) {
    if (require_non_null(priority_queue)) return;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    if (!ensure_capacity(priority_queue, priority_queue->size + collection_size(collection))) {
        iterator_destroy(&iterator);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'priority_queue' capacity");
        return;
    }
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        priority_queue_enqueue(priority_queue, element);
    }
    iterator_destroy(&iterator);
}

void* priority_queue_peek(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;
    if (priority_queue->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'priority_queue' is empty");
        return nullptr;
    }
    return priority_queue->elements[0];
}

void* priority_queue_dequeue(PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;

    if (priority_queue->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'priority_queue' is empty");
        return nullptr;
    }
    void* element = priority_queue->elements[0];
    priority_queue->elements[0] = priority_queue->elements[priority_queue->size - 1];
    priority_queue->elements[priority_queue->size - 1] = element;
    element = priority_queue->elements[priority_queue->size - 1];

    priority_queue->size--;
    priority_queue->modification_count++;

    heapify_after_delete(priority_queue, 0);
    return element;
}

int priority_queue_size(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return 0;
    return priority_queue->size;
}

int priority_queue_capacity(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return 0;
    return priority_queue->capacity;
}

bool priority_queue_is_empty(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return false;
    return priority_queue->size == 0;
}

Iterator* priority_queue_iterator(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;
    Iterator* iterator = create_iterator(priority_queue);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool priority_queue_equals(const PriorityQueue* priority_queue, const PriorityQueue* other_priority_queue) {
    if (require_non_null(priority_queue, other_priority_queue)) return false;
    if (priority_queue == other_priority_queue) {
        return true;
    }
    if (priority_queue->size != other_priority_queue->size) {
        return false;
    }
    for (int i = 0; i < priority_queue->size; i++) {
        if (priority_queue->compare(priority_queue->elements[i], other_priority_queue->elements[i]) != 0) {
            return false;
        }
    }
    return true;
}

void priority_queue_for_each(PriorityQueue* priority_queue, Consumer action) {
    if (require_non_null(priority_queue, action)) return;
    for (int i = 0; i < priority_queue->size; i++) {
        action(priority_queue->elements[i]);
    }
}

void priority_queue_clear(PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return;
    for (int i = 0; i < priority_queue->size; i++) {
        priority_queue->destruct(priority_queue->elements[i]);
        priority_queue->elements[i] = nullptr;
    }
    priority_queue->size = 0;
    priority_queue->modification_count++;
}

bool priority_queue_contains(const PriorityQueue* priority_queue, const void* element) {
    if (require_non_null(priority_queue)) return false;
    for (int i = 0; i < priority_queue->size; i++) {
        if (priority_queue->compare(priority_queue->elements[i], element) == 0) {
            return true;
        }
    }
    return false;
}

bool priority_queue_contains_all(const PriorityQueue* priority_queue, Collection collection) {
    if (require_non_null(priority_queue)) return false;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!priority_queue_contains(priority_queue, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

PriorityQueue* priority_queue_clone(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;

    PriorityQueue* new_priority_queue; Error error;
    if ((error = attempt(new_priority_queue = priority_queue_new(&(PriorityQueueOptions) {
        .initial_capacity = priority_queue->capacity,
        .growth_factor = priority_queue->growth_factor,
        .destruct = noop_destruct,
        .compare = priority_queue->compare,
        .to_string = priority_queue->to_string,
        .memory_alloc = priority_queue->memory_alloc,
        .memory_dealloc = priority_queue->memory_dealloc
    })))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = 0; i < priority_queue->size; i++) {
        new_priority_queue->elements[i] = priority_queue->elements[i];
    }
    new_priority_queue->size = priority_queue->size;
    return new_priority_queue;
}

Collection priority_queue_to_collection(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return (Collection) {};
    return (Collection) {
        .data_structure = priority_queue,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
}

void** priority_queue_to_array(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;
    void** elements = priority_queue->memory_alloc(priority_queue->size * sizeof(void*));
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    for (int i = 0; i < priority_queue->size; i++) {
        elements[i] = priority_queue->elements[i];
    }
    return elements;
}

StringOwned priority_queue_to_string(const PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return string_null();

    char* raw_string = strings_memory_alloc(calculate_string_size(priority_queue));
    if (!raw_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
        return string_null();
    }
    raw_string[0] = '\0'; // initialize string to ignore memory garbage
    strcat(raw_string, priority_queue->size == 0 ? "|" : "| ");

    for (int i = 0; i < priority_queue->size; i++) {
        constexpr int NULL_TERMINATOR = 1;
        const int length = priority_queue->to_string(priority_queue->elements[i], nullptr, 0) + NULL_TERMINATOR;

        char* raw_element_string = strings_memory_alloc(length);
        if (!raw_element_string) {
            strings_memory_dealloc(raw_string);
            set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
            return string_null();
        }
        priority_queue->to_string(priority_queue->elements[i], raw_element_string, length);
        strcat(raw_string, raw_element_string);

        if (i < priority_queue->size - 1) {
            strcat(raw_string, ", ");
        }
        strings_memory_dealloc(raw_element_string);
    }

    strcat(raw_string, priority_queue->size == 0 ? "|" : " |");
    return string_view(raw_string);
}

static size_t calculate_string_size(const PriorityQueue* priority_queue) {
    constexpr int PIPES = 2; constexpr int COMMA_SPACE = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0; i < priority_queue->size; i++) {
        length += priority_queue->to_string(priority_queue->elements[i], nullptr, 0);

        if (i == 0) length += 1; // space after opening pipe
        if (i < priority_queue->size - 1) length += COMMA_SPACE; // prevent ", " on the last element
        if (i == priority_queue->size - 1) length += 1; // space before closing pipe
    }
    return length + PIPES + NULL_TERMINATOR;
}

static bool ensure_capacity(PriorityQueue* priority_queue, int capacity) {
    if (priority_queue->capacity >= capacity) {
        return true;
    }
    int new_capacity = priority_queue->capacity;
    while (new_capacity < capacity) {
        if (priority_queue->growth_factor > MAX_CAPACITY / new_capacity) {
            return false;
        }
        new_capacity *= priority_queue->growth_factor;
    }
    void** elements = priority_queue->memory_alloc(new_capacity * sizeof(void*));
    if (!elements) {
        return false;
    }
    memcpy(elements, priority_queue->elements, priority_queue->size * sizeof(void*));
    priority_queue->memory_dealloc(priority_queue->elements);
    priority_queue->elements = elements;
    priority_queue->capacity = new_capacity;
    return true;
}

static void heapify_after_insert(PriorityQueue* priority_queue, int index) {
    int parent_index = index > 0 ? (index - 1) / 2 : 0;

    const void* parent = priority_queue->elements[parent_index];
    const void* child = priority_queue->elements[index];

    while (index != 0 && priority_queue->compare(child, parent) > 0) {
        void* temporary = priority_queue->elements[index];
        priority_queue->elements[index] = priority_queue->elements[parent_index];
        priority_queue->elements[parent_index] = temporary;

        index = parent_index;
        parent_index = index > 0 ? (index - 1) / 2 : 0;

        parent = priority_queue->elements[parent_index];
        child = priority_queue->elements[index];
    }
}

static void heapify_after_delete(PriorityQueue* priority_queue, int index) {
    if (priority_queue->size <= 1) {
        return;
    }
    const int left_index = 2 * index + 1;
    const int right_index = 2 * index + 2;
    const int old_index = index;

    const void* left = priority_queue->elements[left_index];
    const void* right = priority_queue->elements[right_index];
    const void* current = priority_queue->elements[index];

    if (left_index < priority_queue->size && priority_queue->compare(current, left) < 0) {
        index = left_index;
        current = left;
    }
    if (right_index < priority_queue->size && priority_queue->compare(current, right) < 0) {
        index = right_index;
    }
    if (index != old_index) {
        void* element = priority_queue->elements[old_index];
        priority_queue->elements[old_index] = priority_queue->elements[index];
        priority_queue->elements[index] = element;

        heapify_after_delete(priority_queue, index);
    }
}

typedef struct {
    Iterator iterator;
    PriorityQueue* priority_queue;
    int cursor;
    int modification_count;
}  IterationContext;

static Iterator* create_iterator(const PriorityQueue* priority_queue) {
    IterationContext* iteration_context = priority_queue->memory_alloc(sizeof(IterationContext));

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
    iteration_context->iterator.memory_dealloc = priority_queue->memory_dealloc;

    iteration_context->priority_queue = (PriorityQueue*) priority_queue;
    iteration_context->cursor = 0;
    iteration_context->modification_count = priority_queue->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->cursor < iteration_context->priority_queue->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->priority_queue->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    return iteration_context->priority_queue->elements[iteration_context->cursor++];
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "priority queue iterators doesn't support backward traversal");
    return false;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "priority queue iterators doesn't support backward traversal");
    return nullptr;
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "priority queue iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "priority queue iterators doesn't support setting elements");
}

static void iterator_remove_internal(void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "priority queue iterators doesn't support removing elements");
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->cursor = 0;
    iteration_context->modification_count = iteration_context->priority_queue->modification_count;
}

static int collection_size_internal(const void* priority_queue) {
    return priority_queue_size(priority_queue);
}

static Iterator* collection_iterator_internal(const void* priority_queue) {
    return priority_queue_iterator(priority_queue);
}

static bool collection_contains_internal(const void* priority_queue, const void* elements) {
    return priority_queue_contains(priority_queue, elements);
}