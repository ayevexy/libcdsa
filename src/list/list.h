#ifndef LIBCDSA_LIST_H
#define LIBCDSA_LIST_H

#include "array_list.h"
#include "linked_list.h"

/**
 * @file list.h
 * @brief A thin abstraction layer that provides compile-time dispatch for list types
 *        (ArrayList and LinkedList).
 *
 * Each macro detects the underlying list type and dispatches to the appropriate
 * function, allowing generic code to operate on different list structures
 * without runtime overhead.
 *
 * Example:
 * @code
 * ArrayList* array_list = list_new(DEFAULT_ARRAY_LIST_OPTIONS()); // Calls array_list_new()
 * LinkedList* linked_list = list_new(DEFAULT_LINKED_LIST_OPTIONS()); // Calls linked_list_new()
 *
 * list_add_last(array_list, &(int){10}); // Calls array_list_add_last()
 * list_add_last(linked_list, &(int){20); // Calls linked_list_add_last()
 * @endcode
 *
 * @note there are no function macros for exclusive functions (e.g. array_list_trim_size())
 */

#define LIST_DISPATCH(function, list, ...) \
    _Generic((list), ArrayList*: array_list_##function, LinkedList*: linked_list_##function)(list __VA_OPT__(,) __VA_ARGS__)

#define list_new(options) \
    _Generic((options), ArrayListOptions*: array_list_new, LinkedListOptions*: linked_list_new)(options)

#define list_from(collection, options) \
    _Generic((options), ArrayListOptions*: array_list_from, LinkedListOptions*: linked_list_from)(collection, options)

#define list_destroy(list) \
    _Generic((list), ArrayList**: array_list_destroy, LinkedList**: linked_list_destroy)(list)

#define list_change_destructor(...) LIST_DISPATCH(change_destructor, __VA_ARGS__)

#define list_add(...) LIST_DISPATCH(add, __VA_ARGS__)

#define list_add_first(...) LIST_DISPATCH(add_first, __VA_ARGS__)

#define list_add_last(...) LIST_DISPATCH(add_last, __VA_ARGS__)

#define list_add_all(...) LIST_DISPATCH(add_all, __VA_ARGS__)

#define list_add_all_first(...) LIST_DISPATCH(add_all_first, __VA_ARGS__)

#define list_add_all_last(...) LIST_DISPATCH(add_all_last, __VA_ARGS__)

#define list_get(...) LIST_DISPATCH(get, __VA_ARGS__)

#define list_get_first(...) LIST_DISPATCH(get_first, __VA_ARGS__)

#define list_get_last(...) LIST_DISPATCH(get_last, __VA_ARGS__)

#define list_set(...) LIST_DISPATCH(set, __VA_ARGS__)

#define list_swap(...) LIST_DISPATCH(swap, __VA_ARGS__)

#define list_remove(...) LIST_DISPATCH(remove, __VA_ARGS__)

#define list_remove_first(...) LIST_DISPATCH(remove_first, __VA_ARGS__)

#define list_remove_last(...) LIST_DISPATCH(remove_last, __VA_ARGS__)

#define list_remove_element(...) LIST_DISPATCH(remove_element, __VA_ARGS__)

#define list_remove_all(...) LIST_DISPATCH(remove_all, __VA_ARGS__)

#define list_remove_range(...) LIST_DISPATCH(remove_range, __VA_ARGS__)

#define list_remove_if(...) LIST_DISPATCH(remove_if, __VA_ARGS__)

#define list_replace_all(...) LIST_DISPATCH(replace_all, __VA_ARGS__)

#define list_retain_all(...) LIST_DISPATCH(retain_all, __VA_ARGS__)

#define list_size(...) LIST_DISPATCH(size, __VA_ARGS__)

#define list_is_empty(...) LIST_DISPATCH(is_empty, __VA_ARGS__)

#define list_iterator(...) LIST_DISPATCH(iterator, __VA_ARGS__)

#define list_iterator_at(...) LIST_DISPATCH(iterator_at, __VA_ARGS__)

#define list_equals(...) LIST_DISPATCH(equals, __VA_ARGS__)

#define list_for_each(...) LIST_DISPATCH(for_each, __VA_ARGS__)

#define list_sort(...) LIST_DISPATCH(sort, __VA_ARGS__)

#define list_shuffle(...) LIST_DISPATCH(shuffle, __VA_ARGS__)

#define list_reverse(...) LIST_DISPATCH(reverse, __VA_ARGS__)

#define list_rotate(...) LIST_DISPATCH(rotate, __VA_ARGS__)

#define list_clear(...) LIST_DISPATCH(clear, __VA_ARGS__)

#define list_find(...) LIST_DISPATCH(find, __VA_ARGS__)

#define list_find_last(...) LIST_DISPATCH(find_last, __VA_ARGS__)

#define list_index_where(...) LIST_DISPATCH(index_where, __VA_ARGS__)

#define list_last_index_where(...) LIST_DISPATCH(last_index_where, __VA_ARGS__)

#define list_contains(...) LIST_DISPATCH(contains, __VA_ARGS__)

#define list_contains_all(...) LIST_DISPATCH(contains_all, __VA_ARGS__)

#define list_reduce(...) LIST_DISPATCH(reduce, __VA_ARGS__)

#define list_occurrences_of(...) LIST_DISPATCH(occurrences_of, __VA_ARGS__)

#define list_index_of(...) LIST_DISPATCH(index_of, __VA_ARGS__)

#define list_last_index_of(...) LIST_DISPATCH(last_index_of, __VA_ARGS__)

#define list_clone(...) LIST_DISPATCH(clone, __VA_ARGS__)

#define list_sub_list(...) LIST_DISPATCH(sub_list, __VA_ARGS__)

#define list_to_collection(...) LIST_DISPATCH(to_collection, __VA_ARGS__)

#define list_to_array(...) LIST_DISPATCH(to_array, __VA_ARGS__)

#define list_to_string(...) LIST_DISPATCH(to_string, __VA_ARGS__)

#endif