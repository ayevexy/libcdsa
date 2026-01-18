#include "array_list.h"

#include "util/error.h"
#include "util/non_null.h"
#include <string.h>
#include <limits.h>

#define MIN_CAPACITY 10
#define MAX_CAPACITY (INT_MAX - 1)
#define MIN_GROW_FACTOR 1.1

struct ArrayList {
    void** elements;
    int size;
    int capacity;
    double grow_factor;
    bool (*equals)(const void*, const void*);
    int (*to_string)(const void*, char*, size_t);
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_free)(void*);
    };
};

static size_t calculate_string_size(const ArrayList*);

static bool resize(ArrayList*, int);

typedef struct IterationContext IterationContext;

static Iterator* create_iterator(const ArrayList*);

static bool has_next(const IterationContext*);

static void* next(IterationContext*);

static void reset(IterationContext*);

static void bubble_sort(ArrayList*, Comparator);

static void selection_sort(ArrayList*, Comparator);

static void insertion_sort(ArrayList*, Comparator);

static void merge_sort(ArrayList*, int, int, Comparator);

static void merge(void**, int, int, int, Comparator);

static void quick_sort(ArrayList*, int, int, Comparator);

static int partition(void**, int, int, Comparator);

static void durstenfeld_shuffle(ArrayList*, int (*random)(void));

static void sattolo_shuffle(ArrayList*, int (*random)(void));

static void naive_shuffle(ArrayList*, int (*random)(void));

static void reverse(ArrayList*, int, int);

static void swap(void** a, void** b);

ArrayList* array_list_new(const ArrayListOptions* options) {
    require_non_null(options);

    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->grow_factor < MIN_GROW_FACTOR || !options->equals || !options->to_string
        || !options->memory_alloc || !options->memory_realloc || !options->memory_free
    ) {
        raise_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints") nullptr;
    }
    ArrayList* array_list = options->memory_alloc(sizeof(ArrayList));
    if (!array_list) {
        raise_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array_list'") nullptr;
    }
    array_list->elements = options->memory_alloc(options->initial_capacity * sizeof(void*));
    if (!array_list->elements) {
        options->memory_free(array_list);
        raise_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array_list'") nullptr;
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
    require_non_null(options); require_non_empty_collection(collection);

    ArrayList* array_list; Error error;

    if ((error = attempt(array_list = array_list_new(options)))) {
        raise_error(error, "%s", plain_error_message()) nullptr;
    }

    if ((error = attempt(array_list_add_all_last(array_list, collection)))) {
        array_list_delete(&array_list);
        raise_error(error, "%s", plain_error_message()) nullptr;
    }

    return array_list;
}

void array_list_delete(ArrayList** array_list_pointer) {
    require_non_null(array_list_pointer);
    if (!*array_list_pointer) {
        raise_error(NULL_POINTER_ERROR, "'array_list' must not be null");
    }
    ArrayList* array_list = *array_list_pointer;
    array_list->memory_free(array_list->elements);
    array_list->memory_free(array_list);
    *array_list_pointer = nullptr;
}

void array_list_destroy(ArrayList** array_list_pointer, void (*delete)(void*)) {
    require_non_null(array_list_pointer, delete);
    if (!*array_list_pointer) {
        raise_error(NULL_POINTER_ERROR, "'array_list' must not be null");
    }
    for (int i = 0; i < (*array_list_pointer)->size; i++) {
        delete((*array_list_pointer)->elements[i]);
    }
    array_list_delete(array_list_pointer);
}

void array_list_add(ArrayList* array_list, int index, const void* element) {
    require_non_null(array_list);
    if (index < 0 || index > array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
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
        const int new_capacity = (int) (array_list->capacity * array_list->grow_factor);
        if (!resize(array_list, new_capacity)) {
            raise_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'array_list' capacity");
        }
    }
    for (int i = array_list->size; i > index; i--) {
        array_list->elements[i] = array_list->elements[i - 1];
    }
    array_list->elements[index] = (void*) element;
    array_list->size++;
}

void array_list_add_first(ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    const Error error = attempt(array_list_add(array_list, 0, element));
    if (error) {
        raise_error(error, "%s", plain_error_message());
    }
}

