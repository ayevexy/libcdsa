#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "util/iterator.h"
#include "util/functions.h"
#include "util/collection.h"

#include <stdlib.h>
#include <stddef.h>

/**
 * An entry of the hash map, containing the key-value pair.
 * Entry objects are typically obtained when iterating over the HashMap and represent key–value pairs stored internally.
 *
 * Underlying implementation (simplified):
 * @code
 * struct Entry {
 *      void* key;
 *      void* value;
 * };
 * @endcode
 */
typedef struct Entry Entry;

/**
 * A hash map is a generic dynamic associative data structure that maps keys to values using a hashing function.
 * It automatically resizes to accommodate new entries and allows insertion of values of any type.
 *
 * Internally, the implementation uses a struct containing an array of Entry pointers (buckets). Collisions are
 * resolved by chaining entries. All operations on a hash map receive a pointer to the hash map itself as their first argument.
 * The HashMap type is opaque and can only be modified through the API.
 *
 * It must be configured using a HashMapOptions defining:
 * - its initial capacity
 * - its load factor
 * - its hash function
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
 * struct HashMap {
 *      Entry** buckets;
 *      int size;
 *      int capacity;
 * };
 * @endcode
 *
 * Error handling: Functions signal errors by exiting the program (printing to stderr),
 * or by returning an error object when wrapped with the attempt macro.
 */
typedef struct HashMap HashMap;

/**
 * HashMap configuration structure. Used to define the default behavior and attributes of a HashMap.
 *
 * @pre initial_capacity >= 10
 * @pre initial_capacity < 1'000'000'000
 * @pre load_factor >= 0.5
 * @pre hash != nullptr
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
        void (*memory_free)(void*);
    };
} HashMapOptions;

/**
 * @brief A utility macro that provides a reasonable default HashMapOptions.
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
    .memory_free = free,                                    \
    __VA_ARGS__                                             \
}

/**
 * @brief Retrieves the key stored in the provided Entry.
 *
 * @param entry pointer to an Entry
 *
 * @return pointer to the key contained in the entry
 *
 * @exception NULL_POINTER_ERROR if entry is null
 */
const void* entry_key(const Entry* entry);

/**
 * @brief Retrieves the value stored in the provided Entry.
 *
 * @param entry pointer to an Entry
 *
 * @return pointer to the value contained in the entry
 *
 * @exception NULL_POINTER_ERROR if entry is null
 */
void* entry_value(const Entry* entry);

/**
 * @brief Creates a new empty HashMap using the specified options.
 *
 * @param options pointer to an HashMapOptions defining the hash map configuration
 *
 * @return pointer to the newly created HashMap on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the HashMap fails
 */
HashMap* hash_map_new(const HashMapOptions* options);

/**
 * @brief Creates a new HashMap populated with entries from the provided Collection using the specified options.
 *
 * Each element of the collection must be a pointer to an Entry structure.
 * The entries are inserted into the new HashMap using their respective keys and values.
 *
 * @param entry_collection a Collection containing the entries to be added
 * @param options pointer to a HashMapOptions containing configuration settings
 *
 * @return pointer to the newly created HashMap on success, or nullptr if creation fails
 *
 * @exception NULL_POINTER_ERROR if options is null
 * @exception ILLEGAL_ARGUMENT_ERROR if options violates required constraints
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation for the HashMap fails or entry_collection's iterator fails
 */
HashMap* hash_map_from(Collection entry_collection, const HashMapOptions* options);

/**
 * @brief Destroys an existing HashMap and (optionally) its entries using the provided destruct functions.
 *
 * @param hash_map_pointer pointer to a HashMap pointer
 *
 * @exception NULL_POINTER_ERROR if hash_map_pointer or *hash_map_pointer is null
 *
 * @post *hash_map_pointer == nullptr
 */
void hash_map_destroy(HashMap** hash_map_pointer);

/**
 * @brief Sets the key and value destructors used by the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 * @param key_destructor function used to destroy keys
 * @param value_destructor function used to destroy values
 *
 * @exception NULL_POINTER_ERROR if hash_map or key_destructor or value_destructor are null
 */
void hash_map_set_destructors(HashMap* hash_map, void (*key_destructor)(void*), void (*value_destructor)(void*));

/**
 * @brief Computes a new value for the specified key using the provided remapping function.
 *
 * The remapper receives the key and the current value associated with the key (or nullptr if absent).
 * If the remapper returns nullptr, the entry is removed if it exists.
 * Otherwise, the returned value is stored as the new mapping.
 *
 * @param hash_map pointer to a HashMap
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
 * @param hash_map pointer to a HashMap
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
 * @param hash_map pointer to a HashMap
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
 * @param hash_map pointer to a HashMap
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
 * @brief Associates the specified value with the specified key.
 *
 * If the key already exists, the existing value is replaced and returned.
 * Otherwise, a new entry is created.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the previous value associated with the key, or nullptr if none
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand hash_map capacity on resize or creating a new entry
 */
