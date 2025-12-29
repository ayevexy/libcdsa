#include "array_list.h"

#include <stdio.h>
#include <string.h>

struct ArrayList {
    void** elements;
    int size;
    int capacity;
    double grow_factor;
    bool (*equals)(const void*, const void*);
    char* (*to_string)(const void*);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
};

static bool grow(ArrayList*);

typedef struct IterationContext IterationContext;

static Iterator* iterator(const ArrayList*);

static bool has_next(const ArrayList*, const IterationContext*);

static void* next(const ArrayList*, IterationContext*);

static void reset(IterationContext*);

static void bubble_sort(ArrayList*, Comparator);

static void selection_sort(ArrayList*, Comparator);

static void insertion_sort(ArrayList*, Comparator);

static void merge_sort(ArrayList*, int, int, Comparator);

static void merge(void**, int, int, int, Comparator);

static void quick_sort(ArrayList*, int, int, Comparator);

static int partition(void**, int, int, Comparator);

static void swap(void** a, void** b);

ArrayList* array_list_new(const ArrayListOptions* options) {
    if (options->initial_capacity < 10 || options->grow_factor <= 1.1 || !options->equals || !options->to_string) {
        fprintf(stderr, "Exception at array_list_new(%p) invalid options\n", (void*) options);
        return nullptr;
    }
    ArrayList* array_list = options->memory_alloc(sizeof(ArrayList));
    if (!array_list) {
        fprintf(stderr, "Error: memory allocation failure at array_list_new()\n");
        return nullptr;
    }
    array_list->elements = options->memory_alloc(options->initial_capacity * sizeof(void*));
    if (!array_list->elements) {
        fprintf(stderr, "Error: memory allocation failure at array_list_new()\n");
        options->memory_free(array_list);
        return nullptr;
    }
    array_list->size = 0;
    array_list->capacity = options->initial_capacity;
    array_list->grow_factor = options->grow_factor;
    array_list->equals = options->equals;
    array_list->to_string = options->to_string;
    array_list->memory_alloc = options->memory_alloc;
    array_list->memory_realloc = options->memory_realloc;
    array_list->memory_free = options->memory_free;
    return array_list;
}

ArrayList* array_list_from(Collection collection, const ArrayListOptions* options) {
    ArrayList* array_list = array_list_new(options);
    if (array_list) {
        array_list_add_all_last(array_list, collection);
    }
    return array_list;
}

void array_list_delete(ArrayList** array_list_pointer) {
    if (!*array_list_pointer) {
        fprintf(stderr, "Exception at array_list_delete(%p) null pointer\n", nullptr);
        return;
    }
    ArrayList* array_list = *array_list_pointer;
    array_list->memory_free(array_list->elements);
    array_list->memory_free(array_list);
    *array_list_pointer = nullptr;
}

void array_list_destroy(ArrayList** array_list_pointer, void (*delete)(void*)) {
    if (!*array_list_pointer) {
        fprintf(stderr, "Exception at array_list_destroy(%p) null pointer\n", nullptr);
        return;
    }
    for (int i = 0; i < (*array_list_pointer)->size; i++) {
        delete((*array_list_pointer)->elements[i]);
    }
    array_list_delete(array_list_pointer);
}

bool array_list_add(ArrayList* array_list, int index, const void* element) {
    if (index < 0 || index > array_list->size) {
        fprintf(stderr,  "Exception at array_list_add(%p, %d) index out of bounds\n", (void*) array_list, index);
        return false;
    }
    for (int i = 0; i < (*array_list_pointer)->size; i++) {
        delete((*array_list_pointer)->elements[i]);
    }
    array_list_delete(array_list_pointer);
}

bool array_list_add(ArrayList* array_list, int index, const void* element) {
    if (index < 0 || index > array_list->size) {
        fprintf(stderr,  "Exception at array_list_add(%p, %d) index out of bounds\n", (void*) array_list, index);
        return false;
    }
    if (array_list->size >= array_list->capacity) {
        if (!grow(array_list)) return false;
    }
    for (int i = array_list->size; i > index; i--) {
        array_list->elements[i] = array_list->elements[i - 1];
    }
    array_list->elements[index] = (void*) element;
    array_list->size++;
    return true;
}

