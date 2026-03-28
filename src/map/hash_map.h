#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "map_entry.h"
#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * A hash map is a generic dynamic associative data structure that maps keys to values
 * using a hash function to compute the index of a key within an array.
 * It automatically resizes to accommodate new entries and allows insertion of keys and values of any type.
 *
 * Internally, the implementation uses a dynamically allocated array of `Entry*` (buckets).
 * Collisions are resolved by chaining entries in a singly linked list per bucket.
 * All operations receive a pointer to the hash map as their first argument.
 * The HashMap type is opaque and can only be modified through the public API.
 *
 * A hash map must be configured using a HashMapOptions structure specifying:
 * - the initial capacity (must be a power of two, otherwise it will be automatically rounded to the next power of two)
 * - the load factor
 * - the hash function
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
 * struct HashMap {
 *      Entry** buckets;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Memory ownership:
 * By default, the hash map does not own either its keys or values. If destruct functions are
 * provided, it will be invoked when:
 * - mappings are removed (e.g., hash_map_remove, hash_map_remove_if_equals)
 * - mappings are replaced (e.g., hash_map_put, hash_map_replace, hash_map_replace_all)
 * - the hash map is cleared (hash_map_clear)
 * - the hash map is destroyed (hash_map_destroy)
 *
 * Error handling:
 * Functions signal errors by printing to stderr and terminating the program,
 * or by returning an error object when wrapped with the attempt macro.
 *
 * Iterator invalidation:
 * Any structural modification (insertion, removal, clear, etc.) invalidates all active iterators.
 *
 * Time complexity:
 * - hash_map_put: O(1) average, O(n) worst-case
 * - hash_map_remove: O(1) average, O(n) worst-case
 * - hash_map_get: O(1) average, O(n) worst-case
 * - hash_map_replace: O(1) average, O(n) worst-case
 * - hash_map_contains_entry / contains_key: O(1) average, O(n) worst-case
 * - hash_map_contains_value: O(n)
 * 
 * Note:
 * - Average-case O(1) assumes a good hash function and low load factor.
 * - Worst-case O(n) occurs due to hash collisions (e.g., all keys in one bucket).
 */
typedef struct HashMap HashMap;

/**
 * HashMap configuration structure. Defines the behavior and attributes of a hash map.
 *
 * @pre initial_capacity >= 8
 * @pre initial_capacity <= 1'073'741'824
 * @pre load_factor >= 0.5
 * @pre hash != nullptr
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
    int initial_capacity;
    float load_factor;
    uint64_t (*hash)(const void* key);
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
} HashMapOptions;

/**
 * @brief Utility macro providing default HashMapOptions.
 *
 * @param ... optional field overrides
 */
#define DEFAULT_HASH_MAP_OPTIONS(...) &(HashMapOptions) {   \
    .initial_capacity = 16,                                 \
    .load_factor = 0.75f,                                   \
    .hash = pointer_hash,                                   \
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
 * @brief Creates a new empty hash map using the specified options.
 *
 * @param options pointer to a HashMapOptions structure
 *
 * @return pointer to a newly created hash map
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashMap* hash_map_new(const HashMapOptions* options);

/**
 * @brief Creates a new hash map containing all entries of the given collection.
 *
 * The collection must contain only `MapEntry*` elements, otherwise, the behavior is undefined.
 *
 * @param entry_collection source collection
 * @param options configuration options
 *
 * @return pointer to a newly created hash map
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation or creation of the collection iterator fails
 */
HashMap* hash_map_from(Collection entry_collection, const HashMapOptions* options);

/**
 * @brief Destroys a hash map and optionally its entries.
 *
 * @param hash_map_pointer pointer to a hash map pointer
 *
 * @exception NULL_POINTER_ERROR if hash_map_pointer or *hash_map_pointer is null
 *
 * @post *hash_map_pointer == nullptr
 */
void hash_map_destroy(HashMap** hash_map_pointer);

/**
 * @brief Sets the key destruct function.
 *
 * @param hash_map pointer to a hash map
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if hash_map or destruct is null
 */
void hash_map_set_key_destructor(HashMap* hash_map, void(*destruct)(void*));

/**
 * @brief Sets the value destruct function.
 *
 * @param hash_map pointer to a hash map
 * @param destruct new destruct function
 *
 * @exception NULL_POINTER_ERROR if hash_map or destruct is null
 */
void hash_map_set_value_destructor(HashMap* hash_map, void(*destruct)(void*));

/**
 * @brief Computes a new value for the specified key using the provided remapping function.
 *
 * The remapper receives the key and the current value associated with the key (or nullptr if absent).
 * If the remapper returns nullptr, the entry is removed if it exists.
 * Otherwise, the returned value is stored as the new mapping.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param remapper function used to compute the new value
 *
 * @return the new value associated with the key, or nullptr if removed
 *
 * @exception NULL_POINTER_ERROR if hash_map or remapper is null
 */
void* hash_map_compute(HashMap* hash_map, const void* key, BiOperator remapper);

/**
 * @brief Computes a value for the specified key only if it is not already present.
 *
 * If the key is absent, the mapper function is invoked with the key to compute a value.
 * If the computed value is not nullptr, it is inserted into the HashMap.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param mapper function used to compute the value
 *
 * @return the current or newly computed value associated with the key
 *
 * @exception NULL_POINTER_ERROR if hash_map or mapper is null
 */
void* hash_map_compute_if_absent(HashMap* hash_map, const void* key, Operator mapper);

/**
 * @brief Computes a new value for the specified key only if it is currently present.
 *
 * The remapper receives the key and the current value.
 * If the remapper returns nullptr, the entry is removed.
 * Otherwise, the returned value replaces the existing value.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param remapper function used to compute the new value
 *
 * @return the new value associated with the key, or nullptr if removed or not present
 *
 * @exception NULL_POINTER_ERROR if hash_map or remapper is null
 */
void* hash_map_compute_if_present(HashMap* hash_map, const void* key, BiOperator remapper);

/**
 * @brief Merges the provided value with the existing value associated with the key.
 *
 * If the key is not present, the value is inserted.
 * If the key is present, the remapper is invoked with the existing value and the given value.
 * If the remapper returns nullptr, the entry is removed.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the value to merge
 * @param remapper function used to merge values
 *
 * @return the new value associated with the key, or nullptr if removed
 *
 * @exception NULL_POINTER_ERROR if hash_map or remapper is null
 */
void* hash_map_merge(HashMap* hash_map, const void* key, const void* value, BiOperator remapper);

/**
 * @brief Associates the specified value with the specified key in the hash map.
 *
 * If the key already exists, the existing value is replaced and returned.
 * Otherwise, a new entry is created.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the previous value associated with the key, or nullptr if none
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new entry fails
 * 
 * @note this function calls the value destruct before returning if the key is already mapped.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* hash_map_put(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Associates the specified value with the specified key only if it is absent in the hash map.
 *
 * If the key already exists, the existing value is returned and no modification occurs.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the existing value if present, otherwise nullptr
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing or creating a new entry fails
 */
void* hash_map_put_if_absent(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Inserts all entries of a collection into the hash map.
 *
 * The collection must contain only `MapEntry*` elements, otherwise, the behavior is undefined.
 * Existing mappings are replaced if keys collide.
 *
 * @param hash_map pointer to a hash map
 * @param entry_collection collection containing Entry elements
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if resizing, creating a new entry or creation of the collection iterator fails
 * 
 * @note this function calls the value destruct before returning if keys are already mapped.
 */
void hash_map_put_all(HashMap* hash_map, Collection entry_collection);

/**
 * @brief Retrieves the value associated with the specified key of the hash map.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 *
 * @return the associated value, or nullptr if the key is not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_get(const HashMap* hash_map, const void* key);

/**
 * @brief Retrieves the value associated with the specified key of the hash map, or returns a default value.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param default_value value returned if the key is not present
 *
 * @return the associated value, or default_value if absent
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_get_or_default(const HashMap* hash_map, const void* key, const void* default_value);

/**
 * @brief Replaces the value associated with the specified key, if present, in the hash map,
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the new value
 *
 * @return the previous value, or nullptr if the key was not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * 
 * @note this function calls the value destruct before returning.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* hash_map_replace(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Replaces the value associated with the specified key of the hash map, only if it matches the expected value.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param old_value expected current value
 * @param value new value to store
 *
 * @return true if replacement occurred, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * 
 * @note this function calls the value destruct before returning.
 */
bool hash_map_replace_if_equals(HashMap* hash_map, const void* key, const void* old_value, const void* value);

/**
 * @brief Removes the entry matching the given key of the hash map.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 *
 * @return pointer to the removed entry's value, or nullptr if not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 *       If the value destruct frees the old value, the returned pointer becomes invalid.
 */
void* hash_map_remove(HashMap* hash_map, const void* key);

/**
 * @brief Removes the entry matching the given key and value of the hash map.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
bool hash_map_remove_if_equals(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Replaces all entry values using a bi-operator function.
 *
 * @param hash_map pointer to a hash map
 * @param bi_operator bi-operator function
 *
 * @exception NULL_POINTER_ERROR if hash_map or bi_operator is null
 *
 * @note this function calls the value destruct before returning.
 */
void hash_map_replace_all(HashMap* hash_map, BiOperator bi_operator);

/**
 * @brief Returns the current number of mappings in the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @return the current size
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
int hash_map_size(const HashMap* hash_map);

/**
 * @brief Returns the current capacity of the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @return the current capacity
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
int hash_map_capacity(const HashMap* hash_map);

/**
 * @brief Checks whether the hash map is empty.
 *
 * @param hash_map pointer to a hash map
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_is_empty(const HashMap* hash_map);

/**
 * @brief Creates an iterator for the hash map.
 *
 * The iteration order is not stable and may change after resizing.
 * The iterator returns objects of type `MapEntry*`.
 *
 * @param hash_map pointer to a hash map
 *
 * @return pointer to a newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the iterator does not support the following operations: has_previous(), previous(), add() and set()
 */
Iterator* hash_map_iterator(const HashMap* hash_map);

/**
 * @brief Checks whether two hash maps are equal.
 *
 * Two hash maps are equal if:
 * - they reference the same memory address, or
 * - they have the same size, and each entry in the first hash map is present in the second
 *   hash map according to both key_equals and value_equals functions of the first hash map.
 *
 * @param hash_map first hash map
 * @param other_hash_map second hash map
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if either hash_map is null
 */
bool hash_map_equals(const HashMap* hash_map, const HashMap* other_hash_map);

/**
 * @brief Applies an action to each entry of the hash map.
 *
 * @param hash_map pointer to a hash map
 * @param action function to apply
 *
 * @exception NULL_POINTER_ERROR if hash_map or action is null
 */
void hash_map_for_each(HashMap* hash_map, BiConsumer action);

/**
 * @brief Removes all entries of the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 *
 * @note this function calls both key destruct and value destruct before returning.
 */
void hash_map_clear(HashMap* hash_map);

/**
 * @brief Checks whether the hash map contains the specified entry.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return true if the entry is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_entry(const HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Checks whether the hash map contains the specified key.
 *
 * @param hash_map pointer to a hash map
 * @param key pointer to the key
 *
 * @return true if the key is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_key(const HashMap* hash_map, const void* key);

/**
 * @brief Checks whether the hash map contains the specified value.
 *
 * @param hash_map pointer to a hash map
 * @param value pointer to the value
 *
 * @return true if the value is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_value(const HashMap* hash_map, const void* value);

/**
 * @brief Returns a collection view of the entries of the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_entries(const HashMap* hash_map);

/**
 * @brief Returns a collection view of the keys of the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_keys(const HashMap* hash_map);

/**
 * @brief Returns a collection view of the values of the hash map.
 *
 * @param hash_map pointer to a hash map
 *
 * @return a collection view
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_values(const HashMap* hash_map);

/**
 * @brief Creates a shallow copy of the hash map.
 *
 * The new hash map contains the same mappings but will have independent internal storage.
 *
 * @param hash_map pointer to a hash map
 *
 * @return pointer to the newly created hash map
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashMap* hash_map_clone(const HashMap* hash_map);

/**
 * @brief Converts the hash map to a string representation.
 *
 * @param hash_map pointer to a hash map
 *
 * @return a newly allocated string
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 *
 * @note the created string must be freed manually
 */
char* hash_map_to_string(const HashMap* hash_map);

#endif