void array_list_add_last(ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    const Error error = attempt(array_list_add(array_list, array_list->size, element));
    if (error) {
        raise_error(error, "%s", plain_error_message());
    }
}

void array_list_add_all(ArrayList* array_list, int index, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    if (index < 0 || index > array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
    }
    Error error = NO_ERROR;

    Iterator* iterator;
    if ((error = attempt(iterator = collection_iterator(collection))) == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s of 'collection'", plain_error_message());
    }

    const int new_capacity = array_list->size + collection_size(collection);
    if ((error = attempt(array_list_ensure_capacity(array_list, new_capacity))) == MEMORY_ALLOCATION_ERROR) {
        iterator_delete(&iterator);
        raise_error(error, "%s", plain_error_message());
    }

    const int offset = collection_size(collection);
    for (int i = array_list->size - 1; i >= index; i--) {
        array_list->elements[i + offset] = array_list->elements[i];
    }

    while (iterator_has_next(iterator)) {
        array_list->elements[index++] = iterator_next(iterator);
    }
    array_list->size += collection_size(collection);

    iterator_delete(&iterator);
}

void array_list_add_all_first(ArrayList* array_list, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    const Error error = attempt(array_list_add_all(array_list, 0, collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s", plain_error_message());
    }
}

void array_list_add_all_last(ArrayList* array_list, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    const Error error = attempt(array_list_add_all(array_list, array_list->size, collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s", plain_error_message());
    }
}

void* array_list_get(const ArrayList* array_list, int index) {
    require_non_null(array_list);
    if (index < 0 || index >= array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size) nullptr;
    }
    return array_list->elements[index];
}

void* array_list_get_first(const ArrayList* array_list) {
    require_non_null(array_list);
    if (array_list->size == 0) {
        raise_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty") nullptr;
    }
    return array_list->elements[0];
}

void* array_list_get_last(const ArrayList* array_list) {
    require_non_null(array_list);
    if (array_list->size == 0) {
        raise_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty") nullptr;
    }
    return array_list->elements[array_list->size - 1];
}

void* array_list_set(ArrayList* array_list, int index, const void* element) {
    require_non_null(array_list);
    if (index < 0 || index >= array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size) nullptr;
    }
    void* old_element = array_list->elements[index];
    array_list->elements[index] = (void*) element;
    return old_element;
}

void array_list_swap(ArrayList* array_list, int index_a, int index_b) {
    require_non_null(array_list);
    if (index_a < 0 || index_a >= array_list->size || index_b < 0 || index_b >= array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index_a = %d, index_b = %d, size = %d", index_a, index_b, array_list->size);
    }
    swap(&array_list->elements[index_a], &array_list->elements[index_b]);
}

void* array_list_remove(ArrayList* array_list, int index) {
    require_non_null(array_list);
    if (index < 0 || index >= array_list->size) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size) nullptr;
    }
    void* element = array_list->elements[index];
    array_list->elements[index] = nullptr;

    for (int i = index; i < array_list->size - 1; i++) {
        array_list->elements[i] = array_list->elements[i + 1];
    }
    array_list->size--;
    return element;
}

void* array_list_remove_first(ArrayList* array_list) {
    require_non_null(array_list);
    if (array_list->size == 0) {
        raise_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty") nullptr;
    }
    return array_list_remove(array_list, 0);
}

void* array_list_remove_last(ArrayList* array_list) {
    require_non_null(array_list);
    if (array_list->size == 0) {
        raise_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty") nullptr;
    }
    return array_list_remove(array_list, array_list->size - 1);
}

bool array_list_remove_element(ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    const int index = array_list_index_of(array_list, element);
    if (index >= 0) {
        array_list_remove(array_list, index);
        return true;
    }
    return false;
}

int array_list_remove_all(ArrayList* array_list, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s of 'collection'", plain_error_message()) 0;
    }
    int count = 0;
    while (iterator_has_next(iterator)) {
        if (array_list_remove_element(array_list, iterator_next(iterator))) {
            count++;
        }
    }
    iterator_delete(&iterator);
    return count;
}

