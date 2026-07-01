#ifndef LIBCDSA_SET_H
#define LIBCDSA_SET_H

#include "hash_set.h"
#include "tree_set.h"

/**
 * @file set.h
 * @brief A thin abstraction layer that provides compile-time dispatch for set types
 *        (HashSet and TreeSet).
 *
 * Each macro detects the underlying set type and dispatches to the appropriate
 * function, allowing generic code to operate on different set structures
 * without runtime overhead.
 *
 * Example:
 * @code
 * HashSet* hash_set = set_new(DEFAULT_HASH_SET_OPTIONS()); // Calls hash_set_new()
 * TreeSet* tree_set = set_new(DEFAULT_TREE_SET_OPTIONS()); // Calls tree_set_new()
 *
 * set_add(hash_set, &(int){10}); // Calls hash_set_add()
 * set_add(tree_set, &(int){20); // Calls tree_set_add()
 * @endcode
 *
 * @note there are no function macros for exclusive functions (e.g. hash_set_capacity())
 */

#define SET_DISPATCH(function, set, ...) \
    _Generic((set), HashSet*: hash_set_##function, TreeSet*: tree_set_##function)(set __VA_OPT__(,) __VA_ARGS__)

#define set_new(options) \
    _Generic((options), HashSetOptions*: hash_set_new, TreeSetOptions*: tree_set_new)(options)

#define set_from(collection, options) \
    _Generic((options), HashSetOptions*: hash_set_from, TreeSetOptions*: tree_set_from)(collection, options)

#define set_destroy(set) \
    _Generic((set), HashSet**: hash_set_destroy, TreeSet**: tree_set_destroy)(set)

#define set_change_destructor(...) SET_DISPATCH(change_destructor, __VA_ARGS__)

#define set_add(...) SET_DISPATCH(add, __VA_ARGS__)

#define set_add_all(...) SET_DISPATCH(add_all, __VA_ARGS__)

#define set_remove(...) SET_DISPATCH(remove, __VA_ARGS__)

#define set_remove_all(...) SET_DISPATCH(remove_all, __VA_ARGS__)

#define set_remove_if(...) SET_DISPATCH(remove_if, __VA_ARGS__)

#define set_retain_all(...) SET_DISPATCH(retain_all, __VA_ARGS__)

#define set_size(...) SET_DISPATCH(size, __VA_ARGS__)

#define set_is_empty(...) SET_DISPATCH(is_empty, __VA_ARGS__)

#define set_iterator(...) SET_DISPATCH(iterator, __VA_ARGS__)

#define set_equals(...) SET_DISPATCH(equals, __VA_ARGS__)

#define set_for_each(...) SET_DISPATCH(for_each, __VA_ARGS__)

#define set_clear(...) SET_DISPATCH(clear, __VA_ARGS__)

#define set_contains(...) SET_DISPATCH(contains, __VA_ARGS__)

#define set_contains_all(...) SET_DISPATCH(contains_all, __VA_ARGS__)

#define set_clone(...) SET_DISPATCH(clone, __VA_ARGS__)

#define set_to_collection(...) SET_DISPATCH(to_collection, __VA_ARGS__)

#define set_to_array(...) SET_DISPATCH(to_array, __VA_ARGS__)

#define set_to_string(...) SET_DISPATCH(to_string, __VA_ARGS__)

#endif