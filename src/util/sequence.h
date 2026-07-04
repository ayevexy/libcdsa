#ifndef LIBCDSA_SEQUENCE_H
#define LIBCDSA_SEQUENCE_H

#include "core/array.h"
#include "algorithms.h"
#include "functions.h"
#include "collection.h"
#include "optional.h"

/**
 * A sequence is an abstraction over an existing collection that allows generic
 * algorithms to operate on it. A sequence consists of a source (the collection)
 * and a pipeline (the algorithms).
 *
 * The sequence pipeline is constructed by appending intermediate operations
 * (map, filter, peek) and is executed by calling a terminal operation
 * (for_each, count, to_array). After a terminal operation is called, the
 * sequence becomes invalid.
 *
 * A sequence does not copy the elements of the original collection; it simply
 * iterates over them.
 *
 * Most sequence operations are stateless and require no additional memory.
 * The exceptions are distinct and sort, which allocate internal buffers.
 *
 * A sequence pipeline supports up to 15 chained intermediate operations,
 * including at most 5 sort operations.
 */
typedef struct Sequence Sequence;

/**
 * @brief Error code indicating a failure during sequence pipeline processing.
 */
constexpr int SEQUENCE_PROCESSING_ERROR = -2;

/**
 * @brief Creates a sequence from an existing collection.
 *
 * @param collection the collection
 *
 * @return a newly created sequence
 *
 * @exception MEMORY_ALLOCATION_ERROR if memory allocation fails
 */
Sequence* sequence_from(Collection collection);

/**
 * @brief Appends a 'filter' operation to the sequence pipeline.
 *
 * The filter operation retains elements of the sequence that satisfy the given predicate.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_filter(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends a 'map' operation to the sequence pipeline.
 *
 * The map operation transforms elements of the sequence using the operator function.
 *
 * @param sequence the sequence
 * @param operator the operator
 *
 * @exception NULL_POINTER_ERROR if sequence or operator is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_map(Sequence* sequence, Operator operator);

/**
 * @brief Appends a 'peek' operation to the sequence pipeline.
 *
 * The peek operation applies a consumer function to each element of the sequence.
 *
 * @param sequence the sequence
 * @param action the action
 *
 * @exception NULL_POINTER_ERROR if sequence or action is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_peek(Sequence* sequence, Consumer action);

/**
 * @brief Appends a 'limit' operation to the sequence pipeline.
 *
 * The limit operation retains at most n elements of the sequence.
 *
 * @param sequence the sequence
 * @param max_size number of elements to limit
 *
 * @exception NULL_POINTER_ERROR if sequence is null
 * @exception ILLEGAL_ARGUMENT_ERROR if max_size is negative
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_limit(Sequence* sequence, int max_size);

/**
 * @brief Appends a 'skip' operation to the sequence pipeline.
 *
 * The skip operation drops the first n elements of the sequence.
 *
 * @param sequence the sequence
 * @param count number of elements to skip
 *
 * @exception NULL_POINTER_ERROR if sequence is null
 * @exception ILLEGAL_ARGUMENT_ERROR if count is negative
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_skip(Sequence* sequence, int count);

/**
 * @brief Appends a 'take while' operation to the sequence pipeline.
 *
 * The take-while operation retains elements of the sequence while the predicate is true.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_take_while(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends a 'drop while' operation to the sequence pipeline.
 *
 * The drop-while operation skips elements of the sequence while the predicate is true.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 *
 * @note this is an intermediate stateless operation.
 */