int array_list_remove_range(ArrayList* array_list, int start_index, int end_index) {
    require_non_null(array_list);
    if (start_index < 0 || end_index > array_list->size || start_index > end_index) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "start_index = %d, end_index = %d, size = %d", start_index, end_index, array_list->size) 0;
    }
    const int count = end_index - start_index;
    for (int i = start_index; i < array_list->size - count; i++) {
        array_list->elements[i] = array_list->elements[i + count];
        array_list->elements[i + count] = nullptr;
    }
    array_list->size -= count;
    return count;
}

int array_list_remove_if(ArrayList* array_list, Predicate condition_matches) {
    require_non_null(array_list, condition_matches);
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
    require_non_null(array_list, operator_apply);
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = operator_apply(array_list->elements[i]);
    }
}

int array_list_retain_all(ArrayList* array_list, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s of 'collection'", plain_error_message()) 0;
    }
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
    iterator_delete(&iterator);
    return count;
}

int array_list_size(const ArrayList* array_list) {
    require_non_null(array_list);
    return array_list->size;
}

void array_list_trim_to_size(ArrayList* array_list) {
    require_non_null(array_list);
    if (!resize(array_list, array_list->size)) {
        raise_error(MEMORY_ALLOCATION_ERROR, "failed to shrink 'array_list' capacity");
    }
}

int array_list_capacity(const ArrayList* array_list) {
    require_non_null(array_list);
    return array_list->capacity;
}

void array_list_ensure_capacity(ArrayList* array_list, int capacity) {
    require_non_null(array_list);
    if (array_list->capacity >= capacity) {
        return;
    }
    int new_capacity = array_list->capacity;
    while (new_capacity < capacity) {
        new_capacity = (int) (new_capacity * array_list->grow_factor);
    }
    if (!resize(array_list, new_capacity)) {
        raise_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'array_list' capacity");
    }
}

bool array_list_is_empty(const ArrayList* array_list) {
    require_non_null(array_list);
    return array_list->size == 0;
}

Iterator* array_list_iterator(const ArrayList* array_list) {
    require_non_null(array_list);
    Iterator* iterator = create_iterator(array_list);
    if (!iterator) {
        raise_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'") nullptr;
    }
    return iterator;
}

bool array_list_equals(const ArrayList* array_list, const ArrayList* another) {
    require_non_null(array_list, another);
    if (array_list == another) {
        return true;
    }
    if (array_list->size != another->size) {
        return false;
    }
    for (int i = 0; i < array_list->size; i++) {
        if (!array_list->equals(array_list->elements[i], another->elements[i])) {
            return false;
        }
    }
    return true;
}

void array_list_for_each(ArrayList* array_list, Consumer action) {
    require_non_null(array_list, action);
    for (int i = 0; i < array_list->size; i++) {
        action(array_list->elements[i]);
    }
}

void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm) {
    require_non_null(array_list, comparator);
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

void array_list_shuffle(ArrayList* array_list, int (*random)(void), ShufflingAlgorithm algorithm) {
    require_non_null(array_list, random);
    switch (algorithm) {
        case DURSTENFELD_SHUFFLE: { durstenfeld_shuffle(array_list, random); return; }
        case SATTOLO_SHUFFLE: { sattolo_shuffle(array_list, random); return; }
        case NAIVE_SHUFFLE: { naive_shuffle(array_list, random); }
    }
}

void array_list_reverse(ArrayList* array_list) {
    require_non_null(array_list);
    reverse(array_list, 0, array_list->size - 1);
}

void array_list_rotate(ArrayList* array_list, int distance) {
    require_non_null(array_list);
    if (array_list->size <= 1) return;

    distance %= array_list->size;
    if (distance < 0) {
        distance += array_list->size;
    }
    if (distance == 0) return;

    reverse(array_list, 0, array_list->size - 1);
    reverse(array_list, 0, distance - 1);
    reverse(array_list, distance, array_list->size - 1);
}

void array_list_clear(ArrayList* array_list) {
    require_non_null(array_list);
    for (int i = 0; i < array_list->size; i++) {
        array_list->elements[i] = nullptr;
    }
    array_list->size = 0;
}

void array_list_clear_data(ArrayList* array_list, void (*delete)(void*)) {
    require_non_null(array_list, delete);
    for (int i = 0; i < array_list->size; i++) {
        delete(array_list->elements[i]);
        array_list->elements[i] = nullptr;
    }
    array_list->size = 0;
}

Optional array_list_find(const ArrayList* array_list, Predicate condition) {
    require_non_null(array_list, condition);
    for (int i = 0; i < array_list->size; i++) {
        void* element = array_list->elements[i];
        if (condition(element)) {
            return optional_of(element);
        }
    }
    return optional_empty();
}

Optional array_list_find_last(const ArrayList* array_list, Predicate condition) {
    require_non_null(array_list, condition);
    void* element = nullptr;
    for (int i = 0; i < array_list->size; i++) {
        void* current = array_list->elements[i];
        if (condition(current)) {
            element = current;
        }
    }
    return element ? optional_of(element) : optional_empty();
}

int array_list_index_where(const ArrayList* array_list, Predicate condition) {
    require_non_null(array_list, condition);
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_where(const ArrayList* array_list, Predicate condition) {
    require_non_null(array_list, condition);
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            last_index = i;
        }
    }
    return last_index;
}

