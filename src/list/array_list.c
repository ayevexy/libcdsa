#include "array_list.h"

#include "util/memory.h"
#include <stdio.h>
#include <string.h>

struct ArrayList {
    void** elements;
    int size;
    int capacity;
    double grow_factor;
    bool (*equals)(void*, void*);
    char* (*to_string)(void*);
};

static void grow(ArrayList*);

static bool has_next(void* array_list, void* index);

static void* next(void* array_list, void* index);

static void bubble_sort(ArrayList*, Comparator compare);

ArrayList* array_list_new(Options options) {
    ArrayList* array_list = memory_alloc(sizeof(ArrayList));
    array_list->elements = memory_alloc(options.initial_capacity * sizeof(void*));
    array_list->size = 0;
    array_list->capacity = options.initial_capacity;
    array_list->grow_factor = options.grow_factor;
    array_list->equals = options.equals;
    array_list->to_string = options.to_string;
    return array_list;
}

ArrayList* array_list_from(Collection collection, Options options) {
    ArrayList* array_list = array_list_new(options);
    array_list_add_all(array_list, collection);
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

static void grow(ArrayList* array_list) {
    array_list->elements = memory_realloc(
        array_list->elements,
        array_list->capacity * array_list->grow_factor * sizeof(void*)
        );
    array_list->capacity *= array_list->grow_factor;
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

void array_list_add_all(ArrayList* array_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    while (iterator_has_next(iterator)) {
        array_list_add(array_list, iterator_next(iterator));
    }
    iterator_delete(iterator);
}

void array_list_add_all_at(ArrayList* array_list, int index, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    while (iterator_has_next(iterator)) {
        array_list_add_at(array_list, index++, iterator_next(iterator));
    }
    iterator_delete(iterator);
}

void* array_list_get(ArrayList* array_list, int index) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_get index %d out of bounds\n", index);
        return nullptr;
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
    for (int i = index; i < array_list->size; i++) {
        array_list->elements[i] = array_list->elements[i + 1];
    }
    array_list->size--;
}

void array_list_remove_element(ArrayList* array_list, void* element) {
    int index = array_list_index_of(array_list, element);
    array_list_remove(array_list, index);
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
        return nullptr;
    }
    return ((ArrayList*) array_list)->elements[(*(int*) index)++];
}

void array_list_for_each(ArrayList* array_list, Consumer action) {
    for (int i = 0; i < array_list->size; i++) {
        action(array_list->elements[i]);
    }
}

void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm) {
    switch (algorithm) {
        case BUBBLE_SORT: { bubble_sort(array_list, comparator); }
    }
}

static void bubble_sort(ArrayList* array_list, Comparator compare) {
    for (int i = 0; i < array_list->size - 1; i++) {
        for (int j = 0; j < array_list->size - i - 1; j++) {
            if (compare(array_list->elements[j], array_list->elements[j + 1]) > 0) {
                void* swap = array_list->elements[j];
                array_list->elements[j] = array_list->elements[j + 1];
                array_list->elements[j + 1] = swap;
            }
        }
    }
}

void array_list_clear(ArrayList* array_list) {
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = nullptr;
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
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

ArrayList* array_list_sub_list(ArrayList* array_list, int start_index, int end_index) {
    if (start_index < 0 || start_index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_sub_list start_index %d out of bounds\n", start_index);
        return nullptr;
    }
    if (end_index < 0 || end_index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_sub_list end_index %d out of bounds\n", end_index);
        return nullptr;
    }
    if (start_index > end_index) {
        fprintf(stderr, "Warning: array_list_sub_list start_index %d greater than end_index %d\n", start_index, end_index);
        return nullptr;
    }
    const Options options = {
        .initial_capacity = array_list->capacity,
        .grow_factor = array_list->grow_factor,
        .equals = array_list->equals,
        .to_string = array_list->to_string
    };
    ArrayList* new_array_list = array_list_new(options);
    for (int i = start_index; i <= end_index; i++) {
        array_list_add(new_array_list, array_list->elements[i]);
    }
    return new_array_list;
}

Collection array_list_to_collection(ArrayList* array_list) {
    return collection_from(array_list);
}

char* array_list_to_string(ArrayList* array_list) {
    const unsigned long ELEMENT_STRING_SIZE = strlen(array_list->to_string(array_list->elements[0]));
    char* string = memory_alloc(5 + (ELEMENT_STRING_SIZE + 2) * array_list->size);
    strcat(string, "[ ");
    for (int i = 0; i < array_list->size; i++) {
        strcat(string, array_list->to_string(array_list->elements[i]));
        if (i < array_list->size - 1) {
            strcat(string, ", ");
        }
    }
    strcat(string, " ]");
    return string;
}