#ifndef TREE_MAP_H
#define TREE_MAP_H

#include "map_entry.h"
#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

// TODO: document
/**
 * A tree map is a...
 * It automatically resizes to accommodate new entries and allows insertion of values of any type.
 *
 * Internally, the implementation uses...                                           .Collisions are
 * resolved by...    All operations on a tree map receive a pointer to the tree map itself as their first argument.
 * The TreeMap type is opaque and can only be modified through the API.
 *
 * It must be configured using a TreeMapOptions defining:
 * - the function to compare keys
 * - the destruct function utilized to free key memory
 * - the equals function utilized to compare keys
 * - the to string function utilized to convert its keys to a string representation
 * - the destruct function utilized to free value memory
 * - the equals function utilized to compare values
 * - the to string function utilized to convert its values to a string representation
 * - the function used internally to allocate memory
 * - the function used internally to free memory
 *
 * Underlying implementation (simplified):
 * @code
 * struct TreeMap {
 *      Node* root;
 *      int size;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct TreeMap TreeMap;

/**
 * TreeMap configuration structure. Used to define the default behavior and attributes of a TreeMap.
 *
 * @pre compare_keys != nullptr
 * @pre key_destruct != nullptr
 * @pre key_equals != nullptr
 * @pre key_to_string != nullptr
 * @pre value_destruct != nullptr
 * @pre value_equals != nullptr
 * @pre value_to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_free != nullptr
 */
typedef struct {
    Comparator compare_keys;
    struct {
        void (*key_destruct)(void*);
        bool (*key_equals)(const void*, const void*);
        int (*key_to_string)(const void*, char*, size_t);
    };
    struct {
        void (*value_destruct)(void*);
        bool (*value_equals)(const void*, const void*);
        int (*value_to_string)(const void*, char*, size_t);
    };
    struct {
        void* (*memory_alloc)(size_t);
        void (*memory_free)(void*);
    };
} TreeMapOptions;

/**
 * @brief A utility macro that provides a reasonable default TreeMapOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_TREE_MAP_OPTIONS(...) &(TreeMapOptions) {   \
    .compare_keys = compare_pointers,                       \
    .key_destruct = noop_destruct,                          \
    .key_equals = pointer_equals,                           \
    .key_to_string = pointer_to_string,                     \
    .value_destruct = noop_destruct,                        \
    .value_equals = pointer_equals,                         \
    .value_to_string = pointer_to_string,                   \
    .memory_alloc = malloc,                                 \
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

/**
 * @brief Creates a new empty TreeMap using the specified options.
 *
 * @param options pointer to an TreeMapOptions defining the tree map configuration
 *
 * @return pointer to the newly created TreeMap on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the TreeMap fails
 */
TreeMap* tree_map_new(const TreeMapOptions* options);

/**
 * @brief Creates a new TreeMap populated with entries from the provided Collection using the specified options.
 *
 * Each element of the collection must be a pointer to a MapEntry structure.
 * The entries are inserted into the new TreeMap using their respective keys and values.
 *
 * @param entry_collection a Collection containing the entries to be added
 * @param options pointer to a TreeMapOptions containing configuration settings
 *
 * @return pointer to the newly created TreeMap on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the TreeMap fails or entry_collection's iterator fails
 */
TreeMap* tree_map_from(Collection entry_collection, const TreeMapOptions* options);

/**
 * @brief Destroys an existing TreeMap and (optionally) its entries using the provided destruct functions.
 *
 * @param tree_map_pointer pointer to a TreeMap pointer
 *
 * @exception NULL_POINTER_ERROR if tree_map_pointer or *tree_map_pointer is null
 *
 * @post *tree_map_pointer == nullptr
 */
void tree_map_destroy(TreeMap** tree_map_pointer);

/**
 * @brief Sets the key destructor used by the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 * @param destructor function used to destroy keys
 *
 * @exception NULL_POINTER_ERROR if tree_map or destructor are null
 */
void tree_map_set_key_destructor(TreeMap* tree_map, void(*destructor)(void*));

/**
 * @brief Sets the value destructor used by the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 * @param destructor function used to destroy values
 *
 * @exception NULL_POINTER_ERROR if tree_map or destructor are null
 */
void tree_map_set_value_destructor(TreeMap* tree_map, void(*destructor)(void*));

/**
 * @brief Computes a new value for the specified key using the provided remapping function.
 *
 * The remapper receives the key and the current value associated with the key (or nullptr if absent).
 * If the remapper returns nullptr, the entry is removed if it exists.
 * Otherwise, the returned value is stored as the new mapping.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param remapper function used to compute the new value
 *
 * @return the new value associated with the key, or nullptr if removed
 *
 * @exception NULL_POINTER_ERROR if tree_map or remapper is null
 */
void* tree_map_compute(TreeMap* tree_map, const void* key, BiOperator remapper);

