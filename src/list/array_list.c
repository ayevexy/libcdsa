#include "array_list.h"

#include "util/errors.h"
#include "util/constraints.h"
#include <string.h>

constexpr int MIN_CAPACITY = 10;
constexpr int MAX_CAPACITY = 1'073'741'824;
constexpr float MIN_GROWTH_FACTOR = 1.10f;

struct ArrayList {
    void** elements;
    int size;
    int capacity;
    float growth_factor;
    struct {
        void (*destruct)(void*);
        bool (*equals)(const void*, const void*);
        int (*to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void* (*memory_realloc)(void*, size_t);
        void (*memory_dealloc)(void*);
    };
    int modification_count;
};

static size_t calculate_string_size(const ArrayList*);

static bool ensure_capacity(ArrayList*, int);

static bool resize(ArrayList*, int);

static Iterator* create_iterator(const ArrayList*);

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static bool iterator_has_previous_internal(const void*);

static void* iterator_previous_internal(void*);

static void iterator_add_internal(void*, const void*);

static void iterator_set_internal(void*, const void*);

static void iterator_remove_internal(void*);

static void iterator_reset_internal(void*);

static void bubble_sort(ArrayList*, Comparator);

static void selection_sort(ArrayList*, Comparator);

static void insertion_sort(ArrayList*, Comparator);

static void merge_sort(ArrayList*, int, int, Comparator);

static void merge(ArrayList*, int, int, int, Comparator);

static void quick_sort(ArrayList*, int, int, Comparator);

static int partition(ArrayList*, int, int, Comparator);

static void durstenfeld_shuffle(ArrayList*, int (*random)(void));

static void sattolo_shuffle(ArrayList*, int (*random)(void));

static void naive_shuffle(ArrayList*, int (*random)(void));

static void reverse(ArrayList*, int, int);

static void swap(void** a, void** b);

static int collection_size_internal(const void*);

static Iterator* collection_iterator_internal(const void*);

static bool collection_contains_internal(const void*, const void*);

ArrayList* array_list_new(const ArrayListOptions* options) {
    if (require_non_null(options)) return nullptr;
    if (options->initial_capacity < MIN_CAPACITY || options->initial_capacity > MAX_CAPACITY
        || options->growth_factor < MIN_GROWTH_FACTOR || !options->destruct || !options->equals
        || !options->to_string || !options->memory_alloc || !options->memory_dealloc
    ) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'options' argument must adhere to its constraints");
        return nullptr;
    }
    ArrayList* array_list = options->memory_alloc(sizeof(ArrayList));
    if (!array_list) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array_list'");
        return nullptr;
    }
    array_list->elements = options->memory_alloc(options->initial_capacity * sizeof(void*));
    if (!array_list->elements) {
        options->memory_dealloc(array_list);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array_list'");
        return nullptr;
    }
    array_list->size = 0;
    array_list->capacity = options->initial_capacity;
    array_list->growth_factor = options->growth_factor;
    array_list->destruct = options->destruct;
    array_list->equals = options->equals;
    array_list->to_string = options->to_string;
    array_list->memory_alloc = options->memory_alloc;
    array_list->memory_realloc = options->memory_realloc;
    array_list->memory_dealloc = options->memory_dealloc;
    array_list->modification_count = 0;
    return array_list;
}

