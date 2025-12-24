#include "iterator.h"

#include "internal/memory.h"

struct Iterator {
    void* iterable_structure;
    void* internal_state;
    bool (*has_next)(void* iterable_structure, void* internal_state);
    void* (*next)(void* iterable_structure, void* internal_state);
};

Iterator* iterator_new(
    void* iterable_structure,
    void* internal_state,
    bool (*has_next)(void* iterable_structure, void* internal_state),
    void* (*next)(void* iterable_structure, void* internal_state)
) {
    Iterator* iterator = memory_alloc(sizeof(Iterator));
    iterator->iterable_structure = iterable_structure;
    iterator->internal_state = internal_state;
    iterator->has_next = has_next;
    iterator->next = next;
    return iterator;
}

bool iterator_has_next(Iterator* iterator) {
    return iterator->has_next(iterator->iterable_structure, iterator->internal_state);
}

void *iterator_next(Iterator* iterator) {
    return iterator->next(iterator->iterable_structure, iterator->internal_state);
}

void iterator_delete(Iterator* iterator) {
    memory_free(&iterator->internal_state);
    memory_free((void**) &iterator);
}
