#ifndef LIBCDSA_PAIR_H
#define LIBCDSA_PAIR_H

/**
 * @brief A container object which contains two independently values.
 */
typedef struct {
    void* first;
    void* second;
} Pair;

#endif