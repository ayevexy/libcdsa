# RELEASES

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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