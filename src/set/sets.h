#ifndef LIBCDSA_SETS_H
#define LIBCDSA_SETS_H

/**
 * @file sets.h
 * @brief Common mathematical set operations.
 */

#include "set.h"

/**
 * @brief Returns a set representing the union of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the resulting union
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_union(set_a, set_b, result) _Generic((result),     \
    HashSet*: hash_set_union,                                   \
    TreeSet*: tree_set_union                                    \
)(set_to_collection(set_a), set_to_collection(set_b), result)

HashSet* hash_set_union(Collection set_a, Collection set_b, HashSet* result_set);

TreeSet* tree_set_union(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the intersection of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the intersection
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_intersection(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_intersection,                                \
    TreeSet*: tree_set_intersection                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

HashSet* hash_set_intersection(Collection set_a, Collection set_b, HashSet* result_set);

TreeSet* tree_set_intersection(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the difference
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_difference(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_difference,                                \
    TreeSet*: tree_set_difference                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

HashSet* hash_set_difference(Collection set_a, Collection set_b, HashSet* result_set);

TreeSet* tree_set_difference(Collection set_a, Collection set_b, TreeSet* result_set);

/**
 * @brief Returns a set representing the symmetrical difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 * @param result the result set
 *
 * @return a set containing the symmetrical difference
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed to add elements in the result set
 */
#define sets_symmetric_difference(set_a, set_b, result) _Generic((result),  \
    HashSet*: hash_set_symmetric_difference,                                \
    TreeSet*: tree_set_symmetric_difference                                 \
)(set_to_collection(set_a), set_to_collection(set_b), result)

HashSet* hash_set_symmetric_difference(Collection set_a, Collection set_b, HashSet* result_set);

TreeSet* tree_set_symmetric_difference(Collection set_a, Collection set_b, TreeSet* result_set);
/**
 * @brief Checks whether one set is a subset of another set.
 *
 * @param set_a the subset
 * @param set_b the superset
 *
 * @return true if yes, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed create set iterator for traversal
 */
#define sets_is_subset(set_a, set_b) sets_is_subset(set_to_collection(set_a), set_to_collection(set_b))

bool (sets_is_subset)(Collection set_a, Collection set_b);

/**
 * @brief Checks whether one set is the superset of another set.
 *
 * @param set_a the superset
 * @param set_b the subset
 *
 * @return true if yes, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR If failed create set iterator for traversal
 */
#define sets_is_superset(set_a, set_b) sets_is_superset(set_to_collection(set_a), set_to_collection(set_b))

bool (sets_is_superset)(Collection set_a, Collection set_b);

#endif