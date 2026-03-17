#include "deque.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>
#include <limits.h>

constexpr int MIN_CAPACITY = 8;
constexpr int MAX_CAPACITY = (INT_MAX - 1);
constexpr float GROWTH_FACTOR = 2;

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
        void (*memory_free)(void*);
    };
    int modification_count;
};

static int next_power_of_two(int);

static bool ensure_capacity(Deque*, int);

static Iterator* create_iterator(const Deque*);

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static void iterator_reset_internal(void*);

static int collection_size_internal(const void*);

static Iterator* collection_iterator_internal(const void*);

static bool collection_contains_internal(const void*, const void*);

Deque* deque_new(const DequeOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY || !options->destruct
        || !options->equals || !options->to_string || !options->memory_alloc || !options->memory_free
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
    deque->memory_free = options->memory_free;
    deque->modification_count = 0;
    return deque;
}

Deque* deque_from(Collection collection, const DequeOptions* options) {
    if (require_non_null(options)) return nullptr;
    Deque* deque; Error error;

    if ((error = attempt(deque = deque_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(deque_add_all_last(deque, collection)))) {
        deque_destroy(&deque);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
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
    if (!ensure_capacity(deque, deque->size + 1)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'deque' capacity");
        return;
    }
    deque->first = (deque->first - 1) & (deque->capacity - 1);
    deque->elements[deque->first] = (void*) element;
    deque->size++;
    deque->modification_count++;
}

void deque_add_last(Deque* deque, const void* element) {
    if (require_non_null(deque)) return;
    if (!ensure_capacity(deque, deque->size + 1)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'deque' capacity");
        return;
    }
    deque->elements[deque->last] = (void*) element;
    deque->last = (deque->last + 1) & (deque->capacity - 1);
    deque->size++;
    deque->modification_count++;
}

void deque_add_all_first(Deque* deque, Collection collection) {
    if (require_non_null(deque)) return;
    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    if (!ensure_capacity(deque, deque->size + collection_size(collection))) {
        iterator_destroy(&iterator);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'deque' capacity");
        return;
    }
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        deque_add_first(deque, element);
    }
    iterator_destroy(&iterator);
}

void deque_add_all_last(Deque* deque, Collection collection) {
    if (require_non_null(deque)) return;
    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    if (!ensure_capacity(deque, deque->size + collection_size(collection))) {
        iterator_destroy(&iterator);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'deque' capacity");
        return;
    }
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        deque_add_last(deque, element);
    }
    iterator_destroy(&iterator);
}

void* deque_get_first(const Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    if (deque->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'deque' is empty");
        return nullptr;
    }
    return deque->elements[deque->first];
}

void* deque_get_last(const Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    if (deque->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'deque' is empty");
        return nullptr;
    }
    return deque->elements[(deque->last - 1) & (deque->capacity - 1)];
}

void* deque_remove_first(Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    if (deque->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'deque' is empty");
        return nullptr;
    }
    void *element = deque->elements[deque->first];
    deque->destruct(element);
    deque->elements[deque->first] = nullptr;

    deque->first = (deque->first + 1) & (deque->capacity - 1);
    deque->size--;
    deque->modification_count++;
    return element;
}

void* deque_remove_last(Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    if (deque->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'deque' is empty");
        return nullptr;
    }
    const int last = (deque->last - 1) & (deque->capacity - 1);

    void *element = deque->elements[last];
    deque->destruct(element);
    deque->elements[deque->last] = nullptr;

    deque->last = last;
    deque->size--;
    deque->modification_count++;
    return element;
}

int deque_size(const Deque* deque) {
    if (require_non_null(deque)) return 0;
    return deque->size;
}

int deque_capacity(const Deque* deque) {
    if (require_non_null(deque)) return 0;
    return deque->capacity;
}

bool deque_is_empty(const Deque* deque) {
    if (require_non_null(deque)) return false;
    return deque->size == 0;
}

Iterator* deque_iterator(const Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    Iterator* iterator = create_iterator(deque);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
    }
    return iterator;
}

bool deque_equals(const Deque* deque, const Deque* other_deque) {
    if (require_non_null(deque, other_deque)) return false;
    if (deque == other_deque) {
        return true;
    }
    if (deque->size != other_deque->size) {
        return false;
    }
    for (int i = 0; i < deque->size; i++) {
        const int index_a = (deque->first + i) & (deque->capacity - 1);
        const int index_b = (other_deque->first + i) & (other_deque->capacity - 1);
        if (!deque->equals(deque->elements[index_a], other_deque->elements[index_b])) {
            return false;
        }
    }
    return true;
}

