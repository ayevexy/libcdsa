#include "iterator.h"

#include <stdlib.h>

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
    Iterator* iterator = malloc(sizeof(Iterator));
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
    free(iterator->internal_state);
    free(iterator);
}
