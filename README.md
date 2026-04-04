<p align="center"><img src="logo.svg" alt="Project Logo"></p>

> [!WARNING]
> This library is currently a work in progress. It is experimental and may be unstable — use it at your own risk.

## Table of Contents

- [Overview](#overview)
  - [Features](#features)
  - [Limitations](#limitations)
  - [Technologies](#technologies)
- [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installing](#installing)
    - [Usage](#usage)
    - [Documentation](#documentation)
    - [Building](#building)
    - [Tests](#tests)
    - [Installing System-Wide](#installing-system-wide)
- [Frequently Asked Questions](#frequently-asked-questions)
- [License](#license)

## Overview

**A Feature-Rich, High-Level, and Easy-to-Use Data Structures and Algorithms Library for the C programming language.**

This project provides common data structures and algorithms with clean, expressive, and
reusable abstractions — bringing a modern programming experience to C.
It includes implementations of widely used data structures such as array lists, linked lists, hash maps, and hash sets,
along with classic algorithms like merge sort and quick sort. The API is heavily inspired by the 
Java Collections Framework, offering a familiar interface for developers with a Java background.

### Features

The library currently provides the following data structures:

- [Array List](src/list/array_list.h): A dynamic, linear structure that stores elements contiguously in memory for fast random access.
- [Linked List](src/list/linked_list.h): A dynamic, linear structure that stores elements in non-contiguous memory, linked by references.
- [Hash Map](src/map/hash_map.h): An unordered associative structure mapping unique keys to values using a hash function.
- [Tree Map](src/map/tree_map.h): An ordered associative structure mapping unique keys to values, backed by a binary search tree.
- [Hash Set](src/set/hash_set.h): An unordered structure storing unique elements with hash-based lookup.
- [Tree Set](src/set/tree_set.h): An ordered structure storing unique elements in a sorted manner, backed by a binary search tree.
- [Deque](src/deque/deque.h): A linear structure allowing insertion and removal at both ends of the sequence.
- [Stack](src/stack/stack.h): A linear LIFO structure where elements are inserted and removed from the top.
- [Queue](src/queue/queue.h): A linear FIFO structure where elements are added at the rear and removed from the front.
- [Priority Queue](src/priority_queue/priority_queue.h): A dynamic, linear structure that stores elements according to their priority in a queue.

Also, there is some other utilities which may be useful:

- [Algorithms](src/util/algorithms.h): Algorithms related enumerations.
- [Collection](src/util/collection.h): Collection view abstraction.
- [Constraints](src/util/constraints.h): Pre-condition checks.
- [Errors](src/util/errors.h): Built-in custom error handling system used by the library.
- [For Each](src/util/for_each.h): For Each macro abstraction to iterate through collections.
- [Functions](src/util/functions.h): Functions typedefs and default implementations.
- [Iterator](src/util/iterator.h): Fully generic iterator abstraction.
- [Memory](src/util/memory.h): Memory management abstractions.
- [Optional](src/util/optional.h): Container type which may or may not contain a value.
- [Pair](src/util/pair.h): Container type which contains two values.
- [Sets](src/util/sets.h): Common mathematical set operations and set view abstraction.

### Limitations

Like any other project, there are some limitations (most by design):

- Lack of Type Safety.
- Reference Storage Only.
- Not Optimized for Peak Performance.
- Can cause Namespace Collision.
- Not Thread-Safe.
- It Mighty have some Bugs... (help find them!)

### Technologies

This project is being built using the following technologies:

- The C Programming Language
- CMake Build System
- Unity Test Framework

## Getting Started

There are several ways of adding the library to your project, following is one recommended way:

### Prerequisites

* C Compiler (C Standard 23)
* CMake Build Tool (Version 4.0)

### Installing

Add the library to your `CMakeLists.txt` and link it to your project:

```cmake
FetchContent_Declare(
        libcdsa
        GIT_REPOSITORY https://github.com/ayevexy/libcdsa.git
        GIT_TAG master # replace with the current version, e.g.: v1.0.0
)
FetchContent_MakeAvailable(libcdsa)

target_link_libraries(your_project PRIVATE libcdsa)
```

Instead, if installed system-wide:

```cmake
find_package(libcdsa REQUIRED)

target_link_libraries(your_project PRIVATE libcdsa::libcdsa)
```

### Usage

Now just include the `libcdsa.h` header file and start coding:

```c++
#include "libcdsa.h" // replace with <libcdsa/libcdsa.h> if installed system-wide
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // Create a new ArrayList with custom options. Except for `destruct`, these are the default values,
    // so this could be replaced with just `DEFAULT_ARRAY_LIST_OPTIONS(.destruct = free)`
    ArrayList* array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS(
        .initial_capacity = 10,
        .growth_factor = 2.0f,
        .destruct = free,          // By default elements aren't freed, so let's overwrite it
        .equals = pointer_equals,
        .to_string = pointer_to_string,
        .memory_alloc = malloc,
        .memory_realloc = realloc, // Optional for resizing, can use memory_alloc and memory_dealloc instead
        .memory_dealloc = free,
    ));

    // Add elements (heap-allocated via `new`, so it can be freed later)
    array_list_add_last(array_list, new(int, 1));
    array_list_add_last(array_list, new(int, 2));
    array_list_add_last(array_list, new(int, 3));

    // Error handling:
    int* value; Error error;
    // Without the `attempt` macro, the program would crash and print the error message
    // in the form of: INDEX_OUT_OF_BOUNDS_ERROR: index -1 out of bounds for length 3
    if (((error = attempt(value = array_list_get(array_list, -1))))) {

        printf("%s\n", error_to_string(error)); // INDEX_OUT_OF_BOUNDS_ERROR
        printf("%s\n", plain_error_message()); // index -1 out of bounds for length 3
    }

    // Iterator support:
    Iterator* iterator = array_list_iterator(array_list);
    while (iterator_has_next(iterator)) {
        const int* element = iterator_next(iterator);
        printf("element value: %d\n", *element); // 1, 2, 3
    }
    iterator_destroy(&iterator);

    // Or simplified using `for_each`:
    for_each (int* element, array_list) {
        printf("element value: %d\n", *element); // 1, 2, 3
    }

    // Any data structure can be converted to a collection view, achieving some polymorphism
    Collection collection = array_list_to_collection(array_list);
    
    // Converting the ArrayList to a string representation via `.to_string`
    // by default prints the memory addresses of the elements
    char* string = array_list_to_string(array_list);
    printf("%s", string); // [ 0x7ffd8c1a4e92, 0x7ffd3b7f9c10... ]
    
    // It must be freed later
    free(string);
    
    // Cleanup (elements are freed via `.destruct`)
    array_list_destroy(&array_list);
    return 0;
}
```
```c++
// including only the necessary headers instead:
// replace all with <libcdsa/MODULE_NAME> if installed system-wide
#include "map/hash_map.h"
#include "util/for_each.h" // `for_each.h` should be placed last, after all data structure includes
#include "util/errors.h"

#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // Create a new HashMap with default options, could be replaced with just `DEFAULT_HASH_MAP_OPTIONS()`
    HashMap* hash_map = hash_map_new(DEFAULT_HASH_MAP_OPTIONS(
        .initial_capacity = 16,
        .load_factor = 0.75f,
        .hash = pointer_hash,           // hash function
        .key_destruct = noop_destruct,
        .key_equals = pointer_equals,
        .key_to_string = pointer_to_string,
        .value_destruct = noop_destruct,
        .value_equals = pointer_equals,
        .value_to_string = pointer_to_string,
        .memory_alloc = malloc, 
        .memory_dealloc = free
    ));

    // Put entries (stack-allocated via compound expression, no need to free)
    hash_map_put(hash_map, "Rock", &(int){10});
    hash_map_put(hash_map, "Paper", &(int){20}); // Keys are `void*` too, using string literals just for convenience
    hash_map_put(hash_map, "Scissors", &(int){30}); 

    // Error handling, different approach:
    int* value; Error error = attempt(value = hash_map_get(nullptr, "Rock"));
    
    if (error == NULL_POINTER_ERROR) {
        printf("%s\n", error_to_string(error)); // NULL_POINTER_ERROR
        printf("%s\n", plain_error_message()); // 'hash_map' argument must not be null
    }

    // Iterator support:
    Iterator* iterator = hash_map_iterator(hash_map);
    while (iterator_has_next(iterator)) {
        const MapEntry* entry = iterator_next(iterator);
        printf("key: %s, value: %d\n", (char*) entry->key, *(int*) entry->value); // "Rock" 10, "Paper" 20...
    }
    iterator_destroy(&iterator);

    // Or simplified using `for_each`:
    for_each (MapEntry* entry, hash_map) {
        printf("key: %s, value: %d\n", (char*) entry->key, *(int*) entry->value); // "Rock" 10, "Paper" 20...
    }

    // The HashMap can also be converted to a collection view
    Collection entries = hash_map_entries(hash_map);
    
    // Converting the HashMap to a string representation via `.key_to_string` and `,value_to_string`
    // by default prints the memory addresses of the mapping key-value
    char* string = hash_map_to_string(hash_map);
    printf("%s", string); // [ 0x7ffd8c1a4e92 = 0x7ffd1c9b2f44, 0x7ffd3b7f9c10... ]
    
    // it must be freed later
    free(string);

    // Cleanup (no elements are freed since no destruct functions were provided)
    hash_map_destroy(&hash_map);
    return 0;
}
```

### Documentation

Each data structure and utility is documented in its corresponding header file within the `src` folder.
For usage examples, see the corresponding test file in the `test` folder.

### Building

If you want to build the project locally follow these steps instead:

Clone the project:

```bash
  git clone https://github.com/ayevexy/libcdsa.git
```

Go to the project directory:

```bash
  cd libcdsa
```

Build the project with CMake:

```bash
  mkdir build
  cmake -S . -B build
  cmake --build build
```

It will compile the library as a static library by default.
To compile it as a shared library instead, add the following flag:

```bash
  cmake -S . -B build -DBUILD_SHARED_LIBS=ON
```

### Tests

To run tests, execute the following command:

```bash
  ctest --test-dir build
```

### Installing System-Wide

To install the library system-wide in `/usr/local`, execute the following command:

```bash
  sudo cmake --install build
```

## Frequently Asked Questions

- **Isn't it just another data structures library? Why does this exist?**

I made **libcdsa** as a hobby project to learn and practice the C programming language and data structures
and algorithms. But the project grew while developing it and became a powerful library with a different
and interesting design approach from most implementations.

- **What makes libcdsa different?**

Most implementations focus on simplicity and performance, at the cost of limited scope and heavy use of macros. **libcdsa** is
the opposite—its focus is mainly on functionality, an abstracted API, and ease of use. For example: the [Array List](src/list/array_list.h)
module has 50+ functions! The library also provides some flexibility regarding memory management by making it available but optional.
You can check the documentation to learn more about what this project can offer.

- **Isn't it a reimplementation of the Java Collections Framework, but in C?**

Even though **libcdsa** is heavily inspired by Java Collections, it's not a reimplementation.
The API is based on Java because I personally like it, but there are many differences, both in the API and in the implementation.
For example, Java makes heavy use of exceptions; however, in **libcdsa**, exceptions don’t exist—there’s an entirely new built-in error handling mechanism.
If you look at the source code and the API, the differences will become clear.

- **Is it easy to use?**

Absolutely. If you’ve used Java before, the API will feel familiar.
Even if you haven’t, the design is straightforward and consistent across all data structures.
The only tough part would be installing it, which requires CMake and some steps—unless you write a simple installation script yourself.

- **Is it ready for production?**

If the mentioned [limitations](#limitations) aren’t an obstacle for your project, it might be used in production,
but it's better to wait until the first stable version is released. The project is currently in beta, and breaking changes may occur
before the definitive version `v1.0.0`.

- **Is it suitable for learning?**

One thing I tried to achieve and maintain while developing this project is the clarity and readability of the code.
But it can be difficult due to the size of the project, the topics it addresses, and some design decisions made.
Still, I think it could be a good source for learning.

- **Can I contribute with the project?**

Yes! If you have any feedback, feature requests, bug reports, or questions, you can start a discussion,
open an issue, or email me at `ayevexy@proton.me`. If you’ve used **libcdsa** anywhere, I’d love to know too!

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.