bool array_list_add_first(ArrayList* array_list, const void* element) {
   return array_list_add(array_list, 0, element);
}

bool array_list_add_last(ArrayList* array_list, const void* element) {
   return array_list_add(array_list, array_list->size, element);
}

bool array_list_add_all(ArrayList* array_list, int index, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    int count = 0;
    while (iterator_has_next(iterator)) {
        if (array_list_add(array_list, index++, iterator_next(iterator))) {
            count++;
        }
    }
    iterator_delete(iterator);
    return count == collection_size(collection);
}

bool array_list_add_all_first(ArrayList* array_list, Collection collection) {
    return array_list_add_all(array_list, 0, collection);
}

bool array_list_add_all_last(ArrayList* array_list, Collection collection) {
    return array_list_add_all(array_list, array_list->size, collection);
}

void* array_list_get(const ArrayList* array_list, int index) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Exception at array_list_get(%p, %d) index out of bounds\n", (void*) array_list, index);
        return nullptr;
    }
    return array_list->elements[index];
}

void* array_list_get_first(const ArrayList* array_list) {
    return array_list_get(array_list, 0);
}

void* array_list_get_last(const ArrayList* array_list) {
    return array_list_get(array_list, array_list->size - 1);
}

void* array_list_set(ArrayList* array_list, int index, const void* element) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Exception at array_list_set(%p, %d) index out of bounds\n", (void*) array_list, index);
        return nullptr;
    }
    void* old_element = array_list->elements[index];;
    array_list->elements[index] = (void*) element;
    return old_element;
}

bool array_list_swap(ArrayList* array_list, int index_a, int index_b) {
    if (index_a < 0 || index_a >= array_list->size || index_b < 0 || index_b >= array_list->size) {
        fprintf(stderr, "Exception at array_list_swap(%p, %d, %d) index out of bounds\n", (void*) array_list, index_a, index_b);
        return false;
    }
    swap(&array_list->elements[index_a], &array_list->elements[index_b]);
    return true;
}

void* array_list_remove(ArrayList* array_list, int index) {
    if (index < 0 || index >= array_list->size) {
        fprintf(stderr, "Exception at array_list_remove(%p, %d) index out of bounds\n", (void*) array_list, index);
        return nullptr;
    }
    void* element = array_list->elements[index];
    for (int i = index; i < array_list->size - 1; i++) {
        array_list->elements[i] = array_list->elements[i + 1];
    }
    array_list->size--;
    return element;
}

void* array_list_remove_first(ArrayList* array_list) {
    return array_list_remove(array_list, 0);
}

void* array_list_remove_last(ArrayList* array_list) {
    return array_list_remove(array_list, array_list->size - 1);
}

bool array_list_remove_element(ArrayList* array_list, const void* element) {
    const int index = array_list_index_of(array_list, element);
    if (index >= 0) {
        array_list_remove(array_list, index);
        return true;
    }
    return false;
}

int array_list_remove_all(ArrayList* array_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    int count = 0;
    while (iterator_has_next(iterator)) {
        if (array_list_remove_element(array_list, iterator_next(iterator))) {
            count++;
        }
    }
    iterator_delete(iterator);
    return count;
}

int array_list_remove_range(ArrayList* array_list, int start_index, int end_index) {
    if (start_index < 0 || end_index > array_list->size || start_index >= end_index) {
        fprintf(stderr, "Exception at array_list_remove_range(%p, %d, %d) invalid range\n", (void*) array_list, start_index, end_index);
        return 0;
    }
    int count = end_index - start_index;
    for (int i = start_index; i < array_list->size - count; i++) {
        array_list->elements[i] = array_list->elements[i + count];
    }
    array_list->size -= count;
    return count;
}

int array_list_remove_if(ArrayList* array_list, Predicate condition_matches) {
    int count = 0;
    for (int i = array_list->size - 1; i >= 0; i--) {
        if (condition_matches(array_list->elements[i])) {
            array_list_remove(array_list, i);
            count++;
        }
    }
    return count;
}

