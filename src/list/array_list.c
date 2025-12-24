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

static void bubble_sort(ArrayList*, Comparator);

static void selection_sort(ArrayList*, Comparator);

static void insertion_sort(ArrayList*, Comparator);

static void merge_sort(ArrayList*, Comparator);

static void quick_sort(ArrayList*, Comparator);

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
    const int new_capacity = (int) (array_list->capacity * array_list->grow_factor);
    array_list->elements = memory_realloc(array_list->elements, new_capacity * sizeof(void*));
    array_list->capacity = new_capacity;
}

void array_list_add_first(ArrayList* array_list, void* element) {
    array_list_add_at(array_list, 0, element);
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

void* array_list_get_first(ArrayList* array_list) {
    return array_list_get(array_list, 0);
}

void* array_list_get_last(ArrayList* array_list) {
    return array_list_get(array_list, array_list->size - 1);
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

void array_list_remove_first(ArrayList* array_list) {
    array_list_remove(array_list, 0);
}

void array_list_remove_last(ArrayList* array_list) {
    array_list_remove(array_list, array_list->size - 1);
}

void array_list_remove_element(ArrayList* array_list, void* element) {
    int index = array_list_index_of(array_list, element);
    array_list_remove(array_list, index);
}

void array_list_remove_all(ArrayList* array_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    while (iterator_has_next(iterator)) {
        array_list_remove_element(array_list, iterator_next(iterator));
    }
    iterator_delete(iterator);
}

void array_list_remove_range(ArrayList* array_list, int start_index, int end_index) {
    if (start_index < 0 || start_index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_remove_range start_index %d out of bounds\n", start_index);
        return;
    }
    if (end_index < 0 || end_index >= array_list->size) {
        fprintf(stderr, "Warning: array_list_remove_range end_index %d out of bounds\n", end_index);
        return;
    }
    if (start_index > end_index) {
        fprintf(stderr, "Warning: array_list_remove_range start_index %d greater than end_index %d\n", start_index, end_index);
        return;
    }
    const int initial_size = array_list->size;
    for (int i = start_index, j = end_index + 1; i <= end_index; i++, j++) {
        array_list->elements[i] = j < initial_size ? array_list->elements[j] : nullptr;
        array_list->size--;
    }
}

void array_list_remove_if(ArrayList* array_list, Predicate condition_matches) {
    for (int i = array_list->size - 1; i >= 0; i--) {
        if (condition_matches(array_list->elements[i])) {
            array_list_remove(array_list, i);
        }
    }
}

void array_list_replace_all(ArrayList* array_list, UnaryOperator operator_apply) {
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = operator_apply(array_list->elements[i]);
    }
}

void array_list_retain_all(ArrayList* array_list, Collection collection) {
    for (int i = array_list->size - 1; i >= 0; i--) {
        Iterator* iterator = collection_iterator(collection);
        bool found = false;

        while (iterator_has_next(iterator)) {
            if (array_list->equals(array_list->elements[i], iterator_next(iterator))) {
                found = true;
                break;
            }
        }
        if (!found) {
            array_list_remove(array_list, i);
        }
        iterator_delete(iterator);
    }
}

int array_list_size(ArrayList* array_list) {
    return array_list->size;
}

void array_list_trim_to_size(ArrayList* array_list) {
    array_list->elements = memory_realloc(array_list->elements, sizeof(void*) * array_list->size);
    array_list->capacity = array_list->size;
}

int array_list_capacity(ArrayList* array_list) {
    return array_list->capacity;
}

void array_list_ensure_capacity(ArrayList* array_list, int capacity) {
    while (array_list->capacity < capacity) {
        grow(array_list);
    }
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
        case BUBBLE_SORT: { bubble_sort(array_list, comparator); return; }
        case INSERTION_SORT: { insertion_sort(array_list, comparator); return; }
        case SELECTION_SORT: { selection_sort(array_list, comparator); return; }
        case MERGE_SORT: { merge_sort(array_list, comparator); return; }
        case QUICK_SORT: { quick_sort(array_list, comparator); }
    }
}

static void bubble_sort(ArrayList* array_list, Comparator compare) {
    for (int i = 0; i < array_list->size - 1; i++) {
        for (int j = 0; j < array_list->size - i - 1; j++) {
            if (compare(array_list->elements[j], array_list->elements[j + 1]) >= 0) {
                void* swap = array_list->elements[j];
                array_list->elements[j] = array_list->elements[j + 1];
                array_list->elements[j + 1] = swap;
            }
        }
    }
}

static void selection_sort(ArrayList* array_list, Comparator compare) {
    for (int i = 0; i < array_list->size - 1; i++) {
        int min_index = i;
        for (int j = i + 1; j < array_list->size; j++) {
            if (compare(array_list->elements[j], array_list->elements[min_index]) < 0) {
                min_index = j;
            }
        }
        void* swap = array_list->elements[i];
        array_list->elements[i] = array_list->elements[min_index];
        array_list->elements[min_index] = swap;
    }
}

