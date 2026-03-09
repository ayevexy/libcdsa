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

# Data Structures and Algorithms Library in C

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
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
- [Disclaimer](#disclaimer)

---

## Overview

A Feature-Rich Data Structures and Algorithms Library Implemented in C.

This library is meant to implement the majority of the algorithms and data structures using high-level abstractions
in pure C code. It aims to implement most structures like array lists, linked lists, hashtable, etc. as well as sorting algorithms like merge sort, quick sort. The library provides a high level API to operate on these structures,
instead of just the basic operations. The library comes with opinionated defaults but provides some customization, the intent is to 
act like a high-level language collections library while keeping some low-level control of the implementation. The API is heavily based
on the Java Collections Framework and shares many similarities with it.

This library is designed for ease of use and currently implements the following data structures:

- [Array List](src/list/array_list.h): A linear generic and dynamic data structure that stores data consecutively in memory.
- [Linked List](src/list/linked_list.h): A linear generic and dynamic data structure that stores data in non-consecutive memory locations linked by references.
- [Hash Map](src/map/hash_map.h): A generic dynamic associative data structure that maps keys to values using a hashing function.
- [Hash Set](src/set/hash_set.h): A generic dynamic unordered data structure that store elements using a hashing function to ensure uniqueness.

## Features

This project aims to accomplish the following features:

- Most Data Structures Implemented
- Ease of Use
- Rich API
- Customizable
- Good Defaults
- Great Test Coverage
- Simple Documentation

## Technologies

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

### Documentation

Each data structure is documented in its corresponding header file within the `src` folder.
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

Open an issue to request a feature or report a bug.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Disclaimer

This is a personal project I created for fun, learning, and practice.
It is not designed to be thread-safe, highly performant, or completely bug-free.
Do not use it in real-world projects unless these requirements are not important.