#ifndef ITERATOR_H
#define ITERATOR_H

typedef struct Iterator Iterator;

#define iterator_from(iterable_structure, internal_state, has_next, next, reset)    \
    iterator_new(iterable_structure, internal_state,                                \
        (bool (*)(const void*, void*)) has_next,                                    \
        (void* (*)(const void*, void*)) next,                                       \
        (void (*)(void*)) reset                                                     \
    )

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