#include "iterator.h"

#include "core/constraints.h"

bool iterator_has_next(const Iterator* iterator) {
    if (require_non_null(iterator)) return false;

    return iterator->has_next(iterator->iteration_context);
}

void* iterator_next(Iterator* iterator) {
    if (require_non_null(iterator)) return nullptr;

    return iterator->next(iterator->iteration_context);
}

int iterator_advance(Iterator* iterator, int count) {
    if (require_non_null(iterator) || require_non_negative(count)) return -1;

    int traversed = 0;
    while (traversed < count && iterator_has_next(iterator)) {
        iterator_next(iterator);
        traversed++;
    }
    return traversed;
}

bool iterator_has_previous(const Iterator* iterator) {
    if (require_non_null(iterator)) return false;

    return iterator->has_previous(iterator->iteration_context);
}

void* iterator_previous(Iterator* iterator) {
    if (require_non_null(iterator)) return nullptr;

    return iterator->previous(iterator->iteration_context);
}

int iterator_retreat(Iterator* iterator, int count) {
    if (require_non_null(iterator) || require_non_negative(count)) return -1;

    int traversed = 0;
    while (traversed < count && iterator_has_previous(iterator)) {
        iterator_previous(iterator);
        traversed++;
    }
    return traversed;
}

void* iterator_element(Iterator* iterator, int position) {
    if (require_non_null(iterator) || require_non_negative(position)) return nullptr;

    iterator_advance(iterator, position);

    return iterator_next(iterator);
}

void* iterator_element_or_default(Iterator* iterator, int position, void* default_value) {
    if (require_non_null(iterator) || require_non_negative(position)) return nullptr;

    iterator_advance(iterator, position);

    if (!iterator_has_next(iterator)) {
        return default_value;
    }

    return iterator_next(iterator);
}

void iterator_add(Iterator* iterator, const void* element) {
    if (require_non_null(iterator)) return;

    iterator->add(iterator->iteration_context, element);
}

void iterator_set(Iterator* iterator, const void* element) {
    if (require_non_null(iterator)) return;

    iterator->set(iterator->iteration_context, element);
}

void iterator_remove(Iterator* iterator) {
    if (require_non_null(iterator)) return;

    iterator->remove(iterator->iteration_context);
}

void iterator_reset(Iterator* iterator) {
    if (require_non_null(iterator)) return;

    iterator->reset(iterator->iteration_context);
}

int iterator_size(Iterator* iterator) {
    if (require_non_null(iterator)) return -1;

    int counter = 0;
    while (iterator_has_next(iterator)) {
        iterator_next(iterator);
        counter++;
    }
    return counter;
}

void iterator_for_each_remaining(Iterator* iterator, Consumer action) {
    if (require_non_null(iterator, action)) return;

    while (iterator_has_next(iterator)) {
        action(iterator_next(iterator));
    }
}

void iterator_destroy(Iterator** iterator_pointer) {
    if (require_non_null(iterator_pointer, *iterator_pointer)) return;
    const Iterator* iterator = *iterator_pointer;

    iterator->destroy(iterator->iteration_context);
    *iterator_pointer = nullptr;
}