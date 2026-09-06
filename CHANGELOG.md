# RELEASES

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## 🎉 [v1.0.0-beta.7.1]() — 2026-09-06

A small update to add missing operations to the Sequence API and fix some bugs.

### ✨ Features
- Added `of()`, `empty()`, `generate()`, `iterate()` and `close()` operations to `util/sequence.h`.

### 🪲 Bug Fixes
- Prevent use-after-free in sequence processing (`util/sequence.h`).
- `string_ref()` macro no longer needs the inclusion of the `<string.h>` libc header.

---

## 🎉 [v1.0.0-beta.7]() — 2026-07-05

This update enhances the encapsulation of some modules, redesign the memory system and introduces the new Sequence API.

### ✨ Features
- Added `memory_try_alloc()` and `memory_try_realloc()` to `core/memory.h`.
- Added the memory allocator abstraction and the global memory allocator (`core/allocator.h`, `core/memory.h`).
- Added optioned type aliases for c types (`core/types.h`).
- Added the new Sequence API (`util/sequence.h`):
  - Creation: `sequence_from()`.
  - Operations: `filter()`, `map()`, `peek()`, etc.
  - Terminal Operations: `count()`, `to_array()`, `reduce()`, etc.

### ✏️ Changes
- Moved function definitions from `sets.h`, `collection.h`, `array.h`, and `memory.h` to their respective new source files.
- Replace all `malloc()`, `realloc()` and `free()`calls to `memory_try_alloc()`, `memory_try_realloc()` and `memory_dealloc()`.
- Simplified dispatcher abstraction macros (`list/list.h`, `map/map.h`, `set/set.h`).

### 📚 Documentation
- Correct `delete()` description (`core/memory.h`).
- Introduce the Sequence API in features and usage section of README.
- Add missing example of set operations in usage section of README.

---

## 🎉 [v1.0.0-beta.6]() — 2026-06-28

This update enhances the library project structure and introduces compile-time dispatch abstractions for collections.

### ✨ Features
- Added `malloc()`, `realloc()` and `free()` safe wrappers to `core/memory.h`.
- Added `type` field to `Collection` to allow safe casts to the underlying data structure.
- Added `array_get()` and `array_set()` operations to allow safe index access in `core/array.h`.
- Added compile-time dispatch support through function macros to the following data structures:
  - `ArrayList` and `LinkedList` in `list/list.h`.
  - `HashMap` and `TreeMap` in `map/map.h`.
  - `HashSet` and `TreeSet` in `set/set.h`.

### ✏️ Changes
- Replaced `set_memory_alloc()` and `set_memory_dealloc()` function pointers by `malloc()` and `free()`.
- Added function version of `delete()` to allow usage as function pointer.
- Rename `raw_new()` to `(new)()` to avoid usage.
- Moved pointer functions from `util/functions.h` to `util/pointer.h`.
- Renamed the `memory_dealloc` field in `Iterator` to `destroy`.
- Redesigned array initialization (`core/array.h`):
    - `array_new()` no longer accepts `...` as argument.
    - Moved initialization to the new `array_of()` function.
- Moved the following core utilities to the new `core/` module:
  - `errors.h`
  - `string.h`
  - `memory.h`
  - `array.h`
  - `version.h`
- Moved `map_entry.h` to the `util/` module.
- Changed `set` prefix to `sets` in `util/sets.h` to avoid confusion with `set/set.h`.

### 🪲 Bug Fixes
- Added missing `const` qualifier in `string_ref()` implementation.

### 📚 Documentation
- Add compile-time dispatch of collections to README feature section.
- Improve usage examples in README.

---

## 🎉 [v1.0.0-beta.5]() — 2026-06-07

This update polish the String API and improve its compatibility with the existing library containers.
Also adds the StringBuilder to the String API.

### ✨ Features
- Added the `StringBuilder` type to the String API in `util/string.h`. Currently, with the following operations:
  - `new()`, `append()`, `insert()`, `to_string()`, and `destroy()`.
- The `string_destroy()` operation now can take up to 10 arguments to reduce multiple calls.
- Added `string_data()` operation to retrieve the data of a string.
- Added callback wrappers for common `String` operations to be used in containers:
    - `string_hash_callback()`,
    - `string_equals_callback()`,
    - `string_compare_callback()`,
    - `string_destroy_callback()`,
    - `string_to_string_callback()`.

