#ifndef LIBCDSA_H
#define LIBCDSA_H

/**
 * @file libcdsa.h
 * @brief Master header for the libcdsa library.
 *
 * Aggregates all public modules (including optional utilities)
 * into a single entry point. Include this file to access the
 * complete libcdsa API instead of including individual headers.
 *
 * This file also serves as a documentation index. Check each module below for more information.
 */

/** @brief Version information macros */
#include "core/version.h"

/** @brief Optioned type aliases for existing c types */
#include "core/types.h"

/** @brief Utility macros */
#include "core/macros.h"

/** @brief Custom assertions */
#include "core/assertions.h"

/** @brief Error handling mechanism */
#include "core/errors.h"

/** @brief Validation and constraint utilities */
#include "core/constraints.h"

/** @brief Memory management abstractions */
#include "core/memory.h"

/** @brief Array implementation */
#include "core/array.h"

/** @brief String implementation */
#include "core/string.h"

/** @brief Generic function pointer types for functional-style operations */
#include "core/functions.h"

/** @brief Pointer functions */
#include "core/pointer.h"

/** @brief Optional value container */
#include "core/optional.h"

/** @brief Runtime System abstraction */
#include "core/system.h"

/** @brief Dynamic array-based list implementation */
#include "collections/list/array_list.h"

/** @brief Doubly-linked list implementation */
#include "collections/list/linked_list.h"

/** @brief List compile-time dispatch abstraction */
#include "collections/list/list.h"

/** @brief A map entry */
#include "collections/map/map_entry.h"

/** @brief Hash table based map implementation */
#include "collections/map/hash_map.h"

/** @brief Self-balancing binary tree based map implementation */
#include "collections/map/tree_map.h"

/** @brief Map compile-time dispatch abstraction */
#include "collections/map/map.h"

/** @brief Hash based set implementation */
#include "collections/set/hash_set.h"

/** @brief Self-balancing binary tree based set implementation */
#include "collections/set/tree_set.h"

/** @brief Set compile-time dispatch abstraction and common mathematical set operations */
#include "collections/set/set.h"

/** @brief Double-ended queue implemented with a resizable array */
#include "collections/deque.h"

/** @brief LIFO stack implemented with a resizable array */
#include "collections/stack.h"

/** @brief FIFO queue implemented with a resizable array */
#include "collections/queue.h"

/** @brief Heap based priority queue implementation */
#include "collections/priority_queue.h"

/** @brief Algorithm-related enumerations */
#include "collections/algorithms.h"

/** @brief Collection abstraction view */
#include "collections/collection.h"

/** @brief Iterator abstraction */
#include "collections/iterator.h"

/** @brief for-each iteration macro utility */
#include "collections/for_each.h"

/** @brief Simple file handling abstraction */
#include "util/file.h"

/** @brief Thread synchronization abstractions */
#include "util/sync.h"

/** @brief Sequence API */
#include "util/sequence.h"

/** @brief Custom memory allocators */
#include "util/allocators.h"

/** @brief Simple logging utility */
#include "util/logging.h"

/** @brief Temporal API */
#include "util/temporal.h"

#endif