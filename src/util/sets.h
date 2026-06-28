#ifndef LIBCDSA_SETS_H
#define LIBCDSA_SETS_H

#include "iterator.h"
#include "collection.h"
#include "pair.h"

/**
 * @file sets.h
 * @brief Common mathematical set operations and set view abstraction.
 */

/**
 * @brief Set view abstraction.
 *
 * SetView provides a lightweight, non-owning view over a set or a relation between sets.
 */
typedef struct SetView {
    Pair sets;
    int (*size)(const void*);
    Iterator* (*iterator)(const void*);
    bool (*contains)(const void*, const void*);
} SetView;

/**
 * @brief Retrieves the current size of the set view.
 *
 * @param set_view pointer to a set view
 *
 * @return number of elements in the set view
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to create underlying iterators while calculating
 */
static inline int set_view_size(const SetView* set_view) {
    return set_view->size(&set_view->sets);
}

/**
 * @brief Checks whether the set view is empty.
 *
 * @param set_view pointer to a set view
 *
 * @return true if empty, false otherwise
 *
 * @exception MEMORY_ALLOCATION_ERROR if failed to create underlying iterators while calculating
 */
static inline bool set_view_is_empty(const SetView* set_view) {
    return set_view_size(set_view) == 0;
}

/**
 * @brief Creates an iterator for the set view.
 *
 * The implementation uses the underlying iterators of the sets the set view represents.
 *
 * @param set_view pointer to a set view
 *
 * @return pointer to a newly created Iterator
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
static inline Iterator* set_view_iterator(const SetView* set_view) {
    return set_view->iterator(&set_view->sets);
}

/**
 * @brief Checks whether an element is present in the set view.
 *
 * @param set_view pointer to a set view
 * @param element the element to check
 *
 * @return true if the element is present, false otherwise
 */
static inline bool set_view_contains(const SetView* set_view, const void* element) {
    return set_view->contains(&set_view->sets, element);
}

/**
 * @brief Returns a Collection view of the set view.
 *
 * The returned collection does not own the elements.
 *
 * @param set_view pointer to a set view
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if set_view is null
 */
Collection set_view_to_collection(const SetView* set_view);

/**
 * @brief Returns a set view representing the union of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 *
 * @return a set view containing the resulting union
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_union(set_a, set_b) set_view_union(set_view_of(set_a), set_view_of(set_b))

SetView set_view_union(SetView*, SetView*);

/**
 * @brief Returns a set view representing the intersection of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 *
 * @return a set view containing the intersection
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_intersection(set_a, set_b) set_view_intersection(set_view_of(set_a), set_view_of(set_b))

SetView set_view_intersection(SetView*, SetView*);

/**
 * @brief Returns a set view representing the difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 *
 * @return a set view containing the difference
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_difference(set_a, set_b) set_view_difference(set_view_of(set_a), set_view_of(set_b))

SetView set_view_difference(SetView*, SetView*);

/**
 * @brief Returns a set view representing the symmetrical difference of two sets.
 *
 * @param set_a the first set
 * @param set_b the second set
 *
 * @return a set view containing the symmetrical difference
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_symmetric_difference(set_a, set_b) set_view_symmetric_difference(set_view_of(set_a), set_view_of(set_b))

SetView set_view_symmetric_difference(SetView*, SetView*);

/**
 * @brief Checks whether one set is a subset of another set.
 *
 * @param set_a the subset
 * @param set_b the superset
 *
 * @return true if yes, false otherwise
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_is_subset(set_a, set_b) set_view_is_subset(set_view_of(set_a), set_view_of(set_b))

bool set_view_is_subset(SetView*, SetView*);

/**
 * @brief Checks whether one set is the superset of another set.
 *
 * @param set_a the superset
 * @param set_b the subset
 *
 * @return true if yes, false otherwise
 *
 * @exception NULL_POINTER_ERROR if set_a or set_b are null
 *
 * @note set_a and set_b could be a HashSet*, TreeSet* or another SetView*
 */
#define sets_is_superset(set_a, set_b) set_view_is_superset(set_view_of(set_a), set_view_of(set_b))

bool set_view_is_superset(SetView*, SetView*);

/**
 * @brief Returns a set view of any set (including another SetView).
 *
 * @param set pointer to a set
 *
 * @return a set view representation
 */
#define set_view_of(set) _Generic((set),    \
    set_view_case_hash_set(set)             \
    set_view_case_tree_set(set)             \
    SetView*: set                           \
)

#endif

#ifdef LIBCDSA_HASH_SET_H
    extern SetView* hash_set_view(const HashSet*);
#   define set_view_case_hash_set(set) HashSet*: hash_set_view((HashSet*) set),
#else
#   define set_view_case_hash_set(set)
#endif

#ifdef LIBCDSA_TREE_SET_H
    extern SetView* tree_set_view(const TreeSet*);
#   define set_view_case_tree_set(set) TreeSet*: tree_set_view((TreeSet*) set),
#else
#   define set_view_case_tree_set(set)
#endif