### ✏️ Changes
- The `String` type is now a pointer typedef instead of a raw struct; redesigned the underlying struct.
- Removed `StringView` type; now all functions that previously returned a string view now returns an allocated string.
- The redesigned `String` type now can reference both owning and non-owning data.
- Replace `string_view()` by `string_ref()` to create strings that reference existing data.
- Now `string_format()` accept a `String` instead of a `char*`.
- Removed all leading underscores from all identifiers to accomplish the C standard.
- The `String` type is now truly immutable through `const` modifier.
- All `to_string()` functions now returns a `String` instead of writing to an external buffer.
- Renamed `new()` function to `raw_new()` in `util/memory.h`.

### 🪲 Bug Fixes
- Correct `to_string()` separator calculation logic in `set/hash_set.h`.

---

## 🎉 [v1.0.0-beta.4.1]() — 2026-05-17

### ✨ Features
- Added `collection_to_array()` operation to `util/collection.h`.
- Added `string_length()` operation to `util/string.h`.

### ✏️ Changes
- `string_join()` macro now can take up to 20 arguments, previously it was 10.
- Renamed `sets_memory_alloc()` and `sets_memory_dealloc` dropping the extra `(s)`.
- `string_view()` macro no longer accepts arguments of type `char`.

---

## 🎉 [v1.0.0-beta.4]() — 2026-05-10

### ✨ Features
- Introduce an abstraction layer through the String API allowing interoperability with any type of string.
- Introduce the new Array API in `util/array.h`.
- Core operations: `array_new()`, `array_length()`, `array_destroy()`, read-write like normal c-arrays.

### ✏️ Changes
- The String API was redesigned again, now it has only two distinct types: `String` and `StringView`.
- Most string operations now can take any type of string (literals, arrays, `String` and `StringView`) as argument through a macro dispatch system.
- `string_join()` macro automatically inserts the null string.
- `string_split()` and `string_lines()` no longer appends a null string at the end of the array.
- Replace all `StringOwned` return type from `<collection>_to_string()` by the new `String` type.
- Use the new Array API in the operations that previously returned simple c arrays, i.e.: `<collection>_to_array()` and `string_split()`.
- Removed `string_null()` and `string_empty()` functions.
- Renamed `strings_memory_alloc()` and `strings_memory_dealloc()` to drop the extra `s`

### 📚 Documentation
- Update usage examples and the Feature section in README to reflect the new Array API and the String API changes.
- Added github issues templates, code of conduct, contributing guidelines and security policy.

---

## 🎉 [v1.0.0-beta.3.2]() — 2026-04-26

It’s a small update this time as I’m short on time. Hope you enjoy it!

### ✨ Features
- Added `equals()`, `contains_all()` and `for_each()` operations to `util/collection.h`.
- Added `try-result` construct to `util/errors.h`.

### 🪲 Bug Fixes
- Use the string allocator in `to_string()` operation for all data structures; previously, the data structure allocator was used instead.

### 📚 Documentation
- Added missing information and fixed typos in CHANGELOG.
- Aligned FAQ with pair header/source installation in README.
- Update usage examples in README to include the new `try-result` construct from `util/errors.h`.

---

## 🎉 [v1.0.0-beta.3.1]() — 2026-04-19

A very tiny update that distributes the library as a header/source pair files.

### 📚 Documentation
- Document header/source pair installation in README.

---

## 🎉 [v1.0.0-beta.3]() — 2026-04-19

A small update that refines the String API and brings other small changes.

### ✨ Features
- Added `index_of()`, `last_index_of()` and `replace()` operations to `util/string.h`.
- Added `value_of()` operation to `util/string.h`.

### ✏️ Changes
- Removed redundant `clone()` operation from `util/string.h`.
- Renamed `compare_pointers()` to `pointer_compare()` in `util/functions.h`.
- Removed `key_equals` construct and renamed `compare_keys` to `key_compare` in `tree_map.h`.
- Remove `equals` construct in favor of `compare` in `tree_set.h` and `priority_queue.h`.
- Moved `string_hash()` operation from `util/functions.h` to `util/string.h`.
- Prefix `index_of()`, `last_index_of()`, and `replace()` implementations with underscores and remove documentation in `util/string.h`.

