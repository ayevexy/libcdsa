#ifndef FOR_EACH_H
#define FOR_EACH_H

#include "util/collection.h"
#include "util/iterator.h"

/**
 * @brief A utility macro to iterate sequentially through the elements of a collection.
 *
 * @param variable the variable which will hold the current element
 * @param collection the collection to iterate through
 */
#define for_each(variable, collection)                                                              \
    for (Iterator* _iter_ = _dispatch_collection(collection); _iter_; iterator_destroy(&_iter_))    \
        for (void* _value_; iterator_has_next(_iter_) && (_value_ = iterator_next(_iter_));)        \
            for (variable = _value_, * _flag_ = (void*) 0xDEADBEEF; _flag_; _flag_ = nullptr)


#define _dispatch_collection(collection) _Generic((collection),     \
    _collection_case_array_list                                     \
    _collection_case_linked_list                                    \
    _collection_case_hash_map                                       \
    _collection_case_tree_map                                       \
    _collection_case_hash_set                                       \
    _collection_case_tree_set                                       \
    _collection_case_deque                                          \
    _collection_case_priority_queue                                 \
    _collection_case_set_view                                       \
    Collection: collection_iterator                                 \
)(collection)

#endif

#ifdef ARRAY_LIST_H
#   define _collection_case_array_list ArrayList*: array_list_iterator,
#else
#   define _collection_case_array_list
#endif

#ifdef LINKED_LIST_H
#   define _collection_case_linked_list LinkedList*: linked_list_iterator,
#else
#   define _collection_case_linked_list
#endif

#ifdef HASH_MAP_H
#   define _collection_case_hash_map HashMap*: hash_map_iterator,
#else
#   define _collection_case_hash_map
#endif

#ifdef TREE_MAP_H
#   define _collection_case_tree_map TreeMap*: tree_map_iterator,
#else
#   define _collection_case_tree_map
#endif

#ifdef HASH_SET_H
#   define _collection_case_hash_set HashSet*: hash_set_iterator,
#else
#   define _collection_case_hash_set
#endif

#ifdef TREE_SET_H
#   define _collection_case_tree_set TreeSet*: tree_set_iterator,
#else
#   define _collection_case_tree_set
#endif

#ifdef DEQUE_H
#   define _collection_case_deque Deque*: deque_iterator,
#else
#   define _collection_case_deque
#endif

#ifdef PRIORITY_QUEUE_H
#   define _collection_case_priority_queue PriorityQueue*: priority_queue_iterator,
#else
#   define _collection_case_priority_queue
#endif

#ifdef SET_VIEW_H
#   define _collection_case_set_view SetView*: set_view_iterator,
#else
#   define _collection_case_set_view
#endif