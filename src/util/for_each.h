#ifndef LIBCDSA_FOR_EACH_H
#define LIBCDSA_FOR_EACH_H

#include "collection.h"
#include "iterator.h"

/**
 * @brief A utility macro to iterate sequentially through the elements of a collection.
 *
 * @param variable the variable which will hold the current element
 * @param collection the collection to iterate through
 */
#define for_each(variable, collection)                                                          \
    for (Iterator* iter_ = dispatch_collection(collection); iter_; iterator_destroy(&iter_))    \
        for (void* value_; iterator_has_next(iter_) && (value_ = iterator_next(iter_));)        \
            for (variable = value_, * flag_ = (void*) 0xDEADBEEF; flag_; flag_ = nullptr)


#define dispatch_collection(collection) _Generic((collection),  \
    collection_case_array_list                                  \
    collection_case_linked_list                                 \
    collection_case_hash_map                                    \
    collection_case_tree_map                                    \
    collection_case_hash_set                                    \
    collection_case_tree_set                                    \
    collection_case_deque                                       \
    collection_case_priority_queue                              \
    collection_case_set_view                                    \
    Collection: collection_iterator                             \
)(collection)

#endif

#ifdef LIBCDSA_ARRAY_LIST_H
#   define collection_case_array_list ArrayList*: array_list_iterator,
#else
#   define collection_case_array_list
#endif

#ifdef LIBCDSA_LINKED_LIST_H
#   define collection_case_linked_list LinkedList*: linked_list_iterator,
#else
#   define collection_case_linked_list
#endif

#ifdef LIBCDSA_HASH_MAP_H
#   define collection_case_hash_map HashMap*: hash_map_iterator,
#else
#   define collection_case_hash_map
#endif

#ifdef LIBCDSA_TREE_MAP_H
#   define collection_case_tree_map TreeMap*: tree_map_iterator,
#else
#   define collection_case_tree_map
#endif

#ifdef LIBCDSA_HASH_SET_H
#   define collection_case_hash_set HashSet*: hash_set_iterator,
#else
#   define collection_case_hash_set
#endif

#ifdef LIBCDSA_TREE_SET_H
#   define collection_case_tree_set TreeSet*: tree_set_iterator,
#else
#   define collection_case_tree_set
#endif

#ifdef LIBCDSA_DEQUE_H
#   define collection_case_deque Deque*: deque_iterator,
#else
#   define collection_case_deque
#endif

#ifdef LIBCDSA_PRIORITY_QUEUE_H
#   define collection_case_priority_queue PriorityQueue*: priority_queue_iterator,
#else
#   define collection_case_priority_queue
#endif

#ifdef LIBCDSA_SETS_H
#   define collection_case_set_view SetView*: set_view_iterator,
#else
#   define collection_case_set_view
#endif