void array_list_replace_all(ArrayList* array_list, UnaryOperator operator_apply) {
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = operator_apply(array_list->elements[i]);
    }
}

int array_list_retain_all(ArrayList* array_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    int count = 0;

    for (int i = array_list->size - 1; i >= 0; i--) {
        bool found = false;

        while (iterator_has_next(iterator)) {
            if (array_list->equals(array_list->elements[i], iterator_next(iterator))) {
                found = true;
                break;
            }
        }
        if (!found) {
            array_list_remove(array_list, i);
            count++;
        }
        iterator_reset(iterator);
    }
    iterator_delete(iterator);
    return count;
}

int array_list_size(const ArrayList* array_list) {
    return array_list->size;
}

bool array_list_trim_to_size(ArrayList* array_list) {
    constexpr int MIN_CAPACITY = 10;
    const int new_capacity = (array_list->size < MIN_CAPACITY) ? MIN_CAPACITY : array_list->size;
    void** elements = array_list->memory_realloc(array_list->elements, sizeof(void*) * new_capacity);
    if (!elements) {
        fprintf(stderr, "Error: memory reallocation failure at array_list_trim_to_size()\n");
        return false;
    }
    array_list->elements = elements;
    array_list->capacity = new_capacity;
    return true;
}

int array_list_capacity(const ArrayList* array_list) {
    return array_list->capacity;
}

bool array_list_ensure_capacity(ArrayList* array_list, int capacity) {
    // TODO: improve grow logic
    while (array_list->capacity < capacity) {
        if (!grow(array_list)) return false;
    }
    return true;
}

bool array_list_is_empty(const ArrayList* array_list) {
    return array_list->size == 0;
}

Iterator* array_list_iterator(const ArrayList* array_list) {
   return iterator((ArrayList*) array_list);
}

void array_list_for_each(ArrayList* array_list, Consumer action) {
    for (int i = 0; i < array_list->size; i++) {
        action(array_list->elements[i]);
    }
}

void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm) {
    if (array_list->size < 2) {
        return;
    }
    switch (algorithm) {
        case BUBBLE_SORT: { bubble_sort(array_list, comparator); return; }
        case INSERTION_SORT: { insertion_sort(array_list, comparator); return; }
        case SELECTION_SORT: { selection_sort(array_list, comparator); return; }
        case MERGE_SORT: { merge_sort(array_list, 0, array_list->size - 1, comparator); return; }
        case QUICK_SORT: { quick_sort(array_list, 0, array_list->size - 1, comparator); }
    }
}

void array_list_reverse(ArrayList* array_list) {
    for (int i = 0; i < array_list->size / 2; i++) {
        swap(&array_list->elements[array_list->size - 1 - i], &array_list->elements[i]);
    }
}

void array_list_clear(ArrayList* array_list) {
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = nullptr;
    }
    array_list->size = 0;
}

void array_list_clear_data(ArrayList* array_list, void (*delete)(void*)) {
    for (int i = 0; i < array_list->size; i++) {
        delete(array_list->elements[i]);
        array_list->elements[i] = nullptr;
    }
    array_list->size = 0;
}

void* array_list_find(const ArrayList* array_list, Predicate condition) {
    for (int i = 0; i < array_list->size; i++) {
        void* element = array_list->elements[i];
        if (condition(element)) {
            return element;
        }
    }
    return nullptr;
}

void* array_list_find_last(const ArrayList* array_list, Predicate condition) {
    void* element = nullptr;
    for (int i = 0; i < array_list->size; i++) {
        void* current = array_list->elements[i];
        if (condition(current)) {
            element = current;
        }
    }
    return element;
}

int array_list_index_where(const ArrayList* array_list, Predicate condition) {
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_where(const ArrayList* array_list, Predicate condition) {
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            last_index = i;
        }
    }
    return last_index;
}

bool array_list_contains(const ArrayList* array_list, const void* element) {
    return array_list_index_of(array_list, element) != -1;
}

