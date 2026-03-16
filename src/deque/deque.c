#include "deque.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>
#include <limits.h>

constexpr int MIN_CAPACITY = 8;
constexpr int MAX_CAPACITY = (INT_MAX - 1);
//constexpr float GROWTH_FACTOR = 2;

struct Deque {
    void** elements;
    int first;
    int last;
    int size;
    int capacity;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
    int modification_count;
};

static int next_power_of_two(int);

Deque* deque_new(const DequeOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || !options->destruct || !options->equals || !options->to_string
        || !options->memory_alloc || !options->memory_realloc || !options->memory_free
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    Deque* deque = options->memory_alloc(sizeof(Deque));
    if (!deque) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'deque'");
        return nullptr;
    }
    deque->capacity = next_power_of_two(options->initial_capacity);
    deque->elements = options->memory_alloc(deque->capacity * sizeof(void*));
    if (!deque->elements) {
        options->memory_free(deque);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'deque'");
        return nullptr;
    }
    deque->first = 0;
    deque->last = 0;
    deque->size = 0;
    deque->destruct = options->destruct;
    deque->equals = options->equals;
    deque->to_string = options->to_string;
    deque->memory_alloc = options->memory_alloc;
    deque->memory_realloc = options->memory_realloc;
    deque->memory_free = options->memory_free;
    deque->modification_count = 0;
    return deque;
}

void deque_destroy(Deque** deque_pointer) {
    if (require_non_null(deque_pointer, *deque_pointer)) return;
    Deque* deque = *deque_pointer;
    for (int i = 0; i < deque->size; i++) {
        deque->destruct(deque->elements[i]);
    }
    deque->memory_free(deque->elements);
    deque->memory_free(deque);
    *deque_pointer = nullptr;
}

void deque_set_destructor(Deque* deque, void (*destructor)(void*)) {
    if (require_non_null(deque, destructor)) return;
    deque->destruct = destructor;
}

void deque_add_first(Deque* deque, const void* element) {
    if (require_non_null(deque)) return;
    deque->first = (deque->first - 1) & (deque->capacity - 1);
    deque->elements[deque->first] = (void*) element;
    deque->size++;
    deque->modification_count++;
}

void deque_add_last(Deque* deque, const void* element) {
    if (require_non_null(deque)) return;
    deque->elements[deque->last] = (void*) element;
    deque->last = (deque->last + 1) & (deque->capacity - 1);
    deque->size++;
    deque->modification_count++;
}

int deque_size(const Deque* deque) {
    if (require_non_null(deque)) return 0;
    return deque->size;
}

bool deque_contains(const Deque* deque, const void* element) {
    if (require_non_null(deque)) return false;
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        if (deque->equals(deque->elements[index], element)) {
            return true;
        }
    }
    return false;
}

static int next_power_of_two(int x) {
    int power = 1;
    while (power < x) {
        power <<= 1;
    }
    return power;
}