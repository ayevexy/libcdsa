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
    Comparator compare;
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
        || options->growth_factor < MIN_GROWTH_FACTOR || !options->destruct || !options->equals
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
    priority_queue->compare = options->compare;
    priority_queue->destruct = options->destruct;
    priority_queue->equals = options->equals;
    priority_queue->to_string = options->to_string;
    priority_queue->memory_alloc = options->memory_alloc;
    priority_queue->memory_dealloc = options->memory_dealloc;
    priority_queue->modification_count = 0;
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

void priority_queue_set_destructor(PriorityQueue* priority_queue, void (*destruct)(void*)) {
    if (require_non_null(priority_queue, destruct)) return;
    priority_queue->destruct = destruct;
}

void priority_queue_enqueue(PriorityQueue* priority_queue, const void* element) {
    if (require_non_null(priority_queue)) return;
    
    // TODO: ensure capacity

    priority_queue->elements[priority_queue->size] = (void*) element;

    priority_queue->size++;
    priority_queue->modification_count++;

    heapify_after_insert(priority_queue, priority_queue->size - 1);
}

void* priority_queue_dequeue(PriorityQueue* priority_queue) {
    if (require_non_null(priority_queue)) return nullptr;

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

bool priority_queue_contains(const PriorityQueue* priority_queue, const void* element) {
    (void) priority_queue, (void) element;
    return false;
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