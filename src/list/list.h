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

#define list_change_destructor(list, destructor) LIST_DISPATCH(change_destructor, list, destructor)

#define list_add(list, index, element) LIST_DISPATCH(add, list, index, element)

#define list_add_first(list, element) LIST_DISPATCH(add_first, list, element)

#define list_add_last(list, element) LIST_DISPATCH(add_last, list, element)

#define list_add_all(list, index, collection) LIST_DISPATCH(add_all, list, index, collection)

#define list_add_all_first(list, collection) LIST_DISPATCH(add_all_first, list, collection)

#define list_add_all_last(list, collection) LIST_DISPATCH(add_all_last, list, collection)

#define list_get(list, index) LIST_DISPATCH(get, list, index)

#define list_get_first(list) LIST_DISPATCH(get_first, list)

#define list_get_last(list) LIST_DISPATCH(get_last, list)

#define list_set(list, index, element) LIST_DISPATCH(set, list, index, element)

#define list_swap(list, index_a, index_b) LIST_DISPATCH(swap, list, index_a, index_b)

#define list_remove(list, index) LIST_DISPATCH(remove, list, index)

#define list_remove_first(list) LIST_DISPATCH(remove_first, list)

#define list_remove_last(list) LIST_DISPATCH(remove_last, list)

#define list_remove_element(list, element) LIST_DISPATCH(remove_element, list, element)

#define list_remove_all(list, collection) LIST_DISPATCH(remove_all, list, collection)

#define list_remove_range(list, start_index, end_index) LIST_DISPATCH(remove_range, list, start_index, end_index)

#define list_remove_if(list, condition) LIST_DISPATCH(remove_if, list, condition)

#define list_replace_all(list, operator) LIST_DISPATCH(replace_all, list, operator)

#define list_retain_all(list, collection) LIST_DISPATCH(retain_all, list, collection)

#define list_size(list) LIST_DISPATCH(size, list)

#define list_is_empty(list) LIST_DISPATCH(is_empty, list)

#define list_iterator(list) LIST_DISPATCH(iterator, list)

#define list_iterator_at(list, position) LIST_DISPATCH(iterator_at, list, position)

#define list_equals(list, other_list) LIST_DISPATCH(equals, list, other_list)

#define list_for_each(list, action) LIST_DISPATCH(for_each, list, action)

#define list_sort(list, comparator, algorithm) LIST_DISPATCH(sort, list, comparator, algorithm)

#define list_shuffle(list, random, algorithm) LIST_DISPATCH(shuffle, list, random, algorithm)

#define list_reverse(list) LIST_DISPATCH(reverse, list)

#define list_rotate(list, distance) LIST_DISPATCH(rotate, list, distance)

#define list_clear(list) LIST_DISPATCH(clear, list)

#define list_find(list, condition) LIST_DISPATCH(find, list, condition)

#define list_find_last(list, condition) LIST_DISPATCH(find_last, list, condition)

#define list_index_where(list, condition) LIST_DISPATCH(index_where, list, condition)

#define list_last_index_where(list, condition) LIST_DISPATCH(last_index_where, list, condition)

#define list_contains(list, element) LIST_DISPATCH(contains, list, element)

#define list_contains_all(list, collection) LIST_DISPATCH(contains_all, list, collection)

#define list_reduce(list, identity, accumulator) LIST_DISPATCH(reduce, list, identity, accumulator)

#define list_occurrences_of(list, element) LIST_DISPATCH(occurrences_of, list, element)

#define list_index_of(list, element) LIST_DISPATCH(index_of, list, element)

#define list_last_index_of(list, element) LIST_DISPATCH(last_index_of, list, element)

#define list_clone(list) LIST_DISPATCH(clone, list)

#define list_sub_list(list, start_index, end_index) LIST_DISPATCH(sub_list, list, start_index, end_index)

#define list_to_collection(list) LIST_DISPATCH(to_collection, list)

#define list_to_array(list) LIST_DISPATCH(to_array, list)

#define list_to_string(list) LIST_DISPATCH(to_string, list)

#endif