bool array_list_contains(const ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    return array_list_index_of(array_list, element) != -1;
}

bool array_list_contains_all(const ArrayList* array_list, Collection collection) {
    require_non_null(array_list); require_non_empty_collection(collection);

    Iterator* iterator; const Error error = attempt(iterator = collection_iterator(collection));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s of 'collection'", plain_error_message()) false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!array_list_contains(array_list, element)) {
            contains = false;
            break;
        }
    }
    iterator_delete(&iterator);
    return contains;
}

int array_list_occurrences_of(const ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    int count = 0;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            count++;
        }
    }
    return count;
}

int array_list_index_of(const ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_of(const ArrayList* array_list, const void* element) {
    require_non_null(array_list);
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            last_index = i;
        }
    }
    return last_index;
}

int array_list_binary_search(const ArrayList* array_list, const void* element, Comparator compare) {
    require_non_null(array_list, compare);

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
    require_non_null(array_list);
    ArrayList* new_array_list; const Error error = attempt(new_array_list = array_list_sub_list(array_list, 0, array_list->size));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s", plain_error_message()) nullptr;
    }
    return new_array_list;
}

ArrayList* array_list_sub_list(const ArrayList* array_list, int start_index, int end_index) {
    require_non_null(array_list);
    if (start_index < 0 || end_index > array_list->size || start_index > end_index) {
        raise_error(INDEX_OUT_OF_BOUNDS_ERROR, "start_index = %d, end_index = %d, size = %d", start_index, end_index, array_list->size) nullptr;
    }
    ArrayList* new_array_list; const Error error = attempt(new_array_list = array_list_new(&(ArrayListOptions) {
        .initial_capacity = end_index - start_index < MIN_CAPACITY ? MIN_CAPACITY : end_index - start_index,
        .grow_factor = array_list->grow_factor,
        .equals = array_list->equals,
        .to_string = array_list->to_string,
        .memory_alloc = array_list->memory_alloc,
        .memory_realloc = array_list->memory_realloc,
        .memory_free = array_list->memory_free
    }));
    if (error == MEMORY_ALLOCATION_ERROR) {
        raise_error(error, "%s", plain_error_message()) nullptr;
    }
    for (int i = start_index; i < end_index; i++) {
        array_list_add_last(new_array_list, array_list->elements[i]);
    }
    return new_array_list;
}

Collection array_list_to_collection(const ArrayList* array_list) {
    require_non_null(array_list);
    return collection_from((void*) array_list);
}

void** array_list_to_array(const ArrayList* array_list) {
    require_non_null(array_list);
    void** elements = array_list->memory_alloc(sizeof(void*) * array_list->size);
    if (!elements) {
        raise_error(MEMORY_ALLOCATION_ERROR, "") nullptr;
    }
    for (int i = 0; i < array_list->size; i++) {
        elements[i] = array_list->elements[i];
    }
    return elements;
}

