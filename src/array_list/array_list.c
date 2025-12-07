#include "array_list.h"

#include <stddef.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 10
#define GROW_FACTOR 2

struct ArrayList {
    void** elements;
    size_t size;
    size_t capacity;
};

static void grow(ArrayList*);

ArrayList* array_list_new() {
    ArrayList* array_list = malloc(sizeof(ArrayList));
    array_list->elements = malloc(INITIAL_CAPACITY * sizeof(void*));
    array_list->size = 0;
    array_list->capacity = INITIAL_CAPACITY;
    return array_list;
}

void array_list_delete(ArrayList* array_list) {
    free(array_list->elements);
    free(array_list);
}

void array_list_add(ArrayList* array_list, void* element) {
    if (array_list->size >= array_list->capacity) {
        grow(array_list);
    }
    array_list->elements[array_list->size++] = element;
}

void* array_list_get(ArrayList* array_list, int index) {
    return array_list->elements[index];
}

void array_list_set(ArrayList* array_list, int index, void* element) {
    // TODO: implement
}

void array_list_remove(ArrayList* array_list, int index) {
    // TODO: implement
}

size_t array_list_size(ArrayList* array_list) {
    return array_list->size;
}

size_t array_list_capacity(ArrayList* array_list) {
    return array_list->capacity;
}

static void grow(ArrayList* array_list) {
    void** elements = realloc(array_list->elements, array_list->capacity * GROW_FACTOR * sizeof(void*));
    if (!elements) {
        exit(EXIT_FAILURE); // Hope this never happens
    }
    array_list->elements = elements;
    array_list->capacity *= GROW_FACTOR;
}