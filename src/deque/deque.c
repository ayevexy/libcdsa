#include "deque.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

constexpr int MIN_CAPACITY = 8;
constexpr int MAX_CAPACITY = 1'073'741'824;
constexpr int GROWTH_FACTOR = 2;

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
        void (*memory_dealloc)(void*);
    };
    int modification_count;
};

static size_t calculate_string_size(const Deque*);

static int next_power_of_two(int);

static bool ensure_capacity(Deque*, int);

static Iterator* create_iterator(const Deque*, int);

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

Deque* deque_new(const DequeOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY || !options->destruct
        || !options->equals || !options->to_string || !options->memory_alloc || !options->memory_dealloc
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
        options->memory_dealloc(deque);
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
    deque->memory_dealloc = options->memory_dealloc;
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
        const int index = (deque->first + i) & (deque->capacity - 1);
        deque->destruct(deque->elements[index]);
    }
    deque->memory_dealloc(deque->elements);
    deque->memory_dealloc(deque);
    *deque_pointer = nullptr;
}

void deque_change_destructor(Deque* deque, void (*destruct)(void*)) {
    if (require_non_null(deque, destruct)) return;
    deque->destruct = destruct;
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
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
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
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
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
    void* element = deque->elements[deque->first];
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

    void* element = deque->elements[last];
    deque->destruct(element);
    deque->elements[last] = nullptr;

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
    Iterator* iterator = create_iterator(deque, 0);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

Iterator* deque_iterator_at(const Deque* deque, int position) {
    if (require_non_null(deque)) return nullptr;
    if (position < 0 || position > deque->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "position %d out of bounds for size %d", position, deque->size);
        return nullptr;
    }
    Iterator* iterator = create_iterator(deque, position);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
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

void deque_reverse(Deque* deque) {
    if (require_non_null(deque)) return;
    for (int i = 0; i < deque->size / 2; i++) {
        const int index_a = (deque->first + i) & (deque->capacity - 1);
        const int index_b = (deque->first + deque->size - 1 - i) & (deque->capacity - 1);

        void* temporary = deque->elements[index_a];
        deque->elements[index_a] = deque->elements[index_b];
        deque->elements[index_b] = temporary;
    }
}

void deque_clear(Deque* deque) {
    if (require_non_null(deque)) return;
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        deque->destruct(deque->elements[index]);
        deque->elements[index] = nullptr;
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
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
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
    Deque* new_deque; Error error;

    if ((error = attempt(new_deque = deque_new(&(DequeOptions) {
         .initial_capacity = deque->capacity,
         .destruct = noop_destruct,
         .equals = deque->equals,
         .to_string = deque->to_string,
         .memory_alloc = deque->memory_alloc,
         .memory_dealloc = deque->memory_dealloc
    })))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        deque_add_last(new_deque, deque->elements[index]);
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
    void** elements = deque->memory_alloc(deque->size * sizeof(void*));
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        elements[i] = deque->elements[index];
    }
    return elements;
}

StringOwned deque_to_string(const Deque* deque) {
    if (require_non_null(deque)) return string_null();

    char* raw_string = deque->memory_alloc(calculate_string_size(deque));
    if (!raw_string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
        return string_null();
    }
    raw_string[0] = '\0'; // initialize string to ignore memory garbage
    strcat(raw_string, deque->size == 0 ? "|" : "| ");

    for (int i = 0; i < deque->size; i++) {
        constexpr int NULL_TERMINATOR = 1;
        const int index = (deque->first + i) & (deque->capacity - 1);
        const int length = deque->to_string(deque->elements[index], nullptr, 0) + NULL_TERMINATOR;

        char* raw_element_string = deque->memory_alloc(length);
        if (!raw_element_string) {
            deque->memory_dealloc(raw_string);
            set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
            return string_null();
        }
        deque->to_string(deque->elements[index], raw_element_string, length);
        strcat(raw_string, raw_element_string);

        if (i < deque->size - 1) {
            strcat(raw_string, ", ");
        }
        deque->memory_dealloc(raw_element_string);
    }

    strcat(raw_string, deque->size == 0 ? "|" : " |");
    return string_view(raw_string);
}

static size_t calculate_string_size(const Deque* deque) {
    constexpr int PIPES = 2; constexpr int COMMA_SPACE = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        length += deque->to_string(deque->elements[index], nullptr, 0);

        if (i == 0) length += 1; // space after opening pipe
        if (i < deque->size - 1) length += COMMA_SPACE; // prevent ", " on the last element
        if (i == deque->size - 1) length += 1; // space before closing pipe
    }
    return length + PIPES + NULL_TERMINATOR;
}

static int next_power_of_two(int x) {
    int power = 1;
    while (power < x) {
        power *= 2;
    }
    return power;
}

static bool ensure_capacity(Deque* deque, int capacity) {
    if (deque->capacity >= capacity) {
        return true;
    }
    int new_capacity = deque->capacity;
    while (new_capacity < capacity) {
        if (GROWTH_FACTOR > MAX_CAPACITY / new_capacity) {
            return false;
        }
        new_capacity *= GROWTH_FACTOR;
    }
    void** elements = deque->memory_alloc(new_capacity * sizeof(void*));
    if (!elements) {
        return false;
    }
    for (int i = 0; i < deque->size; i++) {
        const int index = (deque->first + i) & (deque->capacity - 1);
        elements[i] = deque->elements[index];
    }
    deque->memory_dealloc(deque->elements);
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

static Iterator* create_iterator(const Deque* deque, int position) {
    IterationContext* iteration_context = deque->memory_alloc(sizeof(IterationContext));

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
    iteration_context->iterator.memory_dealloc = deque->memory_dealloc;

    iteration_context->deque = (Deque*) deque;
    iteration_context->count = position;
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
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    const int capacity = (iteration_context->deque->capacity - 1);
    const int index = (iteration_context->deque->first + iteration_context->count++) & capacity;
    return iteration_context->deque->elements[index];
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->count > 0;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->deque->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return nullptr;
    }
    if (!iterator_has_previous_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    const int capacity = (iteration_context->deque->capacity - 1);
    const int index = (iteration_context->deque->first + --iteration_context->count) & capacity;
    return iteration_context->deque->elements[index];
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "deque iterators doesn't support adding elements");
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    (void) raw_iteration_context, (void) element;
    set_error(UNSUPPORTED_OPERATION_ERROR, "deque iterators doesn't support setting elements");
}

static void iterator_remove_internal(void* raw_iteration_context) {
    (void) raw_iteration_context;
    set_error(UNSUPPORTED_OPERATION_ERROR, "deque iterators doesn't support removing elements");
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