#include "iterator.h"

#include "errors.h"
#include "constraints.h"

struct Iterator {
    void* internal_state;
    bool (*has_next)(const void* internal_state);
    void* (*next)(void* internal_state);
    void (*reset)(void* internal_state);
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
};

Iterator* iterator_new(
    void* internal_state,
    bool (*has_next)(const void* internal_state),
    void* (*next)(void* internal_state),
    void (*reset)(void* internal_state),
    void* (*memory_alloc)(size_t),
    void (*memory_free)(void*)
) {
    require_non_null(internal_state, has_next, next, reset, memory_alloc, memory_free);
    Iterator* iterator = memory_alloc(sizeof(Iterator));
    if (!iterator) {
        return nullptr;
    }
    iterator->internal_state = internal_state;
    iterator->has_next = has_next;
    iterator->next = next;
    iterator->reset = reset;
    iterator->memory_alloc = memory_alloc;
    iterator->memory_free = memory_free;
    return iterator;
}

bool iterator_has_next(const Iterator* iterator) {
    require_non_null(iterator);
    return iterator->has_next(iterator->internal_state);
}

void* iterator_next(Iterator* iterator) {
    require_non_null(iterator);
    return iterator->next(iterator->internal_state);
}

void iterator_reset(Iterator* iterator) {
    require_non_null(iterator);
    iterator->reset(iterator->internal_state);
}

void iterator_delete(Iterator** iterator_pointer) {
    require_non_null(iterator_pointer);
    if (!*iterator_pointer) {
        set_error(NULL_POINTER_ERROR, "'iterator' must not be null");
        return;
    }
    Iterator* iterator = *iterator_pointer;
    iterator->memory_free(iterator->internal_state);
    iterator->memory_free(iterator);
    *iterator_pointer = nullptr;
}
