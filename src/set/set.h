#ifndef LIBCDSA_SET_H
#define LIBCDSA_SET_H

/**
 * @file set.h
 * @brief Set compile-time dispatch abstraction and common mathematical set operations.
 */

#include "hash_set.h"
#include "tree_set.h"

/**
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
 * set_add(tree_set, &(int){20}); // Calls tree_set_add()
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

#define set_destructor(...) SET_DISPATCH(destructor, __VA_ARGS__)

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

/**
 * @brief Returns a set representing the union of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the resulting union
 *
 * @exception NULL_POINTER_ERROR If result is null
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_union(set_a, set_b, result) _Generic((result),     \
    HashSet*: hash_set_union,                                   \
    TreeSet*: tree_set_union                                    \
)(set_to_collection(set_a), set_to_collection(set_b), result)

extern HashSet* hash_set_union(Collection set_a, Collection set_b, HashSet* result_set);

extern TreeSet* tree_set_union(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the intersection of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the intersection
 *
 * @exception NULL_POINTER_ERROR If result is null
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_intersection(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_intersection,                                \
    TreeSet*: tree_set_intersection                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

extern HashSet* hash_set_intersection(Collection set_a, Collection set_b, HashSet* result_set);

extern TreeSet* tree_set_intersection(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the difference
 *
 * @exception NULL_POINTER_ERROR If result is null
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_difference(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_difference,                                \
    TreeSet*: tree_set_difference                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

extern HashSet* hash_set_difference(Collection set_a, Collection set_b, HashSet* result_set);

extern TreeSet* tree_set_difference(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the symmetrical difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the symmetric difference
 *
 * @exception NULL_POINTER_ERROR If result is null
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_symmetric_difference(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_symmetric_difference,                                \
    TreeSet*: tree_set_symmetric_difference                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

extern HashSet* hash_set_symmetric_difference(Collection set_a, Collection set_b, HashSet* result_set);

extern TreeSet* tree_set_symmetric_difference(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Checks whether one set is a subset of another set.
 *
 * @param set_a the subset
 * @param set_b the superset
 *
 * @return true if yes, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR if creating an iterator fails
 */
#define sets_is_subset(set_a, set_b) \
    (collection_contains_all(set_to_collection(set_b), set_to_collection(set_a)))

/**
 * @brief Checks whether one set is the superset of another set.
 *
 * @param set_a the superset
 * @param set_b the subset
 *
 * @return true if yes, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR if creating an iterator fails
 */
#define sets_is_superset(set_a, set_b) \
    (collection_contains_all(set_to_collection(set_a), set_to_collection(set_b)))

#endif