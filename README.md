<!-- Project Logo -->
<svg width="640" height="220" viewBox="0 0 640 220" xmlns="http://www.w3.org/2000/svg">
    <!-- Background -->
    <rect width="100%" height="100%" fill="#2B2B2B"/>
    <!-- Graph / network symbol forming a "C" -->
    <g stroke="#519ABA" stroke-width="3" fill="#2B2B2B">
        <circle cx="120" cy="80" r="7"/>
        <circle cx="90" cy="110" r="7"/>
        <circle cx="120" cy="140" r="7"/>
        <circle cx="160" cy="140" r="7"/>
        <circle cx="160" cy="110" r="7"/>
        <line x1="120" y1="80" x2="90" y2="110"/>
        <line x1="90" y1="110" x2="120" y2="140"/>
        <line x1="120" y1="140" x2="160" y2="140"/>
        <line x1="160" y1="140" x2="160" y2="110"/>
    </g>
    <!-- Highlight node -->
    <circle cx="120" cy="80" r="5.5" fill="#CC7832"/>
    <!-- Text -->
    <text x="260" y="125"
          font-family="JetBrains Mono, monospace"
          font-size="48"
          fill="#A9B7C6"
          letter-spacing="2">libcdsa</text>
</svg>

---

# Data Structures and Algorithms Library in C (Experimental)

---

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
- [Contributing](#contributing)
- [License](#license)

---

## Overview

A Feature-Rich High-Level Data Structures and Algorithms Library Implemented in C.

This library is meant to implement the most common algorithms and data structures using high-level abstractions in pure C code.
It aims to implement common data structures like array lists, linked lists, hashtable, etc. as well as sorting algorithms like merge sort, quick sort.
The library provides a high level feature-rich API to operate on these structures, instead of just basic operations.
The library comes with reasonable defaults but provides some configuration. 
The API is heavily based on the Java Collections Framework and shares many similarities with it.

The library implements the following data structures:

- [Array List](src/list/array_list.h): A linear generic and dynamic data structure that stores data consecutively in memory.
- [Linked List](src/list/linked_list.h): A linear generic and dynamic data structure that stores data in non-consecutive memory locations linked by references.
- [Hash Map](src/map/hash_map.h): A generic dynamic associative data structure that maps keys to values using a hashing function.
- [Tree Map](src/map/tree_map.h): ...
- [Hash Set](src/set/hash_set.h): A generic dynamic unordered data structure that stores elements using a hashing function to ensure uniqueness.
- [Tree Set](src/set/tree_set.h): ...
- [Deque](src/deque/deque.h): A generic dynamic linear data structure that allows insertion and removal of elements at both the front and back of the sequence.
- [Stack](src/stack/stack.h): A generic dynamic linear data structure that follows the Last-In, First-Out (LIFO) principle, allowing insertion and removal only at the top.
- [Queue](src/queue/queue.h): A generic dynamic linear data structure that follows the First-In, First-Out (FIFO) principle, allowing insertion at the rear and removal from the front.
- [Priority Queue](src/priority_queue/priority_queue.h): ...

Also, there is some other utilities which may be useful:

- [Sets](src/set/sets.h): Common mathematical set operations and set view abstraction.
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

---

### Features

This project aims to accomplish the following features:

- Most Common Data Structures Implemented
- High-Level Abstractions
- Ease of Use
- Feature Rich API
- Configurable
- Reasonable Defaults
- Great Test Coverage
- Simple Documentation
- ISO C23 Compliance

---

### Limitations

Like any other project, there are some limitations (most by design):

- Lack of Type Safety.
- Reference Storage Only.
- Not Optimized for Peak Performance.
- Can cause Namespace Collision.
- Not Thread-Safe.
- It Mighty have some Bugs... (help find them!)

---

### Technologies

This project is being built using the following technologies:

- The C Programming Language
- CMake Build System
- Unity Test Framework

---

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

Now just include the desired header files and start coding, for example: `array_list.h`

```c++
#include "list/array_list.h" // replace with <libcdsa/list/array_list.h> if installed system-wide
#include <stdio.h>

int main() {
    ArrayList* array_list = array_list_new(DEFAULT_ARRAY_LIST_OPTIONS());
    int values[] = { 1, 2, 3, 4, 5 };
    
    for (int i = 0; i < 5; i++) {
        array_list_add_last(array_list, &values[i]);
    }
    
    printf("%s\n", array_list_to_string(array_list)); // [ 1, 2, 3, 4, 5 ]
    return 0;
}
```

The library also provides the `libcdsa.h` header file to include all functionality at once.

### Documentation

Each data structure and utility is documented in its corresponding header file within the `src` folder.
For usage examples, see the corresponding test file in the `test` folder.

---

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

It will compile the library as a shared library by default.
To compile it as a static library instead, add the following flag:

```bash
  cmake -S . -B build -DBUILD_SHARED_LIBS=OFF
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

---

## Contributing

Open an issue to request a feature, report a bug, suggest some improvement or ask something.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.