ArrayList* array_list_from(Collection collection, const ArrayListOptions* options) {
    if (require_non_null(options)) return nullptr;
    ArrayList* array_list; Error error;

    if ((error = attempt(array_list = array_list_new(options)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    if ((error = attempt(array_list_add_all_last(array_list, collection)))) {
        array_list_destroy(&array_list);
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return array_list;
}

void array_list_destroy(ArrayList** array_list_pointer) {
    if (require_non_null(array_list_pointer, *array_list_pointer)) return;
    ArrayList* array_list = *array_list_pointer;
    for (int i = 0; i < array_list->size; i++) {
        array_list->destruct(array_list->elements[i]);
    }
    array_list->memory_dealloc(array_list->elements);
    array_list->memory_dealloc(array_list);
    *array_list_pointer = nullptr;
}

void array_list_set_destructor(ArrayList* array_list, void (*destruct)(void*)) {
    if (require_non_null(array_list, destruct)) return;
    array_list->destruct = destruct;
}

void array_list_add(ArrayList* array_list, int index, const void* element) {
    if (require_non_null(array_list)) return;
    if (index < 0 || index > array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
        return;
    }
    if (!ensure_capacity(array_list, array_list->size + 1)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'array_list' capacity");
        return;
    }
    for (int i = array_list->size; i > index; i--) {
        array_list->elements[i] = array_list->elements[i - 1];
    }
    array_list->elements[index] = (void*) element;
    array_list->size++;
    array_list->modification_count++;
}

void array_list_add_first(ArrayList* array_list, const void* element) {
    array_list_add(array_list, 0, element);
}

void array_list_add_last(ArrayList* array_list, const void* element) {
    array_list_add(array_list, array_list ? array_list->size : -1, element);
}

void array_list_add_all(ArrayList* array_list, int index, Collection collection) {
    if (require_non_null(array_list)) return;
    if (index < 0 || index > array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
        return;
    }
    Iterator* iterator; Error error;
    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return;
    }
    if (!ensure_capacity(array_list, array_list->size + collection_size(collection))) {
        iterator_destroy(&iterator);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'array_list' capacity");
        return;
    }
    const int offset = collection_size(collection);
    for (int i = array_list->size - 1; i >= index; i--) {
        array_list->elements[i + offset] = array_list->elements[i];
    }
    while (iterator_has_next(iterator)) {
        array_list->elements[index++] = iterator_next(iterator);
    }
    array_list->size += collection_size(collection);
    array_list->modification_count++;
    iterator_destroy(&iterator);
}

void array_list_add_all_first(ArrayList* array_list, Collection collection) {
    array_list_add_all(array_list, 0, collection);
}

void array_list_add_all_last(ArrayList* array_list, Collection collection) {
    array_list_add_all(array_list, array_list ? array_list->size : -1, collection);
}

void* array_list_get(const ArrayList* array_list, int index) {
    if (require_non_null(array_list)) return nullptr;
    if (index < 0 || index >= array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
        return nullptr;
    }
    return array_list->elements[index];
}

void* array_list_get_first(const ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    if (array_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty");
        return nullptr;
    }
    return array_list->elements[0];
}

void* array_list_get_last(const ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    if (array_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty");
        return nullptr;
    }
    return array_list->elements[array_list->size - 1];
}

void* array_list_set(ArrayList* array_list, int index, const void* element) {
    if (require_non_null(array_list)) return nullptr;
    if (index < 0 || index >= array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
        return nullptr;
    }
    void* old_element = array_list->elements[index];
    if (old_element != element) {
        array_list->destruct(old_element);
    }
    array_list->elements[index] = (void*) element;
    return old_element;
}

void array_list_swap(ArrayList* array_list, int index_a, int index_b) {
    if (require_non_null(array_list)) return;
    if (index_a < 0 || index_a >= array_list->size || index_b < 0 || index_b >= array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid indexes first = %d, second = %d; size = %d", index_a, index_b, array_list->size);
        return;
    }
    swap(&array_list->elements[index_a], &array_list->elements[index_b]);
}

void* array_list_remove(ArrayList* array_list, int index) {
    if (require_non_null(array_list)) return nullptr;
    if (index < 0 || index >= array_list->size) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "index %d out of bounds for length %d", index, array_list->size);
        return nullptr;
    }
    void* element = array_list->elements[index];
    array_list->destruct(element);
    array_list->elements[index] = nullptr;

    for (int i = index; i < array_list->size - 1; i++) {
        array_list->elements[i] = array_list->elements[i + 1];
    }
    array_list->size--;
    array_list->modification_count++;
    return element;
}

void* array_list_remove_first(ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    if (array_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty");
        return nullptr;
    }
    return array_list_remove(array_list, 0);
}

void* array_list_remove_last(ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    if (array_list->size == 0) {
        set_error(NO_SUCH_ELEMENT_ERROR, "'array_list' is empty");
        return nullptr;
    }
    return array_list_remove(array_list, array_list->size - 1);
}

bool array_list_remove_element(ArrayList* array_list, const void* element) {
    if (require_non_null(array_list)) return false;
    const int index = array_list_index_of(array_list, element);
    if (index >= 0) {
        array_list_remove(array_list, index);
        return true;
    }
    return false;
}

int array_list_remove_all(ArrayList* array_list, Collection collection) {
    if (require_non_null(array_list)) return 0;
    int count = 0;
    for (int i = array_list->size - 1; i >= 0; i--) {
        if (collection_contains(collection, array_list->elements[i])) {
            array_list_remove(array_list, i);
            count++;
        }
    }
    return count;
}

int array_list_remove_range(ArrayList* array_list, int start_index, int end_index) {
    if (require_non_null(array_list)) return 0;
    if (start_index < 0 || end_index > array_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, end = %d; size = %d", start_index, end_index, array_list->size);
        return 0;
    }
    const int count = end_index - start_index;
    const int old_size = array_list->size;
    for (int i = start_index; i < old_size; i++) {
        if (i < end_index) {
            array_list->destruct(array_list->elements[i]);
        } else {
            array_list->elements[i - count] = array_list->elements[i];
        }
        if (i >= old_size - count) {
            array_list->elements[i] = nullptr;
        }
    }
    array_list->size -= count;
    array_list->modification_count++;
    return count;
}

int array_list_remove_if(ArrayList* array_list, Predicate condition) {
    if (require_non_null(array_list, condition)) return 0;
    int count = 0;
    for (int i = array_list->size - 1; i >= 0; i--) {
        if (condition(array_list->elements[i])) {
            array_list_remove(array_list, i);
            count++;
        }
    }
    return count;
}

void array_list_replace_all(ArrayList* array_list, Operator operator) {
    if (require_non_null(array_list, operator)) return;
    for (int i = 0; i < array_list->size; i++) {
        void* old_element = array_list->elements[i];
        void* new_element = operator(old_element);
        if (old_element != new_element) {
            array_list->destruct(old_element);
        }
        array_list->elements[i] = new_element;
    }
}

int array_list_retain_all(ArrayList* array_list, Collection collection) {
    if (require_non_null(array_list)) return 0;
    int count = 0;
    for (int i = array_list->size - 1; i >= 0; i--) {
        if (!collection_contains(collection, array_list->elements[i])) {
            array_list_remove(array_list, i);
            count++;
        }
    }
    return count;
}

int array_list_size(const ArrayList* array_list) {
    if (require_non_null(array_list)) return 0;
    return array_list->size;
}

void array_list_trim_to_size(ArrayList* array_list) {
    if (require_non_null(array_list)) return;
    if (!resize(array_list, array_list->size)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to shrink 'array_list' capacity");
        return;
    }
    array_list->modification_count++;
}

int array_list_capacity(const ArrayList* array_list) {
    if (require_non_null(array_list)) return 0;
    return array_list->capacity;
}

void array_list_ensure_capacity(ArrayList* array_list, int capacity) {
    if (require_non_null(array_list)) return;
    if (!ensure_capacity(array_list, capacity)) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to expand 'array_list' capacity");
        return;
    }
    array_list->modification_count++;
}

