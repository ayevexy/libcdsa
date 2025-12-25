# Array List Data Structure

---
## Table of Contents
- [Overview](#overview)
- [Getting Started](#getting-started)
- [Error Handling](#error-handling)
- [Creating a New Array List](#creating-a-new-array-list)
- [Deleting an Existing Array List](#deleting-an-existing-array-list)
- [Adding Elements](#adding-elements)
- [Getting Elements](#getting-elements)
- [Setting Elements](#setting-elements)
- [Removing Elements](#removing-elements)
- [Size and Capacity](#size-and-capacity)
- [Checking Emptiness](#checking-emptiness)
- [Iterator](#iterator)
- [For-Each Action](#for-each-action)
- [Sorting](#sorting)
- [Searching](#searching)
- [Cloning](#cloning)
- [SubList](#sublist)
- [To Array](#to-array)
- [To Collection](#to-collection)
- [String Representation](#string-representation)

---

## Overview

### Array List Data Structure

An array list is a linear, generic, and dynamic data structure that stores data contiguously in memory.
It automatically grows to accommodate new elements and allows insertion of values of any type.

Internally, the implementation uses a struct containing an array of void pointers. All operations on an array list
receive a pointer to the array list itself as their first argument. The `ArrayList` type is read-only to the client and can
only be modified through the provided API.

Underlying implementation (hidden behind a typedef):

```c++
struct ArrayList {
    void** elements;
    int size;
    // Other members omitted
};
```

---

## Getting Started

After linking the library to your project, include the following header:

```c++
#include "list/array_list.h" // <libcdsa/list/array_list.h> if installed system-wide
```

Now, all the functions and types of this module will be accessible to your code.

---

### Error Handling

All operations that take indexes as parameters fail if those are negative or
greater than the array list size, but the program will continue running normally and
a warning will be printed to `stderr`:

```c++
array_list_add_at(array_list, &value, -1); // "Warning: array_list_add_at index -1 out of bounds
```

---

### Creating a New Array List

To create a new `ArrayList`, call:

```c++
ArrayList* array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS);
```

The function takes an `ArrayListOptions` object that defines the list’s behavior. 
You may supply custom options or use the default:

```c++
#define DEFAULT_ARRAY_LIST_OPTIONS (ArrayListOptions) {     \
    .initial_capacity = 10,                                 \
    .grow_factor = 2.0,                                     \
    .equals = pointer_equals,                               \
    .to_string = pointer_to_string                          \
}
```

For an options object to be valid it has to constraint the following rules:
- `initial_capacity >= 10`
- `grow_factor >= 1.1`
- `equals != nullptr`
- `to_string != nullptr`

The function returns a pointer to a newly allocated `ArrayList` or null if options are invalid.

---

To initialize an array list from an existing `Collection`:

```c++
ArrayList* array_list = array_list_from(collection, DEFAULT_ARRAY_LIST_OPTIONS);
```

See [To Collection](#to-collection) for more details about the `Collection` type.

---

### Deleting an Existing Array List

Once you are done using an array list, free its allocated memory:

```c++
array_list_delete(array_list);
```

This safely releases the memory used back to the operating system.

---

### Adding Elements

To append an element to the end of the list:

```c++
// list: [ 1, 2, 3, 4 ]
int value = 8;
array_list_add(array_list, &value); // [ 1, 2, 3, 4, 8 ]
```

To insert an element at a specific index:

```c++
// list: [ 1, 2, 3, 4 ]
int value = 8;
array_list_add_at(array_list, &value, 2); // [1, 2, 8, 3, 4 ]
```

To insert an element at the beginning of the list:

```c++
// list: [ 1, 2, 3, 4 ]
int value = 8;
array_list_add_first(array_list, &value); // [ 8, 1, 2, 3, 4 ]
```

---

### Adding Multiple Elements

To add all elements from a `Collection`:

```c++
// collection: [ 1, 2, 3, 4 ]
array_list_add_all(array_list, collection); // [ 1, 2, 3, 4 ]
```

To insert all elements from a `Collection` starting at a specific index:

```c++
// list: [ 1, 2, 3, 4 ]
// collection: [ 5, 6 ]
array_list_add_all_at(array_list, 2, collection); // [ 1, 2, 5, 6, 3, 4]
```

---

### Getting Elements

To retrieve an element by index:

```c++
// list: [ 1, 2, 3, 4 ]
void* element = array_list_get(array_list, 2); // 3
```

Then, cast it back to the original type:

```c++
int value = *(int*) element; // 3
```

Or more concisely:

```c++
int value = *(int*) array_list_get(array_list, 2); // 3
```

---

To retrieve the last element of the list:

```c++
// list: [ 1, 2, 3, 4 ]
int value = *(int*) array_list_get_last(array_list); // 4
```

To retrieve the first element of the list:

```c++
// list: [ 1, 2, 3, 4 ]
int value = *(int*) array_list_get_first(array_list); // 1
```

---

### Setting Elements

To replace an element at a specific index:

```c++
// list = [ 1, 2, 3, 4 ]
int new_value = 20;
array_list_set(array_list, 0, &new_value); // [ 20, 2, 3, 4 ]
```

---

### Removing Elements

To remove an element by index:

```c++
// list: [ 1, 2, 3, 4 ]
array_list_remove(array_list, 2); // [ 1, 2, 4 ]
```

To remove the last element of the list:

```c++
// list: [ 1, 2, 3, 4 ]
array_list_remove_last(array_list); // [ 1, 2, 3 ]
```

To remove the first element of the list:

```c++
// list: [ 1, 2, 3, 4 ]
array_list_remove_first(array_list); // [ 2, 3, 4 ]
```

To remove an element by its address:

```c++
array_list_remove_element(array_list, &value);
```

### Removing Multiple Elements

To clear the list, removing all elements:

```c++
// list: [ 1, 2, 3, 4 ]
array_list_clear(array_list); // [] 
```

To remove all elements present in a given `Collection`:

```c++
// list: [ 1, 2, 3, 4 ]
// collection: [ 2, 3 ]
array_list_remove_all(array_list, collection); // [ 3, 4]
```

To remove all elements from a start index (inclusive) to an end index (exclusive):

```c++
// list: [ 1, 2, 3, 4, 5 ]
array_list_remove_range(array_list, 1, 4); // [ 1, 5 ]
```

To remove all elements matching a predicate:

```c++
// list: [ 1, 2, 3, 4, 5, 6 ]
array_list_remove_if(array_list, is_odd); // [ 2, 4, 6 ]
```

Where the predicate is a function like the following:

```c++
bool is_odd(void* element) {
    return *(int*) element % 2 != 0;
}
```

To replace all elements using an operator function:

```c++
// list: [ 1, 2, 3, 4, 5, 6 ]
array_list_replace_all(array_list, replace_odd_by_next_even); // [ 2, 2, 4, 4, 6, 6 ]
```

Where the operator is a function like the following:

```c++
void* replace_odd_by_next_even(void* element) {
    if (is_odd(element)) {
        int* new_element = malloc(sizeof(int));
        *new_element = *(int*) element + 1;
        free(element); // you should free the replaced element here or the pointer will be lost, causing a memory leak
        return new_element;
    }
    return element;
}
```

To retain all elements present in a given `Collection` while removing all other elements:

```c++
array_list_retain_all(array_list, collection);
```

---

### Size and Capacity

To retrieve the current size:

```c++
int size = array_list_size(array_list); // 0 if empty
```

To retrieve the current capacity:

```c++
int capacity = array_list_capacity(array_list); // 10 if not grown yet
```

To trim the capacity to match current size:

```c++
array_list_trim_to_size(array_list);
```

To ensure the provided number of elements can be added, expanding the capacity if necessary:

```c++
//list capacity: 10
array_list_ensure_capacity(array_list, 25); // capacity: 40
```

---

### Checking Emptiness

Check whether the array list is empty:

```c++
bool empty = array_list_is_empty(array_list); // true or false
```

---

### Iterator

The API provides an Iterator abstraction for traversal.

Create an iterator:

```c++
Iterator* iterator = array_list_iterator(array_list);
```

Iterate through elements:

```c++
// list: [ 1, 2, 3, 4 ]
while(iterator_has_next(iterator)) {
    int value = *(int*) iterator_next(iterator);
    printf("%d", value); // 1, 2, 3, 4, respectively
}
```

After use, delete the iterator to free memory:

```c++
iterator_delete(iterator);
```

---

### For-Each Action

To apply an action for every element:

```c++
// list: [ 1, 2, 3, 4 ]
array_list_for_each(array_list, action); // 1 2 3 4
```

Where `action` is a function like the following:

```c++
void action(void* element) {
    printf("%d ", *(int*) element);
}
```

---

### Sorting

To sort an array list:

```c++
// list: [ 3, 1, 4, 2 ]
array_list_sort(array_list, DEFAULT_COMPARATOR(int), BUBBLE_SORT); // [ 1, 2, 3, 4 ]
```

It takes a `Comparator` and a `SortingAlgorithm` as arguments.

The library provides default comparators for the following data types:
`char`, `int`, `long`, `float`, `double`,`void*` and `string`. They can be used through the 
`DEFAULT_COMPARATOR(T)` macro. You can provide your own `Comparator` too:

```c++
int compare(void* previous_element, void* next_element) {
    // return > 0 if previous > next
    // return 0 if equal
    // return < 0 if previous < next
}
```

The supported algorithms are: `BUBBLE_SORT`, `SELECTION_SORT`, `INSERTION_SORT`, `MERGE_SORT` and `QUICK_SORT`.

---

To reverse an array list:

```c++
// list: [ 1, 2, 3, 4, 5 ]
array_list_reverse(array_list); // [ 5, 4, 3, 2, 1 ]
```

---

### Searching

To verify the presence of an element:

```c++
bool contains = array_list_contains(array_list, &value); // true or false
```

To verify the presence of all elements present in a given `Collection`:

```c++
bool contains_all = array_list_contains_all(array_list, collection);
```

To retrieve the index of an element:

```c++
int index = array_list_index_of(array_list, &value); // -1 if not found
```

To retrieve the index of the last occurrence of an element:

```c++
int last_index = array_list_last_index_of(array_list, &value); // -1 if not found
```

These functions rely on the `equals` function provided in `ArrayListOptions`. The library provides default
`equals` implementations for the following data types: `char`, `int`, `long`, `float`, `double`, `void*` and `string`. The one
defined in `DEFAULT_ARRAY_LIST_OPTIONS` compares the memory addresses of the elements. Use the `DEFAULT_EQUALS(T)` macro
to choose between default `equals` implementations. To customize:

```c++
bool my_equals(void* element_a, void* element_b) {
    return *(int*) element_a == *(int*) element_b;
}
```

To search for an element index, using the binary search algorithm:

```c++
int index = array_list_binary_search(array_list, &value, DEFAULT_COMPARATOR(int)); // -1 if not found
```

It requires the array list to be previously sorted.

That function relies on a `Comparator` instead of the `equals` function since the binary search algorithm
needs to compare whether the current element is equal to, greater than, or less than the target element.

---

### Cloning

To create a shallow copy of the array list:

```c++
// list: [ 1, 2, 3, 4 ]
ArrayList* copy = array_list_clone(array_list); // [ 1, 2, 3, 4 ]
```

### SubList

To create a view of the array list from a start index (inclusive) to an end index (exclusive):

```c++
// list: [ 1, 2, 3, 4, 5, 6 ]
ArrayList* sub_list = array_list_sub_list(array_list, 1, 5); // [ 2, 3, 4, 5 ]
```

### To Array

To convert an array list to an array:

```c++
// list: [ 1, 2, 3, 4 ]
void** array = array_list_to_array(array_list); // [ 1, 2, 3, 4 ]
```

Or, more precisely:

```c++
int* values = *array_list_to_array(array_list); // [ 1, 2, 3, 4 ]
```

### To Collection

To convert an array list into a simple abstract type which can represent any data structure:

```c++
Collection collection = array_list_to_collection(array_list);
```

That abstraction is used to achieve polymorphism-like behavior.

### String Representation

To obtain a string representation of the array list:

```c++
// list: [ 1, 2, 3, 4 ]
char* string = array_list_to_string(array_list); // [ 0x7fffb6f53fe0, 0x7fffb6f53fe4, 0x7fffb6f53fe8, 0x7fffb6f53fec ]
```

The output format depends on the `to_string` function defined in `ArrayListOptions`. The library provides default
`to_string` implementations for the following data types: `char`, `int`, `long`, `float`, `double`, `void*` and `string`. The one
defined in `DEFAULT_ARRAY_LIST_OPTIONS` prints the memory addresses of the elements. To customize:

```c++
char* my_to_string(void* element) {
    int length = 10; // length of the string representation of the element
    char* string = malloc(length * sizeof(char)); // This is mandatory, that pointer will be freed internally later
    snprintf(string, length, "%p", element);
    return string;
};
```