#ifndef SORTING_ALGORITHM_H
#define SORTING_ALGORITHM_H

typedef enum SortingAlgorithm {
    BUBBLE_SORT,
    SELECTION_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
} SortingAlgorithm;

typedef enum ShufflingAlgorithm {
    DURSTENFELD_SHUFFLE,
    SATTOLO_SHUFFLE,
    NAIVE_SHUFFLE
} ShufflingAlgorithm;

#endif