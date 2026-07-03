#ifndef LIBCDSA_STREAM_H
#define LIBCDSA_STREAM_H

#include "core/array.h"
#include "algorithms.h"
#include "functions.h"
#include "collection.h"
#include "optional.h"

typedef struct Sequence Sequence;

Sequence* sequence_from(Collection collection);

void sequence_filter(Sequence* sequence, Predicate predicate);

void sequence_map(Sequence* sequence, Operator operator);

void sequence_peek(Sequence* sequence, Consumer action);

void sequence_limit(Sequence* sequence, int max_size);

void sequence_skip(Sequence* sequence, int count);

void sequence_take_while(Sequence* sequence, Predicate predicate);

void sequence_drop_while(Sequence* sequence, Predicate predicate);

void sequence_distinct(Sequence* sequence, bool (*equals)(const void*, const void*));

void sequence_sort(Sequence* sequence, Comparator comparator, SortingAlgorithm algorithm);

void sequence_for_each(Sequence* sequence, Consumer action);

void* sequence_reduce(Sequence* sequence, void* identity, BiOperator accumulator);

Optional sequence_find(Sequence* sequence);

Optional sequence_max(Sequence* sequence, Comparator comparator);

Optional sequence_min(Sequence* sequence, Comparator comparator);

bool sequence_all_match(Sequence* sequence, Predicate predicate);

bool sequence_any_match(Sequence* sequence, Predicate predicate);

bool sequence_none_match(Sequence* sequence, Predicate predicate);

int sequence_count(Sequence* sequence);

Array(void*) sequence_to_array(Sequence* sequence);

#endif