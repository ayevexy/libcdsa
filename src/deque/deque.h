#ifndef DEQUE_H
#define DEQUE_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

typedef struct Deque Deque;

typedef struct {
    int initial_capacity;
    float growth_factor;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
} DequeOptions;

#define DEFAULT_DEQUE_OPTIONS(...) &(DequeOptions) {    \
    .initial_capacity = 10,                             \
    .growth_factor = 2.0f,                              \
    .destruct = noop_destruct,                          \
    .equals = pointer_equals,                           \
    .to_string = pointer_to_string,                     \
    .memory_alloc = malloc,                             \
    .memory_realloc = realloc,                          \
    .memory_free = free,                                \
    __VA_ARGS__                                         \
}

Deque* deque_new(const DequeOptions* options);

Deque* deque_from(Collection collection, const DequeOptions* options);

void deque_destroy(Deque** deque_pointer);

void (*deque_get_destructor(const Deque* deque))(void*);

void deque_set_destructor(Deque* deque, void (*destructor)(void*));

void deque_add_first(Deque* deque, const void* element);

void deque_add_last(Deque* deque, const void* element);

void deque_add_all_first(Deque* deque, Collection collection);

void deque_add_all_last(Deque* deque, Collection collection);

void* deque_get_first(const Deque* deque);

void* deque_get_last(const Deque* deque);

void* deque_set_first(const Deque* deque, const void* element);

void* deque_set_last(const Deque* deque, const void* element);

void* deque_remove_first(Deque* deque);

void* deque_remove_last(Deque* deque);

int deque_size(const Deque* deque);

bool deque_is_empty(const Deque* deque);

Iterator* deque_iterator(const Deque* deque);

bool deque_equals(const Deque* deque, const Deque* other_deque);

void deque_for_each(Deque* deque, Consumer action);

void deque_clear(Deque* deque);

bool deque_contains(const Deque* deque, const void* element);

bool deque_contains_all(const Deque* deque, Collection collection);

Deque* deque_clone(const Deque* deque);

Collection deque_to_collection(const Deque* deque);

void** deque_to_array(const Deque* deque);

char* deque_to_string(const Deque* deque);

#endif