void deque_for_each(Deque* deque, Consumer action) {
    if (require_non_null(deque, action)) return;
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        action(deque->elements[index]);
    }
}

void deque_clear(Deque* deque) {
    if (require_non_null(deque)) return;
    for (int i = 0; i < deque->size; i++) {
        deque->destruct(deque->elements[i]);
        deque->elements[i] = nullptr;
    }
    deque->first = 0;
    deque->last = 0;
    deque->size = 0;
    deque->modification_count++;
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

bool deque_contains_all(const Deque* deque, Collection collection) {
    if (require_non_null(deque)) return false;

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!deque_contains(deque, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

Deque* deque_clone(const Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    Deque* new_deque; const Error error = attempt(new_deque = deque_new(&(DequeOptions) {
         .initial_capacity = deque->capacity,
         .destruct = noop_destruct,
         .equals = deque->equals,
         .to_string = deque->to_string,
         .memory_alloc = deque->memory_alloc,
         .memory_free = deque->memory_free
    }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = 0; i < deque->size; i++) {
        const void* element = deque->elements[(deque->first + i) & (deque->capacity - 1)];
        deque_add_last(new_deque, element);
    }
    return new_deque;
}

Collection deque_to_collection(const Deque* deque) {
    if (require_non_null(deque)) return (Collection) {};
    return (Collection) {
        .data_structure = deque,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
}

void** deque_to_array(const Deque* deque) {
    if (require_non_null(deque)) return nullptr;
    void** elements = deque->memory_alloc(sizeof(void*) * deque->size);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "no additional details available");
        return nullptr;
    }
    for (int i = 0; i < deque->size; i++) {
        elements[i] = deque->elements[(deque->first + i) & (deque->capacity - 1)];
    }
    return elements;
}

static int next_power_of_two(int x) {
    int power = 1;
    while (power < x) {
        power <<= 1;
    }
    return power;
}

static bool ensure_capacity(Deque* deque, int capacity) {
    if (deque->capacity >= capacity) {
        return true;
    }
    int new_capacity = deque->capacity;
    while (new_capacity < capacity) {
        new_capacity = (int) (new_capacity * GROWTH_FACTOR);
    }
    new_capacity = new_capacity > MAX_CAPACITY ? MAX_CAPACITY : new_capacity;

    void** elements = deque->memory_alloc(new_capacity * sizeof(void*));
    if (!elements) {
        return false;
    }
    for (int i = 0; i < deque->size; i++) {
        elements[i] = deque->elements[(deque->first + i) & (deque->capacity - 1)];
    }
    deque->memory_free(deque->elements);
    deque->elements = elements;
    deque->first = 0;
    deque->last = deque->size;
    deque->capacity = new_capacity;
    return true;
}

typedef struct {
    Iterator iterator;
    Deque* deque;
    int count;
    int modification_count;
}  IterationContext;

static Iterator* create_iterator(const Deque* deque) {
    IterationContext* iteration_context = deque->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator.iteration_context = iteration_context;
    iteration_context->iterator.has_next = iterator_has_next_internal;
    iteration_context->iterator.next = iterator_next_internal;
    iteration_context->iterator.reset = iterator_reset_internal;
    iteration_context->iterator.memory_free = deque->memory_free;

    iteration_context->deque = (Deque*) deque;
    iteration_context->count = 0;
    iteration_context->modification_count = deque->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count < iteration_context->deque->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->deque->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection was modified while this iterator still alive");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    const int capacity = (iteration_context->deque->capacity - 1);
    const int index = (iteration_context->deque->first + iteration_context->count) & capacity;
    iteration_context->count++;
    return iteration_context->deque->elements[index];
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->count = 0;
    iteration_context->modification_count = iteration_context->deque->modification_count;
}

static int collection_size_internal(const void* deque) {
    return deque_size(deque);
}

static Iterator* collection_iterator_internal(const void* deque) {
    return deque_iterator(deque);
}

static bool collection_contains_internal(const void* deque, const void* element) {
    return deque_contains(deque, element);
}