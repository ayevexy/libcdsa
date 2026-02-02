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
    /** Bubble sort (simple, stable, O(n²)) */
    BUBBLE_SORT,

    /** Selection sort (simple, unstable, O(n²)) */
    SELECTION_SORT,

    /** Insertion sort (stable, efficient for small or nearly sorted data) */
    INSERTION_SORT,

    /** Merge sort (stable, O(n log n), requires additional memory) */
    MERGE_SORT,

    /** Quick sort (unstable, O(n log n) average, in-place) */
    QUICK_SORT
} SortingAlgorithm;

/**
 * @brief Enumeration of supported shuffling algorithms.
 *
 * These values specify which shuffling strategy should be used by
 * shuffle functions (e.g., `array_list_shuffle`).
 */
typedef enum {
    /** Durstenfeld shuffle (modern Fisher–Yates, unbiased) */
    DURSTENFELD_SHUFFLE,

    /** Sattolo's algorithm (produces a single-cycle permutation) */
    SATTOLO_SHUFFLE,

    /** Naive shuffle (simple but statistically biased) */
    NAIVE_SHUFFLE
} ShufflingAlgorithm;

#endif