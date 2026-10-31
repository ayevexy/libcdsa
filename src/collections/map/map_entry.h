#ifndef LIBCDSA_MAP_ENTRY_H
#define LIBCDSA_MAP_ENTRY_H

/**
 * @file map_entry.h
 * @brief A map entry.
 */

/**
 * @brief An entry of a map, containing the key-value pair.
 */
typedef struct {
    const void* key;
    void* value;
} MapEntry;

#endif