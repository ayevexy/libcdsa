#ifndef FOR_EACH_H
#define FOR_EACH_H

#include "list/array_list.h"
#include "list/linked_list.h"

#include "util/collection.h"
#include "util/iterator.h"

/**
 * @brief A utility macro to iterate sequentially through the elements of a collection.
 *
 * @param variable the variable which will hold the current element
 * @param collection the collection to iterate through
 */
#define for_each(variable, collection)                                                          \
    for (Iterator* _iter_ = _Generic(collection,                                                \
            ArrayList*: array_list_iterator,                                                    \
            LinkedList*: linked_list_iterator                                                   \
        )(collection);                                                                          \
        _iter_;                                                                                 \
        iterator_destroy(&_iter_)                                                               \
    )                                                                                           \
        for (void* _value_; iterator_has_next(_iter_) && (_value_ = iterator_next(_iter_));)    \
            for (variable = _value_, * _flag_ = (void*) 0xDEADBEEF; _flag_; _flag_ = nullptr)

#endif