/**
 * @brief Computes a value for the specified key only if it is not already present.
 *
 * If the key is absent, the mapper function is invoked with the key to compute a value.
 * If the computed value is not nullptr, it is inserted into the TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param mapper function used to compute the value
 *
 * @return the current or newly computed value associated with the key
 *
 * @exception NULL_POINTER_ERROR if tree_map or mapper is null
 */
void* tree_map_compute_if_absent(TreeMap* tree_map, const void* key, Operator mapper);

/**
 * @brief Computes a new value for the specified key only if it is currently present.
 *
 * The remapper receives the key and the current value.
 * If the remapper returns nullptr, the entry is removed.
 * Otherwise, the returned value replaces the existing value.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param remapper function used to compute the new value
 *
 * @return the new value associated with the key, or nullptr if removed or not present
 *
 * @exception NULL_POINTER_ERROR if tree_map or remapper is null
 */
void* tree_map_compute_if_present(TreeMap* tree_map, const void* key, BiOperator remapper);

/**
 * @brief Merges the provided value with the existing value associated with the key.
 *
 * If the key is not present, the value is inserted.
 * If the key is present, the remapper is invoked with the existing value and the given value.
 * If the remapper returns nullptr, the entry is removed.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param value pointer to the value to merge
 * @param remapper function used to merge values
 *
 * @return the new value associated with the key, or nullptr if removed
 *
 * @exception NULL_POINTER_ERROR if tree_map or remapper is null
 */
void* tree_map_merge(TreeMap* tree_map, const void* key, const void* value, BiOperator remapper);

/**
 * @brief Associates the specified value with the specified key.
 *
 * If the key already exists, the existing value is replaced (optionally destructed) and returned.
 * Otherwise, a new entry is created.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the previous value associated with the key, or nullptr if none
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand tree_map capacity on resize or creating a new entry
 */
