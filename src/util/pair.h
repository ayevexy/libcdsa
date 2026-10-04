#ifndef LIBCDSA_PAIR_H
#define LIBCDSA_PAIR_H

/**
 * @file pair.h
 * @brief Pair (two-value tuple) container.
 */

/**
 * @brief A container object which contains two independently values.
 */
typedef struct {
    void* first;
    void* second;
} Pair;

#endif