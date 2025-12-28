#ifndef ITERATOR_H
#define ITERATOR_H

typedef struct Iterator Iterator;

Iterator* iterator_new(
    const void* iterable_structure,
    void* internal_state,
    bool (*has_next)(const void* iterable_structure, void* internal_state),
    void* (*next)(const void* iterable_structure, void* internal_state),
    void (*reset)(void* internal_state)
);

bool iterator_has_next(Iterator*);

void* iterator_next(Iterator*);

void iterator_reset(Iterator*);

void iterator_delete(Iterator*);

#endif