bool array_list_is_empty(const ArrayList* array_list) {
    if (require_non_null(array_list)) return false;
    return array_list->size == 0;
}

Iterator* array_list_iterator(const ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    Iterator* iterator = create_iterator(array_list);
    if (!iterator) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'iterator'");
        return nullptr;
    }
    return iterator;
}

bool array_list_equals(const ArrayList* array_list, const ArrayList* other_array_list) {
    if (require_non_null(array_list, other_array_list)) return false;
    if (array_list == other_array_list) {
        return true;
    }
    if (array_list->size != other_array_list->size) {
        return false;
    }
    for (int i = 0; i < array_list->size; i++) {
        if (!array_list->equals(array_list->elements[i], other_array_list->elements[i])) {
            return false;
        }
    }
    return true;
}

void array_list_for_each(ArrayList* array_list, Consumer action) {
    if (require_non_null(array_list, action)) return;
    for (int i = 0; i < array_list->size; i++) {
        action(array_list->elements[i]);
    }
}

void array_list_sort(ArrayList* array_list, Comparator comparator, SortingAlgorithm algorithm) {
    if (require_non_null(array_list, comparator)) return;
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
    if (require_non_null(array_list, random)) return;
    switch (algorithm) {
        case DURSTENFELD_SHUFFLE: { durstenfeld_shuffle(array_list, random); return; }
        case SATTOLO_SHUFFLE: { sattolo_shuffle(array_list, random); return; }
        case NAIVE_SHUFFLE: { naive_shuffle(array_list, random); }
    }
}

void array_list_reverse(ArrayList* array_list) {
    if (require_non_null(array_list)) return;
    reverse(array_list, 0, array_list->size - 1);
}

void array_list_rotate(ArrayList* array_list, int distance) {
    if (require_non_null(array_list)) return;
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
    if (require_non_null(array_list)) return;
    for (int i = 0; i < array_list->size; i++) {
        array_list->destruct(array_list->elements[i]);
        array_list->elements[i] = nullptr;
    }
    array_list->size = 0;
    array_list->modification_count++;
}

