#ifndef QUEUE_H
#define QUEUE_H

#include "deque/deque.h"

typedef Deque Queue;

typedef DequeOptions QueueOptions;

#define DEFAULT_QUEUE_OPTIONS DEFAULT_DEQUE_OPTIONS

static inline Queue* queue_new(const QueueOptions* options) {
    return deque_new(options);
}

static inline Queue* queue_from(Collection collection, const QueueOptions* options) {
    return deque_from(collection, options);
}

static inline void queue_destroy(Queue** queue) {
    deque_destroy(queue);
}

static inline void queue_set_destructor(Queue* queue, void (*destructor)(void*)) {
    deque_set_destructor(queue, destructor);
}

static inline void queue_enqueue(Queue* queue, const void* element) {
    deque_add_last(queue, element);
}

static inline void queue_enqueue_all(Queue* queue, Collection collection) {
    deque_add_all_last(queue, collection);
}

static inline void* queue_peek(const Queue* queue) {
    return deque_get_first(queue);
}

static inline void* queue_dequeue(Queue* queue) {
    return deque_remove_first(queue);
}

static inline int queue_size(const Queue* queue) {
    return deque_size(queue);
}

static inline bool queue_is_empty(const Queue* queue) {
    return deque_is_empty(queue);
}

static inline Iterator* queue_iterator(const Queue* queue) {
    return deque_iterator(queue);
}

static inline bool queue_equals(const Queue* queue, const Queue* other) {
    return deque_equals(queue, other);
}

static inline void queue_for_each(Queue* queue, Consumer action) {
    deque_for_each(queue, action);
}

static inline void queue_clear(Queue* queue) {
    deque_clear(queue);
}

static inline bool queue_contains(const Queue* queue, const void* element) {
    return deque_contains(queue, element);
}

static inline bool queue_contains_all(const Queue* queue, Collection collection) {
    return deque_contains_all(queue, collection);
}

static inline Queue* queue_clone(const Queue* queue) {
    return deque_clone(queue);
}

static inline Collection queue_to_collection(const Queue* queue) {
    return deque_to_collection(queue);
}

static inline void** queue_to_array(const Queue* queue) {
    return deque_to_array(queue);
}

static inline char* queue_to_string(const Queue* queue) {
    return deque_to_string(queue);
}

#endif