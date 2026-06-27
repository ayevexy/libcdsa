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

#define set_change_destructor(set, destructor) SET_DISPATCH(change_destructor, set, destructor)

#define set_add(set, element) SET_DISPATCH(add, set, element)

#define set_add_all(set, collection) SET_DISPATCH(add_all, set, collection)

#define set_remove(set, element) SET_DISPATCH(remove, set, element)

#define set_remove_all(set, collection) SET_DISPATCH(remove_all, set, collection)

#define set_remove_if(set, condition) SET_DISPATCH(remove_if, set, condition)

#define set_retain_all(set, collection) SET_DISPATCH(retain_all, set, collection)

#define set_size(set) SET_DISPATCH(size, set)

#define set_is_empty(set) SET_DISPATCH(is_empty, set)

#define set_iterator(set) SET_DISPATCH(iterator, set)

#define set_equals(set, other_set) SET_DISPATCH(equals, set, other_set)

#define set_for_each(set, action) SET_DISPATCH(for_each, set, action)

#define set_clear(set) SET_DISPATCH(clear, set)

#define set_contains(set, element) SET_DISPATCH(contains, set, element)

#define set_contains_all(set, collection) SET_DISPATCH(contains_all, set, collection)

#define set_clone(set) SET_DISPATCH(clone, set)

#define set_to_collection(set) SET_DISPATCH(to_collection, set)

#define set_to_array(set) SET_DISPATCH(to_array, set)

#define set_to_string(set) SET_DISPATCH(to_string, set)

#endif