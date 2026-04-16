#ifndef LIBCDSA_TREE_MAP_H
#define LIBCDSA_TREE_MAP_H

#include "map_entry.h"
#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"
#include "util/string.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A tree map is an ordered associative data structure that maps keys to values
 * using a comparator function to keep the entries sorted according to their keys.
 * It allows insertion of keys and values of any type.
 *
 * Internally, the implementation uses a self-balancing binary search tree of `Entry*`.
 * All operations receive a pointer to the tree map as their first argument.
 * The TreeMap type is opaque and can only be modified through the public API.
 *
 * A tree map must be configured using a TreeMapOptions structure specifying:
 * - the key comparator function
 * - the destruct function used to free key memory
 * - the equality function used to compare keys
 * - the to_string function used to convert keys to strings
 * - the destruct function used to free value memory
 * - the equality function used to compare values
 * - the to_string function used to convert values to strings
 * - the memory allocation function
 * - the memory deallocation function
 *
 * Underlying implementation (simplified):
 * @code
 * struct TreeMap {
 *      Entry* root;
 *      int size;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the tree map does not own either its keys or values. If destruct functions are
 * provided, it will be invoked when:
 * - mappings are removed (e.g., tree_map_remove, tree_map_remove_if_equals)
 * - mappings are replaced (e.g., tree_map_put, tree_map_replace, tree_map_replace_all)
 * - the tree map is cleared (tree_map_clear)
 * - the tree map is destroyed (tree_map_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - tree_map_put: O(log n)
 * - tree_map_remove: O(log n)
 * - tree_map_get: O(log n)
 * - tree_map_replace: O(log n)
 * - tree_map_contains_entry / contains_key: O(log n)
 * - tree_map_contains_value: O(n)
 */
typedef struct TreeMap TreeMap;

/**
 * TreeMap configuration structure. Defines the behavior and attributes of a tree map.
 *
 * @pre compare_keys != nullptr
 * @pre key_destruct != nullptr
 * @pre key_equals != nullptr
 * @pre key_to_string != nullptr
 * @pre value_destruct != nullptr
 * @pre value_equals != nullptr
 * @pre value_to_string != nullptr
 * @pre memory_alloc != nullptr
 * @pre memory_dealloc != nullptr
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
        void (*memory_dealloc)(void*);
    };
} TreeMapOptions;

/**
 * @brief Utility macro providing default TreeMapOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_TREE_MAP_OPTIONS(...) &(TreeMapOptions) {   \
    .compare_keys = pointer_compare,                        \
    .key_destruct = noop_destruct,                          \
    .key_equals = pointer_equals,                           \
    .key_to_string = pointer_to_string,                     \
    .value_destruct = noop_destruct,                        \
    .value_equals = pointer_equals,                         \
    .value_to_string = pointer_to_string,                   \
    .memory_alloc = malloc,                                 \
    .memory_dealloc = free,                                 \
    __VA_ARGS__                                             \
}

/**
 * @brief Creates a new empty tree map using the specified options.
 *
 * @param options pointer to a TreeMapOptions structure
 *
 * @return pointer to a newly created tree map
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_new(const TreeMapOptions* options);

/**
 * @brief Creates a new tree map containing all entries of the given collection.
 *
 * The collection must contain only `MapEntry*` elements, otherwise, the behavior is undefined.
 *
 * @param entry_collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created tree map
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
TreeMap* tree_map_from(Collection entry_collection, const TreeMapOptions* options);

/**
 * @brief Destroys a tree map and optionally its entries.
 *
 * @param tree_map_pointer pointer to a tree map pointer
 *
 * @exception NULL_POINTER_ERROR if tree_map_pointer or *tree_map_pointer is null
 *
 * @post *tree_map_pointer == nullptr
 */
void tree_map_destroy(TreeMap** tree_map_pointer);

/**
 * @brief Changes the key destruct function.
 *
 * @param tree_map pointer to a tree map
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if tree_map or destruct is null
 */
void tree_map_change_key_destructor(TreeMap* tree_map, void(*destruct)(void*));

/**
 * @brief Changes the value destruct function.
 *
 * @param tree_map pointer to a tree map
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if tree_map or destruct is null
 */
void tree_map_change_value_destructor(TreeMap* tree_map, void(*destruct)(void*));

/**
 * @brief Computes a new value for the specified key using the provided remapping function.
 *
 * The remapper receives the key and the current value associated with the key (or nullptr if absent).
 * If the remapper returns nullptr, the entry is removed if it exists.
 * Otherwise, the returned value is stored as the new mapping.
 *
 * @param tree_map pointer to a tree map
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
 * @param tree_map pointer to a tree map
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
 * @param tree_map pointer to a tree map
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
 * @param tree_map pointer to a tree map
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
 * @brief Associates the specified value with the specified key in the tree map.
 *
 * If the key already exists, the existing value is replaced and returned.
 * Otherwise, a new entry is created.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the previous value associated with the key, or nullptr if none
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new entry fails
 * 
 * @note this function calls the value destruct before returning if the key is already mapped.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* tree_map_put(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Associates the specified value with the specified key only if it is absent in the tree map.
 *
 * If the key already exists, the existing value is returned and no modification occurs.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the existing value if present, otherwise nullptr
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new entry fails
 */
void* tree_map_put_if_absent(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Inserts all entries of a collection into the tree map.
 *
 * The collection must contain only `MapEntry*` elements, otherwise, the behavior is undefined.
 * Existing mappings are replaced if keys collide.
 *
 * @param tree_map pointer to a tree map
 * @param entry_collection collection containing Entry elements
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing, creating a new entry or creation of the collection iterator fails
 * 
 * @note this function calls the value destruct before returning if keys are already mapped.
 */
void tree_map_put_all(TreeMap* tree_map, Collection entry_collection);

/**
 * @brief Retrieves the value associated with the specified key of the tree map.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return the associated value, or nullptr if the key is not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_get(const TreeMap* tree_map, const void* key);

/**
 * @brief Retrieves the value associated with the specified key of the tree map, or returns a default value.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param default_value value returned if the key is not present
 *
 * @return the associated value, or default_value if absent
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
void* tree_map_get_or_default(const TreeMap* tree_map, const void* key, const void* default_value);

/**
 * @brief Retrieves the first entry of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_get_first(const TreeMap* tree_map);

/**
 * @brief Retrieves the last entry of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 */
MapEntry tree_map_get_last(const TreeMap* tree_map);

/**
 * @brief Replaces the value associated with the specified key, if present, in the tree map,
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param value pointer to the new value
 *
 * @return the previous value, or nullptr if the key was not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * 
 * @note this function calls the value destruct before returning.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* tree_map_replace(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Replaces the value associated with the specified key of the tree map, only if it matches the expected value.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param old_value expected current value
 * @param value new value to store
 *
 * @return true if replacement occurred, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * 
 * @note this function calls the value destruct before returning.
 */
bool tree_map_replace_if_equals(TreeMap* tree_map, const void* key, const void* old_value, const void* value);

/**
 * @brief Removes the entry matching the given key of the tree map.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return pointer to the removed entry's value, or nullptr if not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* tree_map_remove(TreeMap* tree_map, const void* key);

/**
 * @brief Removes the entry matching the given key and value of the tree map.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
bool tree_map_remove_if_equals(TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Removes the first entry of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
MapEntry tree_map_remove_first(TreeMap* tree_map);

/**
 * @brief Removes the last entry of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception NO_SUCH_ELEMENT_ERROR if tree_map is empty
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
MapEntry tree_map_remove_last(TreeMap* tree_map);

/**
 * @brief Replaces all entry values using a bi-operator function.
 *
 * @param tree_map pointer to a tree map
 * @param bi_operator bi-operator function
 *
 * @exception NULL_POINTER_ERROR if tree_map or bi_operator is null
 *
 * @note this function calls the value destruct before returning.
 */
void tree_map_replace_all(TreeMap* tree_map, BiOperator bi_operator);

/**
 * @brief Returns the current number of mappings in the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
int tree_map_size(const TreeMap* tree_map);

/**
 * @brief Checks whether the tree map is empty.
 *
 * @param tree_map pointer to a tree map
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_is_empty(const TreeMap* tree_map);

/**
 * @brief Creates an iterator for the tree map.
 *
 * The iterator traverses entries in key-sorted order.
 * The iterator returns objects of type `MapEntry*`.
 *
 * @param tree_map pointer to a tree map
 *
 * @return pointer to a newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add() and set()
 */
Iterator* tree_map_iterator(const TreeMap* tree_map);

/**
 * @brief Creates an iterator for the tree map starting at the specified position.
 *
 * The iterator traverses entries in key-sorted order.
 * The iterator returns objects of type `MapEntry*`.
 *
 * @param tree_map pointer to a tree map
 * @param position start position
 *
 * @return pointer to a newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception INDEX_OUT_OF_BOUNDS_ERROR if position < 0 || position > tree_map.size
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: add() and set()
 */
Iterator* tree_map_iterator_at(const TreeMap* tree_map, int position);

/**
 * @brief Checks whether two tree maps are equal.
 *
 * Two tree maps are equal if:
 * - they reference the same memory address, or
 * - they have the same size, and each entry in the first tree map is present in the second
 *   tree map according to both key_equals and value_equals functions of the first tree map.
 *
 * @param tree_map first tree map
 * @param other_tree_map second tree map
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either tree_map is null
 */
bool tree_map_equals(const TreeMap* tree_map, const TreeMap* other_tree_map);

/**
 * @brief Applies an action to each entry of the tree map.
 *
 * @param tree_map pointer to a tree map
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if tree_map or action is null
 */
void tree_map_for_each(TreeMap* tree_map, BiConsumer action);

/**
 * @brief Removes all entries of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
void tree_map_clear(TreeMap* tree_map);

/**
 * @brief Retrieves the least entry strictly greater than the specified key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
MapEntry tree_map_higher(const TreeMap* tree_map, const void* key);

/**
 * @brief Retrieves the least entry greater than or equal to the specified key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
MapEntry tree_map_ceiling(const TreeMap* tree_map, const void* key);

/**
 * @brief Retrieves the greatest entry less than or equal to the specified key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
MapEntry tree_map_floor(const TreeMap* tree_map, const void* key);

/**
 * @brief Retrieves the greatest entry strictly less than the specified key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return the map entry view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
MapEntry tree_map_lower(const TreeMap* tree_map, const void* key);

/**
 * @brief Checks whether the tree map contains the specified entry.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return true if the entry is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains(const TreeMap* tree_map, const void* key, const void* value);

/**
 * @brief Checks whether the tree map contains the specified key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the key
 *
 * @return true if the key is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains_key(const TreeMap* tree_map, const void* key);

/**
 * @brief Checks whether the tree map contains the specified value.
 *
 * @param tree_map pointer to a tree map
 * @param value pointer to the value
 *
 * @return true if the value is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
bool tree_map_contains_value(const TreeMap* tree_map, const void* value);

/**
 * @brief Returns a collection view of the entries of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_entries(const TreeMap* tree_map);

/**
 * @brief Returns a collection view of the keys of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_keys(const TreeMap* tree_map);

/**
 * @brief Returns a collection view of the values of the tree map.
 *
 * @param tree_map pointer to a tree map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 */
Collection tree_map_values(const TreeMap* tree_map);

/**
 * @brief Creates a view of the portion of the tree map whose keys are less than or equal to the given key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the exclusive upper bound key
 *
 * @return pointer to the newly created tree map
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception ILLEGAL_ARGUMENT_ERROR if key is nonexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_head_map(const TreeMap* tree_map, const void* key);

/**
 * @brief Creates a view of the portion of the tree map whose keys are greater than or equal to the given key.
 *
 * @param tree_map pointer to a tree map
 * @param key pointer to the inclusive lower bound key
 *
 * @return pointer to the newly created tree map
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception ILLEGAL_ARGUMENT_ERROR if key is nonexistent
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_tail_map(const TreeMap* tree_map, const void* key);

/**
 * @brief Creates a shallow copy of the tree map.
 *
 * The new tree map contains the same mappings but will have independent internal storage.
 *
 * @param tree_map pointer to a tree map
 *
 * @return pointer to the newly created tree map
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_clone(const TreeMap* tree_map);

/**
 * @brief Creates a submap of the tree map within the specified range.
 *
 * @param tree_map pointer to a tree map
 * @param start_key start key (inclusive)
 * @param end_key end key (exclusive)
 *
 * @return pointer to the newly created tree map
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception ILLEGAL_ARGUMENT_ERROR if start_key or end_key are nonexistent or start_key is greater than end_key
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
TreeMap* tree_map_sub_map(const TreeMap* tree_map, const void* start_key, const void* end_key);

/**
 * @brief Converts the tree map to a string representation.
 *
 * @param tree_map pointer to a tree map
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if tree_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
StringOwned tree_map_to_string(const TreeMap* tree_map);

#endif