void* hash_map_put(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Associates the specified value with the specified key only if it is absent.
 *
 * If the key already exists, the existing value is returned and no modification occurs.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 * @param value pointer to the value
 *
 * @return the existing value if present, otherwise nullptr
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand hash_map capacity on resize or creating a new entry
 */
void* hash_map_put_if_absent(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Inserts all entries from the provided entry collection into the HashMap.
 *
 * Each element of the collection must be a pointer to an Entry structure.
 * Existing mappings are replaced if keys collide.
 *
 * @param hash_map pointer to a HashMap
 * @param entry_collection collection containing Entry elements
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to expand hash_map capacity on resize or creating a new entry or entry collection's iterator fails
 */
void hash_map_put_all(HashMap* hash_map, Collection entry_collection);

/**
 * @brief Retrieves the value associated with the specified key.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 *
 * @return the associated value, or nullptr if the key is not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_get(const HashMap* hash_map, const void* key);

/**
 * @brief Retrieves the value associated with the specified key, or returns a default value.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 * @param default_value value returned if the key is not present
 *
 * @return the associated value, or default_value if absent
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_get_or_default(const HashMap* hash_map, const void* key, const void* default_value);

/**
 * @brief Replaces the value associated with the specified key.
 *
 * If the key is present, the value is replaced and the previous value is returned.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 * @param value pointer to the new value
 *
 * @return the previous value, or nullptr if the key was not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_replace(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Replaces the value associated with the specified key only if it matches the expected value.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key
 * @param old_value expected current value
 * @param value new value to store
 *
 * @return true if replacement occurred, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_replace_if_equals(HashMap* hash_map, const void* key, const void* old_value, const void* value);

/**
 * @brief Removes the entry matching the given key of the provided HashMap, (optionally) destructing it.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key to search for removal
 *
 * @return pointer to the removed entry's value, or nullptr if not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void* hash_map_remove(HashMap* hash_map, const void* key);

/**
 * @brief Removes the entry matching the given key and value of the provided HashMap, (optionally) destructing it.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key to search for removal
 * @param value pointer to the value to search for removal
 *
 * @return true if removed, false if not present
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_remove_if_equals(HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Replaces all entries using the given BiOperator of the provided HashMap, (optionally) destructing the old entries.
 *
 * @param hash_map pointer to a HashMap
 * @param bi_operator the bi-operator to replace entries
 *
 * @exception NULL_POINTER_ERROR if hash_map or bi_operator is null
 */
void hash_map_replace_all(HashMap* hash_map, BiOperator bi_operator);

/**
 * @brief Retrieves the current size of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return the current size of the provided HashMap
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
int hash_map_size(const HashMap* hash_map);

/**
 * @brief Retrieves the current capacity of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return the current capacity of the provided HashMap
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
int hash_map_capacity(const HashMap* hash_map);

/**
 * @brief Checks whether the provided HashMap is empty.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return true if empty, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_is_empty(const HashMap* hash_map);

/**
 * @brief Instantiates an Iterator for the provided HashMap.
 *
 * The iteration order of entries is unspecified and may change when the hash map resizes.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return pointer to the newly created Iterator
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if failed to allocate memory for iterator
 */
Iterator* hash_map_iterator(const HashMap* hash_map);

/**
 * @brief Checks whether two HashMap objects are equal.
 *
 * Two hash maps are considered equal if either of the following conditions is true:
 * 1. They reference the same memory location.
 * 2. They have the same size, and each corresponding entry in the first hash map is considered equal to the element
 * of the second hash map according to both key_equals and value_equals functions of the first hash map.
 *
 * @param hash_map pointer to a HashMap
 * @param other_hash_map pointer to a HashMap
 *
 * @return true if equal, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_equals(const HashMap* hash_map, const HashMap* other_hash_map);

/**
 * @brief Performs an action for each entry of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 * @param action the action to be performed
 *
 * @exception NULL_POINTER_ERROR if hash_map or action is null
 */
void hash_map_for_each(HashMap* hash_map, BiConsumer action);

/**
 * @brief Removes all entries of the provided HashMap, (optionally) destructing them.
 *
 * @param hash_map pointer to a HashMap
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
void hash_map_clear(HashMap* hash_map);

/**
 * @brief Checks whether the provided HashMap contains the specified entry.
 *
 * Comparison is performed using both key_equals and value_equals functions configured in the HashMapOptions.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key of the entry to be checked
 * @param value pointer to the value of the entry to be checked
 *
 * @return true if the entry is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_entry(const HashMap* hash_map, const void* key, const void* value);

/**
 * @brief Checks whether the provided HashMap contains the specified key.
 *
 * Comparison is performed using the key_equals function configured in the HashMapOptions.
 *
 * @param hash_map pointer to a HashMap
 * @param key pointer to the key to be checked
 *
 * @return true if the key is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_key(const HashMap* hash_map, const void* key);

/**
 * @brief Checks whether the provided HashMap contains the specified value.
 *
 * Comparison is performed using the value_equals function configured in the HashMapOptions.
 *
 * @param hash_map pointer to a HashMap
 * @param value pointer to the value to be checked
 *
 * @return true if the value is present, false otherwise
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
bool hash_map_contains_value(const HashMap* hash_map, const void* value);

/**
 * @brief Returns a Collection view of the keys of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return a Collection view containing the HashMap's keys
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_keys(const HashMap* hash_map);

/**
 * @brief Returns a Collection view of the values of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return a Collection view containing the HashMap's values
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_values(const HashMap* hash_map);

/**
 * @brief Returns a Collection view of the entries of the provided HashMap.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return a Collection view containing the HashMap's entries
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 */
Collection hash_map_entries(const HashMap* hash_map);

/**
 * @brief Creates a shallow copy of the provided HashMap.
 *
 * The new HashMap will contain the same mappings but will have independent internal storage.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return a newly created HashMap clone, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
HashMap* hash_map_clone(const HashMap* hash_map);

/**
 * @brief Converts the provided HashMap to a string representation.
 *
 * Each entry is converted using both key_to_string and value_to_string functions configured
 * in the HashMapOptions.
 *
 * @param hash_map pointer to a HashMap
 *
 * @return a newly allocated null-terminated string representation, or nullptr on failure
 *
 * @exception NULL_POINTER_ERROR if hash_map is null
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
char* hash_map_to_string(const HashMap* hash_map);

#endif