### 🪲 Bug Fixes
- Add missing `util/string.h` include in `libcdsa.h` header file.
- Ensure null-terminator is placed in result string from `concat()` operation of `util/string.h`.
- Prevent heap corruption in `format()` of `util/string.h`.
- Prevent buffer overflow and correct off-by-one errors in `concat()`, `replace_char()`, `split()`, `to_uppercase()` and `to_lower_case()` operations of `util/string.h`.

---

## 🎉 [v1.0.0-beta.2]() — 2026-04-12

This update introduces the new String API alongside with some bug fixes. Enjoy!

### ✨ Features
- Introduce the new `String` API (`util/string.h`) with owned and view semantics.
- Added `StringOwned` as heap-allocated, mutable string type.
- Added `StringView` as lightweight non-owning string view.
- Core operations: concat, substring, replace, strip, join, and more.

### ✏️ Changes
- Replace `BiConsumer` with `BiOperator` in `reduce()` operation of `array_list.h` and `linked_list.h` to match the intended API.
- Improve performance of `find_last()`, `last_index_where()` and `last_index_of()` in `array_list.h` and `linked_list.h`
  by replacing iteration direction from forward to backward.
- Replace all `to_string()` operations return type from `char*` to `StringOwned` of the new String API.

### 🪲 Bug Fixes
- Aligned internal key-value pair layout with `MapEntry` to avoid undefined behavior in `hash_map.h` and `tree_map.h`.
- Correct `higher()`, `ceiling()`, `floor()` and `lower()` behavior for missing keys in `tree_map.h`.
- Correct `higher()`, `ceiling()`, `floor()` and `lower()` behavior for nonexistent elements in `tree_set.h`.

### 📚 Documentation
- Update usage examples to match the new String API in README, check it [here](README.md#usage).

---

## 🎉 [v1.0.0-beta.1]() — 2026-04-05

Another incremental change to the beta, this release brings a few new features and more modularity to the library.

### ✨ Features
- Added `reduce()` operation to `ArrayList` and `LinkedList`.
- Added `iterator_at()` operation to `Deque`, `ArrayList`, `LinkedList`, `TreeMap` and `TreeSet` to allow creation of iterators at arbitrary positions.
- Added `reverse()` operation to `Deque`.
- Added `util/version.h` header file with version information macros.

### ✏️ Changes
- Removed all data structures includes from `for_each.h`. Now to work, it should be placed last,
  after all data structures includes in the client code unless the `libcdsa.h` header is being used instead.
- Removed both `hash_set.h` and `tree_map.h` includes from `sets.h`. Now to work, it should be placed last,
  after both `hash_set.h` or `tree_map.h` includes in the client code unless the `libcdsa.h` header is being used instead.
- Moved `sets.h` from `set/` to `util/` module since it is a utility and not a core data structure.
- Prefixed all include guards with `LIBCDSA_` to prevent possible name collisions with client code.
- Simplified `tree_map.h` by removing redundant shortcut functions while renaming others, check it [here](https://github.com/ayevexy/libcdsa/commit/1743eb32).
- Renamed `contains_entry()` to `contains()` in `hash_map.h` and `tree_map.h`.
- Renamed all `set_destructor` functions to `change_destructor`.
- Added `const` qualifier to `key` member of `MapEntry`.

### 🪲 Bug Fixes
- Prevent overflow in threshold calculation in `hash_map.h` and `hash_set.h`.

### 📚 Documentation
- Added [Frequently Asked Questions](README.md#frequently-asked-questions) section to README.
- Improved array list usage example and added one for hash map in README, check it [here](README.md#usage).

### ⚙️ Build
- Generate static library by default instead of a shared libray in `CMakeLists.txt`.

---

## 🎉 [v1.0.0-beta]() — 2026-03-29

This is a beta release. The API is not yet stable and may change before v1.0.0.

### ✨ Features
- Core data structures implemented
- Core algorithms implemented
- Iteration abstractions
- Built-in error handling system
- Other useful utilities

### 🪲 Bug Fixes
- No significant bugs found at moment
- Help finding them!

### 🧪 Testing
- Considerable test coverage
- Tests as usage examples

### 📚 Documentation
- README with installation, usage and build instructions
- Inline documentation per module

### ⚙️ Notes
- This is a **beta version**, intended for testing and feedback
- Bugs and edge cases may appear
- Performance optimizations are still pending
- API is subject to change before stable release