void sequence_drop_while(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends a 'distinct' operation to the sequence pipeline.
 *
 * The distinct operation removes duplicate elements from the sequence.
 *
 * @param sequence the sequence
 * @param equals the equality function used to compare elements
 *
 * @exception NULL_POINTER_ERROR if sequence or equals is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 * @exception MEMORY_ALLOCATION_ERROR if allocation of the underlying hash set fails
 *
 * @note this is an intermediate stateful operation.
 */
void sequence_distinct(Sequence* sequence, bool (*equals)(const void*, const void*));

/**
 * @brief Appends a 'sort' operation to the sequence pipeline.
 *
 * The sort operation sorts the elements of the sequence.
 *
 * @param sequence the sequence
 * @param comparator the comparator function
 * @param algorithm the sorting algorithm
 *
 * @exception NULL_POINTER_ERROR if sequence or comparator is null
 * @exception ILLEGAL_STATE_ERROR if appending this operation exceeds the limit
 * @exception MEMORY_ALLOCATION_ERROR if allocation of the underlying array list fails
 *
 * @note this is an intermediate stateful operation.
 */
void sequence_sort(Sequence* sequence, Comparator comparator, SortingAlgorithm algorithm);

/**
 * @brief Appends the 'for each' operation and processes the sequence pipeline.
 *
 * The for-each operation applies a consumer function to each element of the sequence.
 *
 * @param sequence the sequence
 * @param action the action
 *
 * @exception NULL_POINTER_ERROR if sequence or action is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
void sequence_for_each(Sequence* sequence, Consumer action);

/**
 * @brief Appends the 'reduce' operation and processes the sequence pipeline.
 *
 * The reduce operation applies an accumulator to the elements of the sequence and returns the reduced value.
 *
 * @param sequence the sequence
 * @param identity initial value
 * @param accumulator accumulator function
 *
 * @return the result
 *
 * @exception NULL_POINTER_ERROR if sequence or accumulator is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note the returned pointer may refer to the identity argument, which may be modified.
 * @note this is a terminal operation.
 */
void* sequence_reduce(Sequence* sequence, void* identity, BiOperator accumulator);

/**
 * @brief Appends the 'find' operation and processes the sequence pipeline.
 *
 * The find operation returns the first element of the sequence.
 *
 * @param sequence the sequence
 *
 * @return an optional that may contain the first element
 *
 * @exception NULL_POINTER_ERROR if sequence is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
Optional sequence_find(Sequence* sequence);

/**
 * @brief Appends the 'max' operation and processes the sequence pipeline.
 *
 * The max operation returns the greatest element of the sequence according to the given comparator.
 *
 * @param sequence the sequence
 * @param comparator comparator function
 *
 * @return an optional that may contain the maximum element
 *
 * @exception NULL_POINTER_ERROR if sequence or comparator is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
Optional sequence_max(Sequence* sequence, Comparator comparator);

/**
 * @brief Appends the 'min' operation and processes the sequence pipeline.
 *
 * The min operation returns the smallest element of the sequence according to the given comparator.
 *
 * @param sequence the sequence
 * @param comparator comparator function
 *
 * @return an optional that may contain the minimum element
 *
 * @exception NULL_POINTER_ERROR if sequence or comparator is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
Optional sequence_min(Sequence* sequence, Comparator comparator);

/**
 * @brief Appends the 'all match' operation and processes the sequence pipeline.
 *
 * The all-match operation tests whether all elements of the sequence match the given predicate.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @return true if all elements match the predicate, false otherwise
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
bool sequence_all_match(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends the 'any match' operation and processes the sequence pipeline.
 *
 * The any-match operation tests whether any element of the sequence matches the given predicate.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @return true if any element matches the predicate, false otherwise
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
bool sequence_any_match(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends the 'none match' operation and processes the sequence pipeline.
 *
 * The none-match operation tests whether no elements in the sequence match the given predicate.
 *
 * @param sequence the sequence
 * @param predicate the predicate
 *
 * @return true if no elements match the predicate, false otherwise
 *
 * @exception NULL_POINTER_ERROR if sequence or predicate is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
bool sequence_none_match(Sequence* sequence, Predicate predicate);

/**
 * @brief Appends the 'count' operation and processes the sequence pipeline.
 *
 * The count operation returns the number of elements in the sequence.
 *
 * @param sequence the sequence
 *
 * @return the number of elements in the sequence
 *
 * @exception NULL_POINTER_ERROR if sequence is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
int sequence_count(Sequence* sequence);

/**
 * @brief Appends the 'to array' operation and processes the sequence pipeline.
 *
 * The to-array operation converts the elements of the sequence into an array.
 *
 * @param sequence the sequence
 *
 * @return an array containing the current elements of the sequence
 *
 * @exception NULL_POINTER_ERROR if sequence is null
 * @exception SEQUENCE_PROCESSING_ERROR if an error occurs during pipeline processing
 *
 * @note this is a terminal operation.
 */
Array(void*) sequence_to_array(Sequence* sequence);

#endif