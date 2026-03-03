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

/** @brief Dynamic array-based list implementation */
#include "list/array_list.h"

/** @brief Doubly-linked list implementation */
#include "list/linked_list.h"

/** @brief Hash table based map implementation */
#include "map/hash_map.h"

/** @brief Algorithm-related enumerations */
#include "util/algorithms.h"

/** @brief Collection abstraction view */
#include "util/collection.h"

/** @brief Validation and constraint utilities */
#include "util/constraints.h"

/** @brief Error handling mechanism */
#include "util/errors.h"

/** @brief for-each iteration macro utility */
#include "util/for_each.h"

/** @brief Function types and utilities */
#include "util/functions.h"

/** @brief Iterator abstraction */
#include "util/iterator.h"

/** @brief Optional value container */
#include "util/optional.h"

/** @brief Pair (two-value tuple) container */
#include "util/pair.h"

#endif