#ifndef ITERATOR_H
#define ITERATOR_H

typedef struct Iterator Iterator;

Iterator* iterator_new(
    void* iterable_structure,
    void* internal_state,
    bool (*has_next)(void* iterable_structure, void* internal_state),
    void* (*next)(void* iterable_structure, void* internal_state)
);

bool iterator_has_next(Iterator*);

void* iterator_next(Iterator*);

void iterator_delete(Iterator*);

#endif