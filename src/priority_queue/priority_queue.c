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