char* array_list_to_string(const ArrayList* array_list) {
    require_non_null(array_list);
    char* string = array_list->memory_alloc(calculate_string_size(array_list));

    if (!string) {
        raise_error(MEMORY_ALLOCATION_ERROR, "") nullptr;
    }

    string[0] = '\0'; // initialize string to clear trash data

    strcat(string, array_list->size == 0 ? "[" : "[ ");
    for (int i = 0; i < array_list->size; i++) {
        constexpr int NULL_TERMINATOR = 1;

        const int length = array_list->to_string(array_list->elements[i], nullptr, 0) + NULL_TERMINATOR;
        char* element_string = array_list->memory_alloc(length);

        if (!element_string) {
            array_list->memory_free(string);
            raise_error(MEMORY_ALLOCATION_ERROR, "") nullptr;
        }

        array_list->to_string(array_list->elements[i], element_string, length);
        strcat(string, element_string);

        if (i < array_list->size - 1) {
            strcat(string, ", ");
        }
        array_list->memory_free(element_string);
    }
    strcat(string, array_list->size == 0 ? "]" : " ]");

    return string;
}

static size_t calculate_string_size(const ArrayList* array_list) {
    constexpr int BRACKETS = 2; constexpr int SEPARATOR = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0; i < array_list->size; i++) {
        length += array_list->to_string(array_list->elements[i], nullptr, 0);

        if (i == 0) length += 1; // space after opening bracket
        if (i < array_list->size - 1) length += SEPARATOR; // prevent separator on the last element
        if (i == array_list->size - 1) length += 1; // space before closing bracket
    }
    return length + BRACKETS + NULL_TERMINATOR;
}

static bool resize(ArrayList* array_list, int new_capacity) {
    new_capacity = new_capacity < MIN_CAPACITY ? MIN_CAPACITY : new_capacity;
    new_capacity = new_capacity > MAX_CAPACITY ? MAX_CAPACITY : new_capacity;

    void** elements = array_list->memory_realloc(array_list->elements, new_capacity * sizeof(void*));
    if (!elements) {
        return false;
    }
    array_list->elements = elements;
    array_list->capacity = new_capacity;
    return true;
}

struct IterationContext {
    const ArrayList* array_list;
    int cursor;
};

static Iterator* create_iterator(const ArrayList* array_list) {
    IterationContext* iteration_context = array_list->memory_alloc(sizeof(IterationContext));
    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->array_list = array_list;
    iteration_context->cursor = 0;

    Iterator* iterator = iterator_from(array_list, iteration_context, has_next, next, reset);
    if (!iterator) {
        array_list->memory_free(iteration_context);
        return nullptr;
    }
    return iterator;
}

static bool has_next(const IterationContext* iteration_context) {
    return iteration_context->cursor < iteration_context->array_list->size;
}

static void* next(IterationContext* iteration_context) {
    if (has_next(iteration_context)) {
        return iteration_context->array_list->elements[iteration_context->cursor++];
    }
    raise_error(NO_SUCH_ELEMENT_ERROR, "") nullptr;
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

static void durstenfeld_shuffle(ArrayList* array_list, int (*random)(void)) {
    for (int i = array_list->size - 1; i > 0; i--) {
        const int j = random() % (i + 1);
        swap(&array_list->elements[i], &array_list->elements[j]);
    }
}

static void sattolo_shuffle(ArrayList* array_list, int (*random)(void)) {
    for (int i = array_list->size - 1; i > 0; i--) {
        const int j = random() % i;
        swap(&array_list->elements[i], &array_list->elements[j]);
    }
}

static void naive_shuffle(ArrayList* array_list, int (*random)(void)) {
    for (int i = 0; i < array_list->size; i++) {
        const int j = random() % array_list->size;
        swap(&array_list->elements[i], &array_list->elements[j]);
    }
}

static void reverse(ArrayList* array_list, int start_index, int end_index) {
    while (start_index < end_index) {
        void* temporary = array_list->elements[start_index];
        array_list->elements[start_index] = array_list->elements[end_index];
        array_list->elements[end_index] = temporary;
        start_index++;
        end_index--;
    }
}

static void swap(void** a, void** b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}