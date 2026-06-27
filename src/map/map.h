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

#define map_change_key_destructor(map, destructor) MAP_DISPATCH(change_key_destructor, map, destructor)

#define map_change_value_destructor(map, destructor) MAP_DISPATCH(change_value_destructor, map, destructor)

#define map_compute(map, key, remapper) MAP_DISPATCH(compute, map, key, remapper)

#define map_compute_if_absent(map, key, mapper) MAP_DISPATCH(compute_if_absent, map, key, mapper)

#define map_compute_if_present(map, key, remapper) MAP_DISPATCH(compute_if_present, map, key, remapper)

#define map_merge(map, key, value, remapper) MAP_DISPATCH(merge, map, key, value, remapper)

#define map_put(map, key, value) MAP_DISPATCH(put, map, key, value)

#define map_put_if_absent(map, key, value) MAP_DISPATCH(put_if_absent, map, key, value)

#define map_put_all(map, entry_collection) MAP_DISPATCH(put_all, map, entry_collection)

#define map_get(map, key) MAP_DISPATCH(get, map, key)

#define map_get_or_default(map, key, default_value) MAP_DISPATCH(get_or_default, map, key, default_value)

#define map_replace(map, key, value) MAP_DISPATCH(replace, map, key, value)

#define map_replace_if_equals(map, key, old_value, value) MAP_DISPATCH(replace_if_equals, map, key, old_value, value)

#define map_remove(map, key) MAP_DISPATCH(remove, map, key)

#define map_remove_if_equals(map, key, value) MAP_DISPATCH(remove_if_equals, map, key, value)

#define map_replace_all(map, bi_operator) MAP_DISPATCH(replace_all, map, bi_operator)

#define map_size(map) MAP_DISPATCH(size, map)

#define map_is_empty(map) MAP_DISPATCH(is_empty, map)

#define map_iterator(map) MAP_DISPATCH(iterator, map)

#define map_equals(map, other_map) MAP_DISPATCH(equals, map, other_map)

#define map_for_each(map, action) MAP_DISPATCH(for_each, map, action)

#define map_clear(map) MAP_DISPATCH(clear, map)

#define map_contains(map, key, value) MAP_DISPATCH(contains, map, key, value)

#define map_contains_key(map, key) MAP_DISPATCH(contains_key, map, key)

#define map_contains_value(map, value) MAP_DISPATCH(contains_value, map, value)

#define map_entries(map) MAP_DISPATCH(entries, map)

#define map_keys(map) MAP_DISPATCH(keys, map)

#define map_values(map) MAP_DISPATCH(values, map)

#define map_clone(map) MAP_DISPATCH(clone, map)

#define map_to_string(map) MAP_DISPATCH(to_string, map)

#endif