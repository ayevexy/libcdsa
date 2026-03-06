#ifndef MAP_ENTRY_H
#define MAP_ENTRY_H

/**
 * An entry of a map, containing the key-value pair.
 */
typedef struct {
    void* key;
    void* value;
} MapEntry;

#endif