Optional array_list_find(const ArrayList* array_list, Predicate condition) {
    if (require_non_null(array_list, condition)) return optional_empty();
    for (int i = 0; i < array_list->size; i++) {
        void* element = array_list->elements[i];
        if (condition(element)) {
            return optional_of(element);
        }
    }
    return optional_empty();
}

Optional array_list_find_last(const ArrayList* array_list, Predicate condition) {
    if (require_non_null(array_list, condition)) return optional_empty();
    void* element = nullptr;
    bool found = false;
    for (int i = 0; i < array_list->size; i++) {
        void* current = array_list->elements[i];
        if (condition(current)) {
            found = true;
            element = current;
        }
    }
    return found ? optional_of(element) : optional_empty();
}

int array_list_index_where(const ArrayList* array_list, Predicate condition) {
    if (require_non_null(array_list, condition)) return 0;
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_where(const ArrayList* array_list, Predicate condition) {
    if (require_non_null(array_list, condition)) return 0;
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (condition(array_list->elements[i])) {
            last_index = i;
        }
    }
    return last_index;
}

bool array_list_contains(const ArrayList* array_list, const void* element) {
    if (require_non_null(array_list)) return false;
    return array_list_index_of(array_list, element) != -1;
}

bool array_list_contains_all(const ArrayList* array_list, Collection collection) {
    if (require_non_null(array_list)) return false;
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!array_list_contains(array_list, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

int array_list_occurrences_of(const ArrayList* array_list, const void* element) {
    if (require_non_null(array_list)) return 0;
    int count = 0;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            count++;
        }
    }
    return count;
}

int array_list_index_of(const ArrayList* array_list, const void* element) {
    if (require_non_null(array_list)) return 0;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            return i;
        }
    }
    return -1;
}

int array_list_last_index_of(const ArrayList* array_list, const void* element) {
    if (require_non_null(array_list)) return 0;
    int last_index = -1;
    for (int i = 0; i < array_list->size; i++) {
        if (array_list->equals(array_list->elements[i], element)) {
            last_index = i;
        }
    }
    return last_index;
}

int array_list_binary_search(const ArrayList* array_list, const void* element, Comparator compare) {
    if (require_non_null(array_list, compare)) return 0;

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
    if (require_non_null(array_list)) return nullptr;
    ArrayList* new_array_list; Error error;

    if ((error = attempt(new_array_list = array_list_sub_list(array_list, 0, array_list->size)))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    return new_array_list;
}

ArrayList* array_list_sub_list(const ArrayList* array_list, int start_index, int end_index) {
    if (require_non_null(array_list)) return nullptr;
    if (start_index < 0 || end_index > array_list->size || start_index > end_index) {
        set_error(INDEX_OUT_OF_BOUNDS_ERROR, "invalid range start = %d, end = %d; size = %d", start_index, end_index, array_list->size);
        return nullptr;
    }
    ArrayList* new_array_list; Error error;
    if ((error = attempt(new_array_list = array_list_new(&(ArrayListOptions) {
        .initial_capacity = end_index - start_index < MIN_CAPACITY ? MIN_CAPACITY : end_index - start_index,
        .growth_factor = array_list->growth_factor,
        .destruct = noop_destruct,
        .equals = array_list->equals,
        .to_string = array_list->to_string,
        .memory_alloc = array_list->memory_alloc,
        .memory_realloc = array_list->memory_realloc,
        .memory_dealloc = array_list->memory_dealloc
    })))) {
        set_error(error, "%s", plain_error_message());
        return nullptr;
    }
    for (int i = start_index; i < end_index; i++) {
        array_list_add_last(new_array_list, array_list->elements[i]);
    }
    return new_array_list;
}

Collection array_list_to_collection(const ArrayList* array_list) {
    if (require_non_null(array_list)) return (Collection) {};
    return (Collection) {
        .data_structure = array_list,
        .size = collection_size_internal,
        .iterator = collection_iterator_internal,
        .contains = collection_contains_internal
    };
}

void** array_list_to_array(const ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;
    void** elements = array_list->memory_alloc(array_list->size * sizeof(void*));
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    for (int i = 0; i < array_list->size; i++) {
        elements[i] = array_list->elements[i];
    }
    return elements;
}

char* array_list_to_string(const ArrayList* array_list) {
    if (require_non_null(array_list)) return nullptr;

    char* string = array_list->memory_alloc(calculate_string_size(array_list));
    if (!string) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
        return nullptr;
    }
    string[0] = '\0'; // initialize string to ignore memory garbage
    strcat(string, array_list->size == 0 ? "[" : "[ ");

    for (int i = 0; i < array_list->size; i++) {
        constexpr int NULL_TERMINATOR = 1;
        const int length = array_list->to_string(array_list->elements[i], nullptr, 0) + NULL_TERMINATOR;

        char* element_string = array_list->memory_alloc(length);
        if (!element_string) {
            array_list->memory_dealloc(string);
            set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'string'");
            return nullptr;
        }
        array_list->to_string(array_list->elements[i], element_string, length);
        strcat(string, element_string);

        if (i < array_list->size - 1) {
            strcat(string, ", ");
        }
        array_list->memory_dealloc(element_string);
    }

    strcat(string, array_list->size == 0 ? "]" : " ]");
    return string;
}

