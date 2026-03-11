#include "sets.h"

#include "util/pair.h"
#include "util/constraints.h"
#include "util/errors.h"

#include <stdlib.h>
#include <assert.h>

typedef enum {
    READY,
    NOT_READY,
    DONE,
    FAILED,
} IterationState;

typedef struct IterationContext {
    Iterator iterator;
    Pair sets;
    Pair set_iterators;
    IterationState state;
    void* next_element;
    void* (*internal_next)(void*);
} IterationContext;

static bool internal_iterator_compute_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->state = FAILED;
    iteration_context->next_element = iteration_context->internal_next(iteration_context);
    if (iteration_context->state != DONE) {
        iteration_context->state = READY;
        return true;
    }
    return false;
}

static bool internal_iterator_has_next(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    assert(iteration_context->state != FAILED);
    switch (iteration_context->state) {
        case READY: return true;
        case DONE: return false;
        default: ;
    }
    return internal_iterator_compute_next((void*) iteration_context);
}

static void* internal_iterator_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (!internal_iterator_has_next(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    iteration_context->state = NOT_READY;
    return iteration_context->next_element;
}

static void internal_iterator_reset(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;

    iterator_destroy((Iterator**) &iteration_context->set_iterators.first);
    iterator_destroy((Iterator**) &iteration_context->set_iterators.second);

    iteration_context->set_iterators = (Pair) {
        set_view_iterator(*(SetView*) iteration_context->sets.first),
        set_view_iterator(*(SetView*) iteration_context->sets.second)
    };
    iteration_context->state = NOT_READY;
    iteration_context->next_element = nullptr;
}

static void iteration_context_destroy(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iterator_destroy((Iterator**) &iteration_context->set_iterators.first);
    iterator_destroy((Iterator**) &iteration_context->set_iterators.second);
    free(iteration_context);
}

static Iterator* internal_iterator_new(const void* raw_sets, void* (*internal_next)(void*)) {
    const Pair* sets = raw_sets;
    SetView* set_a = sets->first, * set_b = sets->second;

    IterationContext* iteration_context = malloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator.iteration_context = iteration_context;
    iteration_context->iterator.has_next = internal_iterator_has_next;
    iteration_context->iterator.next = internal_iterator_next;
    iteration_context->iterator.reset = internal_iterator_reset;
    iteration_context->iterator.memory_free = iteration_context_destroy;
    
    iteration_context->sets = (Pair) { set_a, set_b };
    iteration_context->set_iterators = (Pair) { set_view_iterator(*set_a), set_view_iterator(*set_b) };
    iteration_context->state = NOT_READY;
    iteration_context->internal_next = internal_next;
    iteration_context->next_element = nullptr;

    return &iteration_context->iterator;
}

static int union_set_size(const void* raw_sets) {
    const Pair* sets = raw_sets;
    int size = set_view_size(*(SetView*) sets->first);
    Iterator* iterator_b = set_view_iterator(*(SetView*) sets->second);
    while (iterator_has_next(iterator_b)) {
        const void* element = iterator_next(iterator_b);
        if (!set_view_contains(*(SetView*) sets->first, element)) {
            size++;
        }
    }
    iterator_destroy(&iterator_b);
    return size;
}

static void* union_set_iterator_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iterator_has_next(iteration_context->set_iterators.first)) {
        return iterator_next(iteration_context->set_iterators.first);
    }
    while (iterator_has_next(iteration_context->set_iterators.second)) {
        void* element = iterator_next(iteration_context->set_iterators.second);
        if (!set_view_contains(*(SetView*) iteration_context->sets.first, element)) {
            return element;
        }
    }
    iteration_context->state = DONE;
    return nullptr;
}

static Iterator* union_set_iterator(const void* sets) {
    return internal_iterator_new(sets, union_set_iterator_next);
}

static bool union_set_contains(const void* raw_sets, const void* element) {
    const Pair* sets = raw_sets;
    return set_view_contains(*(SetView*) sets->first, element) || set_view_contains(*(SetView*) sets->second, element);
}

static int intersection_set_size(const void* raw_sets) {
    const Pair* sets = raw_sets;
    int size = 0;
    Iterator* iterator = set_view_iterator(*(SetView*) sets->first);
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (set_view_contains(*(SetView*) sets->second, element)) {
            size++;
        }
    }
    iterator_destroy(&iterator);
    return size;
}

static void* intersection_set_iterator_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    while (iterator_has_next(iteration_context->set_iterators.first)) {
        void* element = iterator_next(iteration_context->set_iterators.first);
        if (set_view_contains(*(SetView*) iteration_context->sets.second, element)) {
            return element;
        }
    }
    iteration_context->state = DONE;
    return nullptr;
}

