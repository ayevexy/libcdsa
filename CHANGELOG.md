# RELEASES

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
  after all data structures includes in the client code unless the `libcda.h` header is being used instead.
- Removed both `hash_set.h` and `tree_map.h` includes from `sets.h`. Now to work, it should be placed last,
  after both `hash_set.h` or `tree_map.h` includes in the client code unless the `libcda.h` header is being used instead.
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