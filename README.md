<p align="center"><img src="logo.svg" alt="Project Logo"></p>

> [!WARNING]
> This library is currently a work in progress. It is experimental and may be unstable — use with caution.

## Table of Contents

- [Overview](#overview)
  - [Features](#features)
  - [Strengths](#strengths)
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

**A Feature-Rich, High-Level, and Easy-to-Use General-Purpose Utility Library for the C Programming Language.**

This project provides a comprehensive collection of utilities, abstractions, data structures,
and algorithms designed to make C development more expressive, convenient, and productive.
It offers a broad range of functionality, including collections, memory management,
string manipulation, mathematical operations, date and time handling, file
and system utilities, logging, and more. Its API emphasizes consistency, reusability,
and ease of use, drawing inspiration from modern programming languages and libraries,
particularly the Java ecosystem.

### Features

#### Collections

The library provides a variety of commonly used data structures:

- [Array List](src/collections/list/array_list.h): A dynamic, linear structure that stores elements contiguously in memory for fast random access.
- [Linked List](src/collections/list/linked_list.h): A dynamic, linear structure that stores elements in non-contiguous memory, linked by references.
- [Hash Map](src/collections/map/hash_map.h): An unordered associative structure mapping unique keys to values using a hash function.
- [Tree Map](src/collections/map/tree_map.h): An ordered associative structure mapping unique keys to values, backed by a binary search tree.
- [Hash Set](src/collections/set/hash_set.h): An unordered structure storing unique elements with hash-based lookup.
- [Tree Set](src/collections/set/tree_set.h): An ordered structure storing unique elements in a sorted manner, backed by a binary search tree.
- [Deque](src/collections/deque.h): A linear structure allowing insertion and removal at both ends of the sequence.
- [Stack](src/collections/stack.h): A linear LIFO structure where elements are inserted and removed from the top.
- [Queue](src/collections/queue.h): A linear FIFO structure where elements are added at the rear and removed from the front.
- [Priority Queue](src/collections/priority_queue.h): A dynamic structure that stores elements according to their priority.

The library also provides thin abstraction layers for list, map, and set types through compile-time dispatch:

- [List](src/collections/list/list.h): An abstraction over `ArrayList` and `LinkedList`.
- [Map](src/collections/map/map.h): An abstraction over `HashMap` and `TreeMap`.
- [Set](src/collections/set/set.h): An abstraction over `HashSet` and `TreeSet`.

Additional collection-related functionality:

- [Collection](src/collections/collection.h): Generic, non-owning collection view abstraction.
- [Iterator](src/collections/iterator.h): Generic iterator abstraction.
- [Sequence](src/collections/sequence.h): Declarative pipeline processing of collections.
- [For Each](src/collections/for_each.h): Macro abstraction for iterating through collections.
- [Algorithms](src/collections/algorithms.h): Enumerations defining available sorting and shuffling algorithms.
- [Callbacks](src/collections/callbacks.h): Default function implementations for library containers.

#### Core

Fundamental abstractions and functionality used throughout the library:

- [Types](src/core/types.h): Common type aliases.
- [Limits](src/core/limits.h): Typed numeric limit aliases.
- [Macros](src/core/macros.h): General-purpose utility macros.
- [Errors](src/core/errors.h): Built-in error handling system.
- [Assertions](src/core/assertions.h): Custom assertion utilities.
- [Constraints](src/core/constraints.h): Precondition checks.
- [Memory](src/core/memory.h): Memory management abstractions.
- [Math](src/core/math.h): Mathematical functions, constants, and utilities.
- [Array](src/core/array.h): Built-in custom array implementation.
- [String](src/core/string.h): Built-in custom string implementation.
- [Functions](src/core/functions.h): Generic function pointer typedefs.
- [Optional](src/core/optional.h): A container type which may or may not contain a value.
- [System](src/core/system.h): Runtime system abstraction.

#### Util

Additional functionality for common programming tasks:

- [File](src/util/file.h): File handling abstractions.
- [Sync](src/util/sync.h): Thread synchronization abstractions.
- [Allocators](src/util/allocators.h): Custom memory allocators.
- [Logging](src/util/logging.h): Simple logging utility.
- [Temporal](src/util/temporal.h): Date/Time abstractions.

### Strengths

Some of the library's strengths:

- Rich feature set covering a wide range of data structures, algorithms, and utilities.
- High-level abstractions designed to make C development easier.
- Consistent and intuitive API design.
- Modular architecture with independent headers.
- No external dependencies.
- Comprehensive documentation and test coverage.
- Strictly C23 conformance.

### Limitations

Like any other project, there are some limitations (most by design):

- Requires a compiler with C23 support.
- Not optimized for peak performance.
- May cause namespace collisions with existing or third-party code.
- Most functionalities are not designed to be thread-safe.
- Some functionalities are only available on Linux.
- It might have some bugs... (help find them!)

### Technologies

This project is being built using the following technologies:

- The C Programming Language
- CMake Build System
- Unity Test Framework

## Getting Started

There are several ways of adding the library to your project:

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

The library is also distributed as a header/source pair files in [Releases](https://github.com/ayevexy/libcdsa/releases/latest). Just grab them:

```
your-project/
  libcdsa.h
  libcdsa.c
  main.c
```

### Usage

Simply include the headers you need (or the `libcdsa.h` master header) and start coding.
Don't forget to prefix includes with `libcdsa/` if installed system-wide.

The following examples demonstrate some of the library's features:

#### Collections

**Lists**

```c++
#include "collections/list/list.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    ArrayList* list = list_new(ARRAY_LIST_OPTIONS(cstring));

    list_add_last(list, "Hello");
    list_add_last(list, "World!");
    
    const char* hello = list_get(list, 0);
    const char* world = list_get(list, 1);
    
    printf("%s %s\n", hello, world); // Hello World!

    list_destroy(&list);
    return 0;
}
```

**Maps**

```c++
#include "collections/map/map.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    HashMap* map = map_new(HASH_MAP_OPTIONS(cstring, int));

    map_put(map, "Hello", &(int){5});
    map_put(map, "World!", &(int){6});

    int* hello_length = map_get(map, "Hello");
    int* world_length = map_get(map, "World!");

    printf("Full length: %d\n", *hello_length + *world_length); // 11

    map_destroy(&map);
    return 0;
}
```

**Sets**

```c++
#include "collections/set/set.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    HashSet* set = set_new(HASH_SET_OPTIONS(int));
    
    set_add(set, &(int){10});
    set_add(set, &(int){20});

    bool present = set_contains(set, &(int){30});
    printf("%s\n", present ? "true" : "false"); // false

    set_destroy(&set);
    return 0;
}
```

**Iterators**

```c++
#include "collections/list/list.h"
#include "collections/iterator.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    ArrayList* list = list_new(ARRAY_LIST_OPTIONS(cstring));

    list_add_last(list, "Hello");
    list_add_last(list, "World!");

    Iterator* iter = list_iterator(list);

    while (iterator_has_next(iter)) {
        const char* string = iterator_next(iter);
        printf("%s ", string); // Hello World!
    }
    printf("\n");

    iterator_destroy(&iter);

    list_destroy(&list);
    return 0;
}
```

**For Each**

```c++
#include "collections/list/list.h"
#include "collections/for_each.h" // should be included last
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    ArrayList* list = list_new(ARRAY_LIST_OPTIONS(cstring));

    list_add_last(list, "Hello");
    list_add_last(list, "World!");

    for_each (const char* string, list) {
        printf("%s ", string); // Hello World!
    }
    printf("\n");

    list_destroy(&list);
    return 0;
}
```

**Sequences**

```c++
#include "collections/list/list.h"
#include "collections/collection.h"
#include "collections/callbacks.h"
#include "collections/sequence.h"
#include "core/memory.h"
#include "core/array.h"
#include <stdio.h>

static bool is_even(const void* number) {
    return *(int*) number % 2 == 0;
}

static void add_one(void* number) {
    *(int*) number += 1;
}

static void* replace_by_two_times(void* number) {
    return new(int, *(int*) number * 2);
}

int main(void) {
    ArrayList* list = list_new(ARRAY_LIST_OPTIONS(int));

    list_add_last(list, &(int){1});
    list_add_last(list, &(int){2});
    list_add_last(list, &(int){3});
    list_add_last(list, &(int){4});
    list_add_last(list, &(int){5});

    Sequence* sequence = sequence_from(list_to_collection(list));

    sequence_filter(sequence, is_even); // 2, 4
    sequence_peek(sequence, add_one); // 3, 5
    sequence_map(sequence, replace_by_two_times); // 6, 10

    Array(void*) array = sequence_to_array(sequence);

    for (int i = 0; i < array_length(array); i++) {
        printf("%d ", *(int*) array[i]); // 6, 10
    }
    printf("\n");

    list_destroy(&list);
    array_destroy(&array);
    return 0;
}
```

**Set Operations**

```c++
#include "collections/set/set.h"
#include "collections/collection.h"
#include "collections/for_each.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    HashSet* set_a = set_new(HASH_SET_OPTIONS(int));
    set_add(set_a, &(int){1});
    set_add(set_a, &(int){2});
    set_add(set_a, &(int){3});

    TreeSet* set_b = set_new(TREE_SET_OPTIONS(int));
    set_add(set_b, &(int){3});
    set_add(set_b, &(int){4});
    set_add(set_b, &(int){5});

    TreeSet* union_set = sets_union(set_a, set_b, set_new(TREE_SET_OPTIONS(int))); // 1, 2, 3, 4, 5
    HashSet* intersection_set = sets_intersection(set_a, set_b, set_new(HASH_SET_OPTIONS(int))); // 3
    TreeSet* difference_set = sets_difference(set_a, set_b, set_new(TREE_SET_OPTIONS(int))); // 1, 2

    for_each (int* e, union_set) printf("%d ", *e); // 1, 2, 3, 4, 5
    printf("\n");

    for_each (int* e, intersection_set) printf("%d ", *e); // 3
    printf("\n");
    
    for_each (int* e, difference_set) printf("%d ", *e); // 1, 2
    printf("\n");

    set_destroy(&set_a);
    set_destroy(&set_b);
    return 0;
}
```

**Collection Views and Conversion**

```c++
#include "collections/list/list.h"
#include "collections/collection.h"
#include "collections/callbacks.h"
#include <stdio.h>

int main(void) {
    ArrayList* list = list_new(ARRAY_LIST_OPTIONS(cstring));

    list_add_last(list, "Hello");
    list_add_last(list, "World!");

    Collection collection = list_to_collection(list);

    LinkedList* other_list = list_new(LINKED_LIST_OPTIONS(cstring));
    list_add_all_last(other_list, collection);

    const char* hello = list_get(other_list, 0);
    const char* world = list_get(other_list, 1);

    printf("%s %s\n", hello, world); // Hello World!

    list_destroy(&list);
    list_destroy(&other_list);
    return 0;
}
```

#### Core

**Error Handling**

```c++
#include "core/errors.h"
#include <stdio.h>

// custom application error
static const Error INVALID_OPERATION_ERROR = ERROR("INVALID_OPERATION_ERROR");

int divide(int a, int b) {
    if (b == 0) {
        set_error(ARITHMETIC_ERROR, "Cannot divide by zero: %d / %d", a, b);
        return -1;
    }
    return a / b;
}

int main(void) {
    int value;
    Error error = attempt(value = divide(10, 0));

    if (error) {
        error_print(); // ARITHMETIC_ERROR: Cannot divide by zero: 10 / 0
    }                  //       at divide()

    printf("value: %d\n", value); // value: -1

    Result(int) result = try(divide(5, 0));

    if (result.error == ARITHMETIC_ERROR) {
        error_print(); // ARITHMETIC_ERROR: Cannot divide by zero: 5 / 0
    }                  //       at divide()
    
    printf("result value: %d\n", result.value); // result value: -1
    
    set_error(INVALID_OPERATION_ERROR); // INVALID_OPERATION_ERROR: no additional details available
                                        //      at main()
    printf("unreachable");

    return 0;
}
```

**Assertions**

```c++
#include "core/assertions.h"

static int divide(int a, int b) {
    assert_precondition(b != 0, "divisor must not be zero, received %d", b);

    const int result = a / b;

    assert_postcondition(result * b == a, "expected %d, received %d", a, result * b);

    return result;
}

int main(void) {
    int size = 5;
    int capacity = 10;

    assert_condition(size > 0, "expected positive size, received %d", size);
    assert_invariant(size <= capacity, "size (%d) exceeds capacity (%d)", size, capacity);

    divide(10, 2);

    if (false) {
        assert_unreachable();
    }

    return 0;
}
```

**Constraints**

```c++
#include "core/constraints.h"
#include <stdio.h>

// custom constraint
#define require_different_accounts(source, destination) \
    require((source) != (destination), ILLEGAL_ARGUMENT_ERROR, "source and destination accounts must be different")

static bool transfer(int source_account_id, int destination_account_id, double amount) {
    if (require_positive(amount)) return false;
    if (require_different_accounts(source_account_id, destination_account_id)) return false;

    printf("Transferred $%.2f from account %d to account %d\n",
           amount, source_account_id, destination_account_id);

    return true;
}

int main(void) {
    transfer(1001, 2002, 50.00); // Transferred $50.00 from account 1001 to account 2002
    transfer(1001, 1001, 25.00); // ILLEGAL_ARGUMENT_ERROR: source and destination accounts must be different
    transfer(1001, 2002, -10.00); // ILLEGAL_ARGUMENT_ERROR: 'amount' must be greater than zero

    return 0;
}
```

**Memory Management**

```c++
#include "core/memory.h"
#include <stdio.h>

typedef struct {
    float x, y;
} Point;

int main(void) {
    int* number = new(int, 10); // heap allocated
    int* local = ref(int, 20); // stack allocated
    Point* point = new(Point, .x = 10, .y = 20);

    printf("%d\n", *number); // 10
    printf("%d\n", *local); // 20
    printf("x: %f, y: %f\n", point->x, point->y); // x: 10, y: 20

    delete(number);
    delete(point);
}
```

**Arrays**

```c++
#include "core/array.h"
#include <stdio.h>

int main(void) {
    Array(int) array_a = array_new(5, int); // [ 0, 0, 0, 0, 0 ]
    Array(int) array_b = array_of(int, 1, 2, 3, 4, 5); // [ 1, 2, 3, 4, 5 ]

    int n = array_get(array_a, 0); // `int n = array_a[0];` also works
    printf("%d\n", n); // 0

    array_set(array_a, 0, 10); // `array_a[0] = 10;` also works

    for (int i = 0; i < array_length(array_a); i++) {
        printf("%d ", array_a[i]); // 10, 0, 0, 0, 0
    }    
    printf("\n");

    for (int i = 0; i < array_length(array_b); i++) {
        printf("%d ", array_b[i]); // 1, 2, 3, 4, 5
    }
    printf("\n");

    array_destroy(&array_a);
    array_destroy(&array_b);
    
    return 0;
}
```

**Strings**

```c++
#include "core/string.h"
#include "core/array.h"
#include <stdio.h>

int main(void) {
    String hello = string_new("Hello"); // copies "Hello"
    String world = string_ref("World!"); // references "World!"

    char c = string_char_at(hello, 2);
    printf("%c\n", c); // l

    String sub = string_substring("literal", 0, 3);
    printf("%s\n", string_data(sub)); // lit

    // functions accept both literals and string types
    String hello_world = string_join(" ", hello, world, "Code!");
    printf("%s\n", string_data(hello_world)); // Hello World! Code!

    Array(String) words = string_split(hello_world, ' ');
    for (int i = 0; i < array_length(words); i++) {
        printf("%s ", string_data(words[i])); // Hello World! Code!
    }
    printf("\n");

    string_destroy_all(&hello, &world, &sub, &hello_world);
    array_destroy(&words);
    
    return 0;
}
```

**System**

```c++
#include "core/system.h"
#include "core/string.h"
#include <inttypes.h>

static void* hello_thread(void*) {
    system_write_line("I'm a thread!");
    return nullptr;
}

int main(void) {
    // input/output operations
    system_write_line("What's up?");

    String response = system_read_line();
    system_write_line(response);

    string_destroy(&response);

    // process management
    const intptr process_id = system_process_create("echo", "I'm a child process!");

    system_process_wait(process_id);

    // multithreading
    const intptr thread_id = system_thread_create(hello_thread);

    system_thread_join(thread_id);

    // environment variables
    system_set_environment_variable("LIBCDSA_TEST_ENV", "0");

    const char* env = system_get_environment_variable("LIBCDSA_TEST_ENV");
    system_write_line("LIBCDSA_TEST_ENV: %s", env);

    system_remove_environment_variable("LIBCDSA_TEST_ENV");

    // platform information
    system_write_line(system_platform_name());
    system_write_line(system_platform_version());
    system_write_line(system_platform_architecture());

    // time measurement
    uint64 milliseconds = system_current_time(); // milliseconds elapsed since the Unix epoch.
    uint64 nanoseconds = system_elapsed_time(); // nanoseconds suitable for measuring elapsed time.

    system_write_line("%" PRIu64 "", milliseconds);
    system_write_line("%" PRIu64 "", nanoseconds);

    return 0;
}
```

#### Util

**File Operations**

```c++
#include "util/file.h"
#include "core/string.h"
#include "core/array.h"
#include <stdio.h>

int main(void) {
    // file operations
    file_create("test_file.txt");

    File* file = file_open("test_file.txt", FILE_READ | FILE_WRITE);

    file_write_string(file, "Hello, World!\n");
    file_rewind(file);

    String line = file_read_line(file);
    printf("%s", string_data(line));

    file_close(file);
    file_delete("test_file.txt");
    
    string_destroy(&line);

    // directory operations
    file_create_directory("test_dir");

    file_create("test_dir/file_1.txt");
    file_create("test_dir/file_2.txt");

    Array(String) files = file_list_directory("test_dir");

    for (size_t i = 0; i < array_length(files); i++) {
        printf("%s\n", string_data(files[i]));
    }

    array_destroy(&files);

    // path operations
    bool exists = file_exists("test_dir/file_2.txt");
    printf("file_2.txt exists: %s\n", exists ? "true" : "false");

    FileInfo info = file_info("test_dir/file_2.txt");

    printf("Size: %zu bytes\n", info.size);
    printf("Type: %s\n",
           info.is_regular ? "regular file" :
           info.is_directory ? "directory" :
           info.is_symbolic_link ? "symbolic link" :
           "other"
    );

    file_move("test_dir/file_2.txt", "test_dir/file_2_new.txt");
    file_delete("test_dir/file_2_new.txt");

    file_delete("test_dir/file_1.txt");
    file_delete("test_dir");

    return 0;
}
```

**Memory Allocators**

```c++
#include "util/allocators.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    // an arena allocates memory from a fixed-size region
    MemoryArena* arena = memory_arena_new(1024);

    char* message = memory_arena_alloc(arena, 32);
    int* numbers = memory_arena_alloc(arena, 4 * sizeof(int));

    strcpy(message, "Hello from the arena!");

    for (int i = 0; i < 4; i++) {
        numbers[i] = i + 1;
    }

    printf("Arena: %s\n", message); // Arena: Hello from the arena!

    printf("Arena numbers: %d %d %d %d\n", numbers[0], numbers[1], numbers[2], numbers[3]);
    // Arena numbers: 1 2 3 4

    // reset reuses the arena without destroying it
    memory_arena_reset(arena);

    // a pool provides a fixed number of equally-sized blocks
    MemoryPool* pool = memory_pool_new(4, 32); // 4 blocks of 32 bytes

    char* block_1 = memory_pool_alloc(pool);
    char* block_2 = memory_pool_alloc(pool);

    strcpy(block_1, "First pool block");
    strcpy(block_2, "Second pool block");

    printf("Pool: %s\n", block_1); // Pool: First pool block
    printf("Pool: %s\n", block_2); // Pool: Second pool block

    // individual blocks can be returned to the pool
    memory_pool_dealloc(pool, block_1);
    memory_pool_dealloc(pool, block_2);

    memory_arena_destroy(&arena);
    memory_pool_destroy(&pool);

    return 0;
}
```

**Logging**

```c++
#include "util/logging.h"

#undef LOG_MODULE
#define LOG_MODULE "main"

int main(void) {
    logger_initialize((LoggerOutput) { .console = true });

    logger_trace("Entering function...");
    logger_debug("Variable value: %d", 42);
    logger_info("Nothing happened...");
    logger_warn("Something unexpected happened...");
    logger_error("An operation failed...");
    logger_fatal("Unrecoverable error occurred...");

    // Output:
    // [2026-10-22 19:26:59] [TRACE] [main] [main.c:9 main()] : Entering function...
    // [2026-10-22 19:26:59] [DEBUG] [main] [main.c:10 main()] : Variable value: 42
    // [2026-10-22 19:26:59] [INFO] [main] [main.c:11 main()] : Nothing happened...
    // [2026-10-22 19:26:59] [WARN] [main] [main.c:12 main()] : Something unexpected happened...
    // [2026-10-22 19:26:59] [ERROR] [main] [main.c:13 main()] : An operation failed...
    // [2026-10-22 19:26:59] [FATAL] [main] [main.c:14 main()] : Unrecoverable error occurred...

    logger_finalize();
    return 0;
}
```

**Date and Time**

```c++
#include "util/temporal.h"
#include "core/string.h"
#include <stdio.h>

int main(void) {
    // current date and time
    const DateTime now = date_time_now();

    // create a specific date and time
    const DateTime launch = date_time_of(2025, 12, 17, 14, 30, 0);

    // format date and time
    printf("Current: %s\n", string_data(date_time_to_string(now)));
    printf("Launch:  %s\n", string_data(date_time_to_string(launch)));

    // extract date and time components
    const Date date = now.date;
    const Time time = now.time;

    printf("DateTime: %04d-%02d-%02d %02d:%02d:%02d\n",
        now.year, now.month, now.day, now.hour, now.minute, now.second
    );
    printf("Date: %04d-%02d-%02d\n", date.year, date.month, date.day);
    printf("Time: %02d:%02d:%02d\n", time.hour, time.minute, time.second);

    // date calculations
    const DateTime tomorrow = date_time_plus(now, 1, DAY);
    const DateTime next_week = date_time_plus(now, 7, DAY);

    printf("Tomorrow:  %s\n", string_data(date_time_to_string(tomorrow)));
    printf("Next week: %s\n", string_data(date_time_to_string(next_week)));

    // date inspection
    printf("Leap year: %s\n", date_is_leap_year(date) ? "yes" : "no");
    printf("Day of year: %d\n", date_year_day(date));
    printf("Weekday: %d\n", date_week_day(date));

    // comparison
    if (date_time_is_after(now, launch)) {
        printf("The launch is in the future.\n");
    } else if (date_time_is_before(now, launch)) {
        printf("The launch has passed.\n");
    }

    return 0;
}
```

### Documentation

Each module and utility is documented in its corresponding header file within the `src` folder.
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

Instead, to install to a custom directory in the home folder:

```bash
  cmake --install build --prefix $HOME/my-libraries
```

## Frequently Asked Questions

- **Why does libcdsa exist?**

  I originally created **libcdsa** as a hobby project to learn
  and practice the C programming language, particularly data
  structures and algorithms.

  As development progressed, the project gradually expanded
  beyond its original scope, evolving into a general-purpose
  library with a broad collection of functionality and its
  own design philosophy.


- **What makes libcdsa different?**

  Many C libraries prioritize simplicity and performance,
  often favoring limited functionality and non-intuitive api.

  **libcdsa**, on the other hand, prioritizes functionality,
  abstraction, and ease of use. Its goal is to provide rich,
  expressive APIs with consistent interfaces across different
  modules.

  For example, the [ArrayList](src/collections/list/array_list.h)
  module provides over 50 functions! You can explore the
  documentation to discover everything the library has to offer.


- **Isn't it a reimplementation of the Java Standard Library in C?**

  Although **libcdsa** is heavily inspired by the Java
  Standard Library, it is not a direct reimplementation.

  Java's API design is something I personally appreciate,
  so it serves as an important source of inspiration.
  However, there are considerable differences in both API
  design and implementation.

  For example, Java relies heavily on exceptions for error
  handling, whereas **libcdsa** provides its own built-in
  error handling mechanism. The differences become even more
  apparent when exploring the source code and API.


- **Is it easy to use?**

  Absolutely. If you've used Java before, many of the APIs
  will feel familiar. Even if you haven't, the library aims
  to maintain straightforward and consistent interfaces
  across its different modules.

  Installation requires CMake and a few additional steps,
  although individual header/source pairs can also be used
  directly.


- **Is it ready for production?**

  If the [limitations](#limitations) aren't an obstacle for
  your project, it may be suitable for production use.
  However, it's recommended to wait until the first stable
  release.

  The project is currently in beta, and breaking changes
  may occur before the definitive version `v1.0.0`.


- **Is it suitable for learning?**

  One of my goals throughout development has been to maintain
  clarity and readability in the code.

  However, learning from the project can be challenging due
  to its size, the variety of topics it covers, and some of
  its design decisions.

  Nevertheless, I believe it can be a valuable resource for
  exploring C programming, data structures, algorithms,
  memory management, API design, and other related topics.


- **Can I contribute to the project?**

  Absolutely! If you have feedback, feature requests, bug
  reports, or questions, you can start a discussion, open
  an issue, or email me at `ayevexy@proton.me`.

  If you've used **libcdsa** in any project, I'd love to
  hear about it!


- **Who are you?**

  Just a random boy who enjoys computers. 😉👍

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.