static Iterator* intersection_set_iterator(const void* sets) {
    return internal_iterator_new(sets, intersection_set_iterator_next);
}

static bool intersection_set_contains(const void* raw_sets, const void* element) {
    const Pair* sets = raw_sets;
    return set_view_contains(*(SetView*) sets->first, element) && set_view_contains(*(SetView*) sets->second, element);
}

static int difference_set_size(const void* raw_sets) {
    const Pair* sets = raw_sets;
    int size = 0;
    Iterator* iterator_a = set_view_iterator(*(SetView*) sets->first);
    while (iterator_has_next(iterator_a)) {
        const void* element = iterator_next(iterator_a);
        if (!set_view_contains(*(SetView*) sets->second, element)) {
            size++;
        }
    }
    iterator_destroy(&iterator_a);
    return size;
}

static void* difference_set_iterator_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    while (iterator_has_next(iteration_context->set_iterators.first)) {
        void* element = iterator_next(iteration_context->set_iterators.first);
        if (!set_view_contains(*(SetView*) iteration_context->sets.second, element)) {
            return element;
        }
    }
    iteration_context->state = DONE;
    return nullptr;
}

static Iterator* difference_set_iterator(const void* sets) {
    return internal_iterator_new(sets, difference_set_iterator_next);
}

static bool difference_set_contains(const void* raw_sets, const void* element) {
    const Pair* sets = raw_sets;
    return set_view_contains(*(SetView*) sets->first, element) && !set_view_contains(*(SetView*) sets->second, element);
}

static int symmetric_difference_set_size(const void* raw_sets) {
    const Pair* sets = raw_sets;
    int size = 0;
    Iterator* iterator_a = set_view_iterator(*(SetView*) sets->first);
    while (iterator_has_next(iterator_a)) {
        const void* element = iterator_next(iterator_a);
        if (!set_view_contains(*(SetView*) sets->second, element)) {
            size++;
        }
    }
    iterator_destroy(&iterator_a);
    Iterator* iterator_b = set_view_iterator(*(SetView*) sets->second);
    while (iterator_has_next(iterator_b)) {
        const void* element = iterator_next(iterator_b);
        if (!set_view_contains(*(SetView*) sets->first, element)) {
            size++;
        }
    }
    iterator_destroy(&iterator_b);
    return size;
}

static void* symmetric_difference_set_iterator_next(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    while (iterator_has_next(iteration_context->set_iterators.first)) {
        void* element = iterator_next(iteration_context->set_iterators.first);
        if (!set_view_contains(*(SetView*) iteration_context->sets.second, element)) {
            return element;
        }
    }
    while (iterator_has_next(iteration_context->set_iterators.second)) {
        void* element = iterator_next(iteration_context->set_iterators.second);
        if (!set_view_contains(*(SetView*) iteration_context->sets.first, element)) {
            return element;
        }
    }
    iteration_context->state = DONE;
    return nullptr;
}

static Iterator* symmetric_difference_set_iterator(const void* sets) {
    return internal_iterator_new(sets, symmetric_difference_set_iterator_next);
}

static bool symmetric_difference_set_contains(const void* raw_sets, const void* element) {
    const Pair* sets = raw_sets;
    return set_view_contains(*(SetView*) sets->first, element) ^ set_view_contains(*(SetView*) sets->second, element);
}

SetView _set_view_union(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return (SetView) {};
    return (SetView) {
        .sets = { set_a, set_b },
        .size = union_set_size,
        .iterator = union_set_iterator,
        .contains = union_set_contains
    };
}

SetView _set_view_intersection(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return (SetView) {};
    return (SetView) {
        .sets = { set_a, set_b },
        .size = intersection_set_size,
        .iterator = intersection_set_iterator,
        .contains = intersection_set_contains
    };
}

SetView _set_view_difference(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return (SetView) {};
    return (SetView) {
        .sets = { set_a, set_b },
        .size = difference_set_size,
        .iterator = difference_set_iterator,
        .contains = difference_set_contains
    };
}

SetView _set_view_symmetric_difference(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return (SetView) {};
    return (SetView) {
        .sets = { set_a, set_b },
        .size = symmetric_difference_set_size,
        .iterator = symmetric_difference_set_iterator,
        .contains = symmetric_difference_set_contains
    };
}

bool _set_view_is_subset(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return false;
    Iterator* iterator = set_view_iterator(*set_a);
    bool subset = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!set_view_contains(*set_b, element)) {
            subset = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return subset;
}

bool _set_view_is_superset(SetView* set_a, SetView* set_b) {
    if (require_non_null(set_a, set_b)) return false;
    return _set_view_is_subset(set_b, set_a);
}