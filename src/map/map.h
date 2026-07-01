#ifndef LIBCDSA_MAP_H
#define LIBCDSA_MAP_H

#include "hash_map.h"
#include "tree_map.h"

/**
 * @file map.h
 * @brief A thin abstraction layer that provides compile-time dispatch for map types
 *        (HashMap and TreeMap).
 *
 * Each macro detects the underlying map type and dispatches to the appropriate
 * function, allowing generic code to operate on different map structures
 * without runtime overhead.
 *
 * Example:
 * @code
 * HashMap* hash_map = map_new(DEFAULT_HASH_MAP_OPTIONS()); // Calls hash_map_new()
 * TreeMap* tree_map = map_new(DEFAULT_TREE_MAP_OPTIONS()); // Calls tree_map_new()
 *
 * map_put(hash_map, "key_a", &(int){10}); // Calls hash_map_put()
 * map_put(tree_map, "key_b", &(int){20); // Calls tree_map_put()
 * @endcode
 *
 * @note there are no function macros for exclusive functions (e.g. tree_map_higher())
 */

#define MAP_DISPATCH(function, map, ...) \
    _Generic((map), HashMap*: hash_map_##function, TreeMap*: tree_map_##function)(map __VA_OPT__(,) __VA_ARGS__)

#define map_new(options) \
    _Generic((options), HashMapOptions*: hash_map_new, TreeMapOptions*: tree_map_new)(options)

#define map_from(entry_collection, options) \
    _Generic((options), HashMapOptions*: hash_map_from,  TreeMapOptions*: tree_map_from)(entry_collection, options)

#define map_destroy(map) \
    _Generic((map), HashMap**: hash_map_destroy, TreeMap**: tree_map_destroy)(map)

#define map_change_key_destructor(...) MAP_DISPATCH(change_key_destructor, __VA_ARGS__)

#define map_change_value_destructor(...) MAP_DISPATCH(change_value_destructor, __VA_ARGS__)

#define map_compute(...) MAP_DISPATCH(compute, __VA_ARGS__)

#define map_compute_if_absent(...) MAP_DISPATCH(compute_if_absent, __VA_ARGS__)

#define map_compute_if_present(...) MAP_DISPATCH(compute_if_present, __VA_ARGS__)

#define map_merge(...) MAP_DISPATCH(merge, __VA_ARGS__)

#define map_put(...) MAP_DISPATCH(put, __VA_ARGS__)

#define map_put_if_absent(...) MAP_DISPATCH(put_if_absent, __VA_ARGS__)

#define map_put_all(...) MAP_DISPATCH(put_all, __VA_ARGS__)

#define map_get(...) MAP_DISPATCH(get, __VA_ARGS__)

#define map_get_or_default(...) MAP_DISPATCH(get_or_default, __VA_ARGS__)

#define map_replace(...) MAP_DISPATCH(replace, __VA_ARGS__)

#define map_replace_if_equals(...) MAP_DISPATCH(replace_if_equals, __VA_ARGS__)

#define map_remove(...) MAP_DISPATCH(remove, __VA_ARGS__)

#define map_remove_if_equals(...) MAP_DISPATCH(remove_if_equals, __VA_ARGS__)

#define map_replace_all(...) MAP_DISPATCH(replace_all, __VA_ARGS__)

#define map_size(...) MAP_DISPATCH(size, __VA_ARGS__)

#define map_is_empty(...) MAP_DISPATCH(is_empty, __VA_ARGS__)

#define map_iterator(...) MAP_DISPATCH(iterator, __VA_ARGS__)

#define map_equals(...) MAP_DISPATCH(equals, __VA_ARGS__)

#define map_for_each(...) MAP_DISPATCH(for_each, __VA_ARGS__)

#define map_clear(...) MAP_DISPATCH(clear, __VA_ARGS__)

#define map_contains(...) MAP_DISPATCH(contains, __VA_ARGS__)

#define map_contains_key(...) MAP_DISPATCH(contains_key, __VA_ARGS__)

#define map_contains_value(...) MAP_DISPATCH(contains_value, __VA_ARGS__)

#define map_entries(...) MAP_DISPATCH(entries, __VA_ARGS__)

#define map_keys(...) MAP_DISPATCH(keys, __VA_ARGS__)

#define map_values(...) MAP_DISPATCH(values, __VA_ARGS__)

#define map_clone(...) MAP_DISPATCH(clone, __VA_ARGS__)

#define map_to_string(...) MAP_DISPATCH(to_string, __VA_ARGS__)

#endif