void* tree_map_put(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Associates the specified value with the specified key only if it is absent.
 *
 * If the key already exists, the existing value is returned and no modification occurs.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the existing value if present, otherwise nullptr
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand tree_map capacity on resize or creating a new entry
 */
void* tree_map_put_if_absent(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Inserts all entries from the provided entry collection into the TreeMap.
 *
 * Each element of the collection must be a pointer to a MapEntry structure.
 * Existing mappings are replaced if keys collide.
 *
 * @param tree_map pointer to a TreeMap
 * @param entry_collection collection containing Entry elements
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand tree_map capacity on resize or creating a new entry or entry collection's iterator fails
 */
void tree_map_put_all(TreeMap* tree_map, Collection entry_collection);

/**
 * @brief Retrieves the value associated with the specified key.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 *
 * @return the associated value, or nullptr if the key is not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_get(const TreeMap* tree_map, const void* key);

/**
 * @brief Retrieves the value associated with the specified key, or returns a default value.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param default_value value returned if the key is not present
 *
 * @return the associated value, or default_value if absent
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_get_or_default(const TreeMap* tree_map, const void* key, const void* default_value);

/**
 * @brief Retrieves the first entry of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_first_entry(const TreeMap* tree_map);

/**
 * @brief Retrieves the last entry of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_last_entry(const TreeMap* tree_map);

/**
 * @brief Retrieves the first entry's key of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the entry's key, or nullptr if empty
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
void* tree_map_first_key(const TreeMap* tree_map);

/**
 * @brief Retrieves the last entry's key of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the entry's key, or nullptr if empty
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
void* tree_map_last_key(const TreeMap* tree_map);

/**
 * @brief Replaces the value associated with the specified key.
 *
 * If the key is present, the value is replaced and the previous value is (optionally) destructed and returned.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param value pointer to the new value
 *
 * @return the previous value, or nullptr if the key was not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_replace(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Replaces the value associated with the specified key only if it matches the expected value, (optionally) destructing the old value.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key
 * @param old_value expected current value
 * @param value new value to store
 *
 * @return true if replacement occurred, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_replace_if_equals(TreeMap* tree_map, const void* key, const void* old_value, const void* value);

/**
 * @brief Removes the entry matching the given key of the provided TreeMap, (optionally) destructing it.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key to search for removal
 *
 * @return pointer to the removed entry's value, or nullptr if not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_remove(TreeMap* tree_map, const void* key);

/**
 * @brief Removes the entry matching the given key and value of the provided TreeMap, (optionally) destructing it.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key to search for removal
 * @param value pointer to the value to search for removal
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_remove_if_equals(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Removes the first entry of the provided TreeMap, (optionally) destructing it.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_poll_first_entry(TreeMap* tree_map);

/**
 * @brief Removes the last entry of the provided TreeMap, (optionally) destructing it.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_poll_last_entry(TreeMap* tree_map);

/**
 * @brief Replaces all entries using the given BiOperator of the provided TreeMap, (optionally) destructing the old entries.
 *
 * @param tree_map pointer to a TreeMap
 * @param bi_operator the bi-operator to replace entries
 *
 * @exception NULL_POINTER_ERROR if tree_map or bi_operator is null
 */
void tree_map_replace_all(TreeMap* tree_map, BiOperator bi_operator);

/**
 * @brief Retrieves the current size of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return the current size of the provided TreeMap
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
int tree_map_size(const TreeMap* tree_map);

/**
 * @brief Checks whether the provided TreeMap is empty.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_is_empty(const TreeMap* tree_map);

/**
 * @brief Instantiates an Iterator for the provided TreeMap.
 *
 * The iteration order of entries is unspecified and may change when the tree map resizes.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 *
 * @note this iterator doesn't support backward traversal nor adding or setting elements
 */
Iterator* tree_map_iterator(const TreeMap* tree_map);

/**
 * @brief Checks whether two TreeMap objects are equal.
 *
 * Two tree maps are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and each corresponding entry in the first tree map is considered equal to the element
 * of the second tree map according to both key_equals and value_equals functions of the first tree map.
 *
 * @param tree_map pointer to a TreeMap
 * @param other_tree_map pointer to a TreeMap
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_equals(const TreeMap* tree_map, const TreeMap* other_tree_map);

/**
 * @brief Performs an action for each entry of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if tree_map or action is null
 */
void tree_map_for_each(TreeMap* tree_map, BiConsumer action);

/**
 * @brief Removes all entries of the provided TreeMap, (optionally) destructing them.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void tree_map_clear(TreeMap* tree_map);

// TODO: document
MapEntry tree_map_higher_entry(const TreeMap* tree_map, const void* key);

MapEntry tree_map_ceiling_entry(const TreeMap* tree_map, const void* key);

MapEntry tree_map_floor_entry(const TreeMap* tree_map, const void* key);

MapEntry tree_map_lower_entry(const TreeMap* tree_map, const void* key);

void* tree_map_higher_key(const TreeMap* tree_map, const void* key);

void* tree_map_ceiling_key(const TreeMap* tree_map, const void* key);

void* tree_map_floor_key(const TreeMap* tree_map, const void* key);

void* tree_map_lower_key(const TreeMap* tree_map, const void* key);

/**
 * @brief Checks whether the provided TreeMap contains the specified entry.
 *
 * Comparison is performed using both key_equals and value_equals functions configured in the TreeMapOptions.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key of the entry to be checked
 * @param value pointer to the value of the entry to be checked
 *
 * @return true if the entry is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains_entry(const TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Checks whether the provided TreeMap contains the specified key.
 *
 * Comparison is performed using the key_equals function configured in the TreeMapOptions.
 *
 * @param tree_map pointer to a TreeMap
 * @param key pointer to the key to be checked
 *
 * @return true if the key is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains_key(const TreeMap* tree_map, const void* key);

/**
 * @brief Checks whether the provided TreeMap contains the specified value.
 *
 * Comparison is performed using the value_equals function configured in the TreeMapOptions.
 *
 * @param tree_map pointer to a TreeMap
 * @param value pointer to the value to be checked
 *
 * @return true if the value is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains_value(const TreeMap* tree_map, const void* value);

/**
 * @brief Returns a Collection view of the keys of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return a Collection view containing the TreeMap's keys
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_keys(const TreeMap* tree_map);

/**
 * @brief Returns a Collection view of the values of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return a Collection view containing the TreeMap's values
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_values(const TreeMap* tree_map);

/**
 * @brief Returns a Collection view of the entries of the provided TreeMap.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return a Collection view containing the TreeMap's entries
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_entries(const TreeMap* tree_map);

// TODO: document
TreeMap* tree_map_reversed(const TreeMap* tree_map);

// TODO: document
TreeMap* tree_map_filter(const TreeMap* tree_map, const void* key);

/**
 * @brief Creates a submap of the provided TreeMap within the specified range.
 *
 * The returned TreeMap contains elements from start_key (inclusive)
 * to end_key (exclusive).
 *
 * @param tree_map pointer to a TreeMap
 * @param start_key start key (inclusive)
 * @param end_key end key (exclusive)
 *
 * @return a newly created TreeMap containing the specified range, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_sub_map(const TreeMap* tree_map, const void* start_key, const void* end_key);

/**
 * @brief Creates a shallow copy of the provided TreeMap.
 *
 * The new TreeMap will contain the same mappings but will have independent internal storage.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return a newly created TreeMap clone, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_clone(const TreeMap* tree_map);

/**
 * @brief Converts the provided TreeMap to a string representation.
 *
 * Each entry is converted using both key_to_string and value_to_string functions configured
 * in the TreeMapOptions.
 *
 * @param tree_map pointer to a TreeMap
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* tree_map_to_string(const TreeMap* tree_map);

#endif