bool array_list_contains_all(const ArrayList* array_list, Collection collection) {
    Iterator* iterator = collection_iterator(collection);
    bool contains = true;
    while (iterator_has_next(iterator)) {
        void* element = iterator_next(iterator);
        if (!array_list_contains(array_list, element)) {
            contains = false;
            break;
        }
    }
    iterator_delete(iterator);
    return contains;
}

int array_list_occurrences_of(const ArrayList* array_list, const void* element) {
    int count = 0;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            count++;
        }
    }
    return count;
}

int array_list_index_of(const ArrayList* array_list, const void* element) {
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_of(const ArrayList* array_list, const void* element) {
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            last_index = i;
        }
    }
    return last_index;
}

int array_list_binary_search(const ArrayList* array_list, const void* element, Comparator compare) {
    int start_index = 0;
    int end_index = array_list->size - 1;

    while (start_index <= end_index) {
        const int mid_index = start_index + (end_index - start_index) / 2;
        const int result = compare(array_list->elements[mid_index], element);

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

ArrayList* array_list_clone(const ArrayList* array_list) {
    return array_list_sub_list(array_list, 0, array_list->size);
}

ArrayList* array_list_sub_list(const ArrayList* array_list, int start_index, int end_index) {
    if (start_index < 0 || end_index > array_list->size || start_index > end_index) {
        fprintf(stderr, "Exception at array_list_sub_list(%p, %d, %d) invalid range\n", (void*) array_list, start_index, end_index);
        return nullptr;
    }
    ArrayList* new_array_list = array_list_new(&(ArrayListOptions) {
        .initial_capacity = array_list->capacity,
        .grow_factor = array_list->grow_factor,
        .equals = array_list->equals,
        .to_string = array_list->to_string,
        .memory_alloc = array_list->memory_alloc,
        .memory_realloc = array_list->memory_realloc,
        .memory_free = array_list->memory_free
    });
    if (!new_array_list) {
        fprintf(stderr, "Error: array_list_sub_list() failed to create new list\n");
        return nullptr;
    }
    for (int i = start_index; i < end_index; i++) {
        array_list_add_last(new_array_list, array_list->elements[i]);
    }
    return new_array_list;
}

Collection array_list_to_collection(const ArrayList* array_list) {
    return collection_from((void*) array_list);
}

void** array_list_to_array(const ArrayList* array_list) {
    void** elements = array_list->memory_alloc(sizeof(void*) * array_list->size);
    if (!elements) {
        fprintf(stderr, "Error: memory allocation failure at array_list_to_array()\n");
        return nullptr;
    }
    for (int i = 0; i < array_list->size; i++) {
        elements[i] = array_list->elements[i];
    }
    return elements;
}

// TODO: redesign, currently it uses two different allocators
char* array_list_to_string(const ArrayList* array_list) {
    char* string = array_list->memory_alloc(sizeof(char) * 4);
    string[0] = '\0'; // initialize string to clear trash data

    if (array_list->size == 0) {
        strcat(string, "[]");
        return string;
    }

    strcat(string, "[ ");

    for (int i = 0; i < array_list->size; i++) {
        char* element_string = array_list->to_string(array_list->elements[i]);
        const int extra_space = i < array_list->size - 1 ? 2 : 0;

        string = array_list->memory_realloc(string, strlen(string) + strlen(element_string) + extra_space + 1);
        strcat(string, element_string);

        if (i < array_list->size - 1) {
            strcat(string, ", ");
        }
        array_list->memory_free(element_string);
    }

    string = array_list->memory_realloc(string, strlen(string) + 3);
    strcat(string, " ]");
    return string;
}

static bool grow(ArrayList* array_list) {
    const int new_capacity = (int) (array_list->capacity * array_list->grow_factor);
    void** elements = array_list->memory_realloc(array_list->elements, new_capacity * sizeof(void*));
    if (!elements) {
        fprintf(stderr, "Error: memory allocation failure at array_list grow()\n");
        return false;
    }
    array_list->elements = elements;
    array_list->capacity = new_capacity;
    return true;
}

struct IterationContext {
    int cursor;
};

static Iterator* iterator(const ArrayList* array_list) {
    IterationContext* iteration_context = array_list->memory_alloc(sizeof(IterationContext));
    if (!iteration_context) {
        fprintf(stderr, "Error: memory allocation failure at array_list iterator()\n");
        return nullptr;
    }
    iteration_context->cursor = 0;
    return iterator_from(array_list, iteration_context, has_next, next, reset);
}

static bool has_next(const ArrayList* array_list, const IterationContext* iteration_context) {
    return iteration_context->cursor < array_list->size;
}

static void* next(const ArrayList* array_list, IterationContext* iteration_context) {
    if (iteration_context->cursor >= array_list->size) {
        return nullptr;
    }
    return array_list->elements[iteration_context->cursor++];
}

static void reset(IterationContext* iteration_context) {
    iteration_context->cursor = 0;
}

static void bubble_sort(ArrayList* array_list, Comparator compare) {
    for (int i = 0; i < array_list->size - 1; i++) {
        for (int j = 0; j < array_list->size - i - 1; j++) {
            if (compare(array_list->elements[j], array_list->elements[j + 1]) > 0) {
                swap(&array_list->elements[j], &array_list->elements[j + 1]);
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
        swap(&array_list->elements[min_index], &array_list->elements[i]);
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

static void merge_sort(ArrayList* array_list, int start_index, int end_index, Comparator compare) {
    if (start_index < end_index) {
        const int middle_index = start_index + (end_index - start_index) / 2;

        merge_sort(array_list, start_index, middle_index, compare);
        merge_sort(array_list, middle_index + 1, end_index, compare);

        merge(array_list->elements, start_index, middle_index, end_index, compare);
    }
}

static void merge(void** elements, int start_index, int middle_index, int end_index, Comparator compare) {
    const int LEFT_ELEMENTS_SIZE = middle_index - start_index + 1;
    const int RIGHT_ELEMENTS_SIZE = end_index - middle_index;

    void* left_elements[LEFT_ELEMENTS_SIZE];
    void* right_elements[RIGHT_ELEMENTS_SIZE];

    for (int i = 0; i < LEFT_ELEMENTS_SIZE; i++) {
        left_elements[i] = elements[start_index + i];
    }
    for (int i = 0; i < RIGHT_ELEMENTS_SIZE; i++) {
        right_elements[i] = elements[middle_index + 1 + i];
    }

    int left_elements_index = 0, right_elements_index = 0, index = start_index;

    while (left_elements_index < LEFT_ELEMENTS_SIZE && right_elements_index < RIGHT_ELEMENTS_SIZE) {
        if (compare(left_elements[left_elements_index], right_elements[right_elements_index]) <= 0) {
            elements[index] = left_elements[left_elements_index];
            left_elements_index++;
        } else {
            elements[index] = right_elements[right_elements_index];
            right_elements_index++;
        }
        index++;
    }

    while (left_elements_index < LEFT_ELEMENTS_SIZE) {
        elements[index] = left_elements[left_elements_index];
        left_elements_index++;
        index++;
    }

    while (right_elements_index < RIGHT_ELEMENTS_SIZE) {
        elements[index] = right_elements[right_elements_index];
        right_elements_index++;
        index++;
    }
}

static void quick_sort(ArrayList* array_list, int start_index, int end_index, Comparator compare) {
    if (start_index < end_index) {
        const int pivot = partition(array_list->elements, start_index, end_index, compare);

        quick_sort(array_list, start_index, pivot - 1, compare);
        quick_sort(array_list, pivot + 1, end_index, compare);
    }
}

static int partition(void** elements, int start_index, int end_index, Comparator compare) {
    int pivot = start_index - 1;

    for (int i = start_index; i < end_index; i++) {
        if (compare(elements[i], elements[end_index]) <= 0) {
            pivot++;
            swap(&elements[i], &elements[pivot]);
        }
    }
    swap(&elements[pivot + 1], &elements[end_index]);
    return pivot + 1;
}

static void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}