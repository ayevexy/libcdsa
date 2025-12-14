#include "array_list.h"

#include "util/memory.h"
#include <stdio.h>

struct ArrayList {
    void** elements;
    int size;
    int capacity;
    double grow_factor;
    bool (*equals)(void*, void*);
};

static void grow(ArrayList*);

static bool has_next(void* array_list, void* index);

static void* next(void* array_list, void* index);

ArrayList* array_list_new(Options options) {
    ArrayList* array_list = memory_alloc(sizeof(ArrayList));
    array_list->elements = memory_alloc(options.initial_capacity * sizeof(void*));
    array_list->size = 0;
    array_list->capacity = options.initial_capacity;
    array_list->grow_factor = options.grow_factor;
    array_list->equals = options.equals;
    return array_list;
}

void array_list_delete(ArrayList* array_list) {
    memory_free((void**) &array_list->elements);
    memory_free((void**) &array_list);
}

void array_list_add(ArrayList* array_list, void* element) {
    if (array_list->size >= array_list->capacity) {
        grow(array_list);
    }
    array_list->elements[array_list->size++] = element;
}

void array_list_add_at(ArrayList* array_list, int index, void* element) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_add_at index %d out of bounds\n", index);
        return;
    }
    if (array_list->size >= array_list->capacity) {
        grow(array_list);
    }
    for (int i = array_list->size; i > index; i--) {
        array_list->elements[i] = array_list->elements[i - 1];
    }
    array_list->elements[index] = element;
    array_list->size++;
}

void* array_list_get(ArrayList* array_list, int index) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_get index %d out of bounds\n", index);
        return NULL;
    }
    return array_list->elements[index];
}

void array_list_set(ArrayList* array_list, int index, void* element) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_set index %d out of bounds\n", index);
        return;
    }
    array_list->elements[index] = element;
}

void array_list_remove(ArrayList* array_list, int index) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_remove index %d out of bounds\n", index);
        return;
    }
    for (int i = index; i < array_list_size(array_list); i++) {
        array_list->elements[i] = array_list->elements[i + 1];
    }
    array_list->size--;
}

int array_list_size(ArrayList* array_list) {
    return array_list->size;
}

int array_list_capacity(ArrayList* array_list) {
    return array_list->capacity;
}

bool array_list_is_empty(ArrayList* array_list) {
    return array_list->size == 0;
}

Iterator* array_list_iterator(ArrayList* array_list) {
    int* index = memory_alloc(sizeof(int));
    *index = 0;
    return iterator_new(array_list, index, &has_next, &next);
}

static bool has_next(void* array_list, void* index) {
    return *(int*) index < ((ArrayList*) array_list)->size;
}

static void* next(void* array_list, void* index) {
    if (*(int*) index >= ((ArrayList*) array_list)->size) {
        return NULL;
    }
    return ((ArrayList*) array_list)->elements[(*(int*) index)++];
}

void array_list_clear(ArrayList* array_list) {
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = NULL;
    }
    array_list->size = 0;
}

bool array_list_contains(ArrayList* array_list, void* element) {
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return true;
        }
    }
    return false;
}

int array_list_index_of(ArrayList* array_list, void* element) {
    for (int i = 0; i < array_list_size(array_list); i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

static void grow(ArrayList* array_list) {
    array_list->elements = memory_realloc(
        array_list->elements,
        array_list->capacity * array_list->grow_factor * sizeof(void*)
        );
    array_list->capacity *= array_list->grow_factor;
}