static void insertion_sort(ArrayList* array_list, Comparator compare) {
    for (int i = 1; i < array_list->size; i++) {
        void* element = array_list->elements[i];
        int j = i - 1;
        while (j >= 0 && compare(array_list->elements[j], element) > 0) {
            array_list->elements[j + 1] = array_list->elements[j];
            j = j - 1;
        }
        array_list->elements[j + 1] = element;
    }
}

// No recursion because yes
static void merge_sort(ArrayList* array_list, Comparator compare) {
    void** temporary = memory_alloc(sizeof(void*) * array_list->size);

    for (int subarray_size = 1; subarray_size < array_list->size; subarray_size *= 2) {
        for (int start_index = 0; start_index < array_list->size - 1; start_index += 2 * subarray_size) {

            int mid_index = start_index + subarray_size - 1;
            int end_index = start_index + 2 * subarray_size - 1;

            if (mid_index >= array_list->size - 1) {
                continue;
            }
            if (end_index >= array_list->size) {
                end_index = array_list->size - 1;
            }

            int left_subarray_index = start_index;
            int right_subarray_index = mid_index + 1;
            int current_index = start_index;

            while (left_subarray_index <= mid_index && right_subarray_index <= end_index) {
                if (compare(array_list->elements[left_subarray_index], array_list->elements[right_subarray_index]) <= 0) {
                    temporary[current_index++] = array_list->elements[left_subarray_index++];
                } else {
                    temporary[current_index++] = array_list->elements[right_subarray_index++];
                }
            }

            while (left_subarray_index <= mid_index) {
                temporary[current_index++] = array_list->elements[left_subarray_index++];
            }
            while (right_subarray_index <= end_index) {
                temporary[current_index++] = array_list->elements[right_subarray_index++];
            }

            for (left_subarray_index = start_index; left_subarray_index <= end_index; left_subarray_index++) {
                array_list->elements[left_subarray_index] = temporary[left_subarray_index];
            }
        }
    }
    memory_free((void**) &temporary);
}

// Another iterative implementation
static void quick_sort(ArrayList* array_list, Comparator compare) {
    int* stack = memory_alloc(sizeof(int) * (array_list->size + 1));
    int top = -1;

    stack[++top] = 0;
    stack[++top] = array_list->size - 1;

    while (top >= 0) {
        int end_index = stack[top--];
        int start_index = stack[top--];

        int pivot = start_index - 1;

        for (int i = start_index; i <= end_index - 1; i++) {
            if (compare(array_list->elements[i], array_list->elements[end_index]) <= 0) {
                pivot++;
                void* swap = array_list->elements[pivot];
                array_list->elements[pivot] = array_list->elements[i];
                array_list->elements[i] = swap;
            }
        }
        pivot++;

        void* swap = array_list->elements[pivot];
        array_list->elements[pivot] = array_list->elements[end_index];
        array_list->elements[end_index] = swap;

        if (pivot - 1 > start_index) {
            stack[++top] = start_index;
            stack[++top] = pivot - 1;
        }

        if (pivot + 1 < end_index) {
            stack[++top] = pivot + 1;
            stack[++top] = end_index;
        }
    }
    memory_free((void**) &stack);
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

bool array_list_contains_all(ArrayList* array_list, Collection collection) {
    bool contains = false;
    Iterator* iterator = collection_iterator(collection);
    while (iterator_has_next(iterator)) {
        void* element = iterator_next(iterator);
        contains = array_list_contains(array_list, element);
        if (!contains) break;
    }
    iterator_delete(iterator);
    return contains;
}

int array_list_index_of(ArrayList* array_list, void* element) {
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_of(ArrayList* array_list, void* element) {
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            last_index = i;
        }
    }
    return last_index;
}

int array_list_binary_search(ArrayList* array_list, void* element, Comparator compare) {
    int start_index = 0;
    int end_index = array_list->size - 1;

    while (start_index <= end_index) {
        int mid_index = start_index + (end_index - start_index) / 2;
        int result = compare(array_list->elements[mid_index], element);

        if (result == 0) {
            return mid_index;
        }
        if (result < 0) {
            start_index = mid_index + 1;
        }
        if (result > 0) {
            end_index = mid_index - 1;
        }
    }
    return -1;
}

ArrayList* array_list_clone(ArrayList* array_list) {
    return array_list_sub_list(array_list, 0, array_list->size - 1);
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

void** array_list_to_array(ArrayList* array_list) {
    void** elements = memory_alloc(sizeof(void*) * array_list->size);
    for (int i = 0; i < array_list->size; i++) {
        elements[i] = array_list->elements[i];
    }
    return elements;
}

char* array_list_to_string(ArrayList* array_list) {
    char* element_string = array_list->to_string(array_list->elements[0]);
    char* string = memory_alloc(5 + (strlen(element_string) + 2) * array_list->size);
    memory_free((void**) &element_string);

    string[0] = '\0'; // initialize string to clear trash data
    strcat(string, "[ ");

    for (int i = 0; i < array_list->size; i++) {
        element_string = array_list->to_string(array_list->elements[i]);
        strcat(string, element_string);

        if (i < array_list->size - 1) {
            strcat(string, ", ");
        }
        memory_free((void**) &element_string);
    }

    strcat(string, " ]");
    return string;
}