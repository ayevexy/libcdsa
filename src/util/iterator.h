#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

typedef struct Iterator Iterator;

#define iterator_from(iterable_structure, internal_state, has_next, next, reset)    \
    iterator_new(internal_state,                                                    \
        (bool (*)(const void*)) has_next,                                           \
        (void* (*)(void*)) next,                                                    \
        (void (*)(void*)) reset,                                                    \
        iterable_structure->memory_alloc,                                           \
        iterable_structure->memory_free                                             \
    )

Iterator* iterator_new(
    void* internal_state,
    bool (*has_next)(const void* internal_state),
    void* (*next)(void* internal_state),
    void (*reset)(void* internal_state),
    void* (*memory_alloc)(size_t),
    void (*memory_free)(void*)
);

bool iterator_has_next(const Iterator*);

void* iterator_next(Iterator*);

void iterator_reset(Iterator*);

void iterator_delete(Iterator**);

#endif