static size_t calculate_string_size(const ArrayList* array_list) {
    constexpr int BRACKETS = 2; constexpr int COMMA_SPACE = 2; constexpr int NULL_TERMINATOR = 1;
    size_t length = 0;

    for (int i = 0; i < array_list->size; i++) {
        length += array_list->to_string(array_list->elements[i], nullptr, 0);

        if (i == 0) length += 1; // space after opening bracket
        if (i < array_list->size - 1) length += COMMA_SPACE; // prevent ", " on the last element
        if (i == array_list->size - 1) length += 1; // space before closing bracket
    }
    return length + BRACKETS + NULL_TERMINATOR;
}

static bool ensure_capacity(ArrayList* array_list, int capacity) {
    if (array_list->capacity >= capacity) {
        return true;
    }
    int new_capacity = array_list->capacity;
    while (new_capacity < capacity) {
        if (array_list->growth_factor > MAX_CAPACITY / new_capacity) {
            return false;
        }
        new_capacity *= array_list->growth_factor;
    }
    return resize(array_list, new_capacity);
}

static bool resize(ArrayList* array_list, int new_capacity) {
    new_capacity = new_capacity < MIN_CAPACITY ? MIN_CAPACITY : new_capacity;
    new_capacity = new_capacity > MAX_CAPACITY ? MAX_CAPACITY : new_capacity;

    void** elements;
    if (array_list->memory_realloc) {
        elements = array_list->memory_realloc(array_list->elements, new_capacity * sizeof(void*));
        if (!elements) return false;
    } else {
        elements = array_list->memory_alloc(new_capacity * sizeof(void*));
        if (!elements) return false;
        memcpy(elements, array_list->elements, array_list->size * sizeof(void*));
        array_list->memory_dealloc(array_list->elements);
    }
    array_list->elements = elements;
    array_list->capacity = new_capacity;
    return true;
}

typedef struct {
    Iterator iterator;
    ArrayList* array_list;
    int cursor;
    int last_returned;
    int modification_count;
}  IterationContext;

static Iterator* create_iterator(const ArrayList* array_list) {
    IterationContext* iteration_context = array_list->memory_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator.iteration_context = iteration_context;
    iteration_context->iterator.has_next = iterator_has_next_internal;
    iteration_context->iterator.next = iterator_next_internal;
    iteration_context->iterator.has_previous = iterator_has_previous_internal;
    iteration_context->iterator.previous = iterator_previous_internal;

    iteration_context->iterator.add = iterator_add_internal;
    iteration_context->iterator.set = iterator_set_internal;
    iteration_context->iterator.remove = iterator_remove_internal;
    iteration_context->iterator.reset = iterator_reset_internal;
    iteration_context->iterator.memory_dealloc = array_list->memory_dealloc;

    iteration_context->array_list = (ArrayList*) array_list;
    iteration_context->cursor = 0;
    iteration_context->last_returned = -1;
    iteration_context->modification_count = array_list->modification_count;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->cursor < iteration_context->array_list->size;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->array_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return nullptr;
    }
    if (!iterator_has_next_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    iteration_context->last_returned = iteration_context->cursor;
    return iteration_context->array_list->elements[iteration_context->cursor++];
}

static bool iterator_has_previous_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    return iteration_context->cursor > 0;
}

static void* iterator_previous_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->array_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return nullptr;
    }
    if (!iterator_has_previous_internal(iteration_context)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "iterator has no more elements");
        return nullptr;
    }
    iteration_context->last_returned = --iteration_context->cursor;
    return iteration_context->array_list->elements[iteration_context->cursor];
}

