#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/**
 * @brief Enumeration of supported sorting algorithms.
 *
 * These values specify which sorting strategy should be used by
 * sorting functions (e.g., `array_list_sort`).
 *
 * The choice of algorithm may affect performance characteristics
 * such as time complexity, stability, and memory usage.
 */
typedef enum {
    BUBBLE_SORT,
    SELECTION_SORT,
    INSERTION_SORT,
    MERGE_SORT,
    QUICK_SORT
} SortingAlgorithm;

/**
 * @brief Enumeration of supported shuffling algorithms.
 *
 * These values specify which shuffling strategy should be used by
 * shuffle functions (e.g., `array_list_shuffle`).
 */
typedef enum {
    DURSTENFELD_SHUFFLE,
    SATTOLO_SHUFFLE,
    NAIVE_SHUFFLE
} ShufflingAlgorithm;

#endif