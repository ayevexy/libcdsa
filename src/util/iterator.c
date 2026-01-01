#include "iterator.h"

#include "error.h"

struct Iterator {
    const void* iterable_structure;
    void* internal_state;
    bool (*has_next)(const void* iterable_structure, void* internal_state);
    void* (*next)(const void* iterable_structure, void* internal_state);
    void (*reset)(void* internal_state);
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
};

Iterator* iterator_new(
    const void* iterable_structure,
    void* internal_state,
    bool (*has_next)(const void* iterable_structure, void* internal_state),
    void* (*next)(const void* iterable_structure, void* internal_state),
    void (*reset)(void* internal_state),
    void* (*memory_alloc)(size_t),
    void (*memory_free)(void*)
) {
    Iterator* iterator = memory_alloc(sizeof(Iterator));
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "Error at %s(): memory allocation failed", __func__);
        return nullptr;
    }
    iterator->iterable_structure = iterable_structure;
    iterator->internal_state = internal_state;
    iterator->has_next = has_next;
    iterator->next = next;
    iterator->reset = reset;
    iterator->memory_alloc = memory_alloc;
    iterator->memory_free = memory_free;
    return iterator;
}

bool iterator_has_next(Iterator* iterator) {
    return iterator->has_next(iterator->iterable_structure, iterator->internal_state);
}

void *iterator_next(Iterator* iterator) {
    return iterator->next(iterator->iterable_structure, iterator->internal_state);
}

void iterator_reset(Iterator* iterator) {
    iterator->reset(iterator->internal_state);
}

void iterator_delete(Iterator* iterator) {
    iterator->memory_free(iterator->internal_state);
    iterator->memory_free(iterator);
}