static void iterator_add_internal(void* raw_iteration_context, const void* element) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->array_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return;
    }
    array_list_add(iteration_context->array_list, iteration_context->cursor++, element);
    iteration_context->last_returned = -1;
    iteration_context->modification_count = iteration_context->array_list->modification_count;
}

static void iterator_set_internal(void* raw_iteration_context, const void* element) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->array_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return;
    }
    if (iteration_context->last_returned < 0) {
        set_error(ILLEGAL_STATE_ERROR, "set() called before any next() or previous() call");
        return;
    }
    array_list_set(iteration_context->array_list, iteration_context->last_returned, element);
}

static void iterator_remove_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->modification_count != iteration_context->array_list->modification_count) {
        set_error(CONCURRENT_MODIFICATION_ERROR, "collection modified while iterator is active");
        return;
    }
    if (iteration_context->last_returned < 0) {
        set_error(ILLEGAL_STATE_ERROR, "remove() called twice or before any next() or previous() call");
        return;
    }
    array_list_remove(iteration_context->array_list, iteration_context->last_returned);
    if (iteration_context->cursor > iteration_context->last_returned) {
        iteration_context->cursor--;
    }
    iteration_context->last_returned = -1;
    iteration_context->modification_count = iteration_context->array_list->modification_count;
}

static void iterator_reset_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    iteration_context->cursor = 0;
    iteration_context->last_returned = -1;
    iteration_context->modification_count = iteration_context->array_list->modification_count;
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

        merge(array_list, start_index, middle_index, end_index, compare);
    }
}

// TODO: replace by a single temporary array to avoid to many allocations
static void merge(ArrayList* array_list, int start_index, int middle_index, int end_index, Comparator compare) {
    const int LEFT_ELEMENTS_SIZE = middle_index - start_index + 1;
    const int RIGHT_ELEMENTS_SIZE = end_index - middle_index;

    void** left_elements = array_list->memory_alloc(LEFT_ELEMENTS_SIZE * sizeof(void*));
    void** right_elements = array_list->memory_alloc(RIGHT_ELEMENTS_SIZE * sizeof(void*));

    if (!left_elements || !right_elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate temporary arrays for merge");
        free(left_elements);
        free(right_elements);
        return;
    }
    for (int i = 0; i < LEFT_ELEMENTS_SIZE; i++) {
        left_elements[i] = array_list->elements[start_index + i];
    }
    for (int i = 0; i < RIGHT_ELEMENTS_SIZE; i++) {
        right_elements[i] = array_list->elements[middle_index + 1 + i];
    }
    int left_elements_index = 0, right_elements_index = 0, index = start_index;

    while (left_elements_index < LEFT_ELEMENTS_SIZE && right_elements_index < RIGHT_ELEMENTS_SIZE) {
        if (compare(left_elements[left_elements_index], right_elements[right_elements_index]) <= 0) {
            array_list->elements[index] = left_elements[left_elements_index];
            left_elements_index++;
        } else {
            array_list->elements[index] = right_elements[right_elements_index];
            right_elements_index++;
        }
        index++;
    }
    while (left_elements_index < LEFT_ELEMENTS_SIZE) {
        array_list->elements[index] = left_elements[left_elements_index];
        left_elements_index++;
        index++;
    }
    while (right_elements_index < RIGHT_ELEMENTS_SIZE) {
        array_list->elements[index] = right_elements[right_elements_index];
        right_elements_index++;
        index++;
    }
    array_list->memory_dealloc(left_elements);
    array_list->memory_dealloc(right_elements);
}

static void quick_sort(ArrayList* array_list, int start_index, int end_index, Comparator compare) {
    if (start_index < end_index) {
        const int pivot = partition(array_list, start_index, end_index, compare);

        quick_sort(array_list, start_index, pivot - 1, compare);
        quick_sort(array_list, pivot + 1, end_index, compare);
    }
}

static int partition(ArrayList* array_list, int start_index, int end_index, Comparator compare) {
    int pivot = start_index - 1;

    for (int i = start_index; i < end_index; i++) {
        if (compare(array_list->elements[i], array_list->elements[end_index]) <= 0) {
            pivot++;
            swap(&array_list->elements[i], &array_list->elements[pivot]);
        }
    }
    swap(&array_list->elements[pivot + 1], &array_list->elements[end_index]);
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

static int collection_size_internal(const void* array_list) {
    return array_list_size(array_list);
}

static Iterator* collection_iterator_internal(const void* array_list) {
    return array_list_iterator(array_list);
}

static bool collection_contains_internal(const void* array_list, const void* element) {
    return array_list_contains(array_list, element);
}