#ifndef STACK_H
#define STACK_H

#include "deque/deque.h"

typedef Deque Stack;

typedef DequeOptions StackOptions;

#define DEFAULT_STACK_OPTIONS DEFAULT_DEQUE_OPTIONS

static inline Stack* stack_new(const StackOptions* options) {
    return deque_new(options);
}

static inline Stack* stack_from(Collection collection, const StackOptions* options) {
    return deque_from(collection, options);
}

static inline void stack_destroy(Stack** stack) {
    deque_destroy(stack);
}

static inline void stack_set_destructor(Stack* stack, void (*destructor)(void*)) {
    deque_set_destructor(stack, destructor);
}

static inline void stack_push(Stack* stack, const void* element) {
    deque_add_last(stack, element);
}

static inline void stack_push_all(Stack* stack, Collection collection) {
    deque_add_all_last(stack, collection);
}

static inline void* stack_peek(const Stack* stack) {
    return deque_get_last(stack);
}

static inline void* stack_pop(Stack* stack) {
    return deque_remove_last(stack);
}

static inline int stack_size(const Stack* stack) {
    return deque_size(stack);
}

static inline bool stack_is_empty(const Stack* stack) {
    return deque_is_empty(stack);
}

static inline Iterator* stack_iterator(const Stack* stack) {
    return deque_iterator(stack);
}

static inline bool stack_equals(const Stack* stack, const Stack* other) {
    return deque_equals(stack, other);
}

static inline void stack_for_each(Stack* stack, Consumer action) {
    deque_for_each(stack, action);
}

static inline void stack_clear(Stack* stack) {
    deque_clear(stack);
}

static inline bool stack_contains(const Stack* stack, const void* element) {
    return deque_contains(stack, element);
}

static inline bool stack_contains_all(const Stack* stack, Collection collection) {
    return deque_contains_all(stack, collection);
}

static inline Stack* stack_clone(const Stack* stack) {
    return deque_clone(stack);
}

static inline Collection stack_to_collection(const Stack* stack) {
    return deque_to_collection(stack);
}

static inline void** stack_to_array(const Stack* stack) {
    return deque_to_array(stack);
}

static inline char* stack_to_string(const Stack* stack) {
    return deque_to_string(stack);
}

#endif