#ifndef BENCHMARK_UTILITIES_H
#define BENCHMARK_UTILITIES_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#define BENCHMARK_CAPACITY 30

#define BENCHMARK_INIT()                                                                                \
    const int benchmark_runs = BENCHMARK_RUNS;                                                          \
    double benchmark_times[BENCHMARK_CAPACITY][BENCHMARK_RUNS];                                         \
    int benchmark_times_line = 0;                                                                       \
    int benchmark_times_column = 0;                                                                     \
    printf("Running benchmarks... (It can take some time)\n\n");                                        \
    printf("%-40s %12s %20s %10s\n", "Benchmark", "Median (ms)", "Iterations", "Runs");                 \
    printf("-------------------------------------------------------------------------------------\n");  \
    for (; benchmark_times_column < benchmark_runs; benchmark_times_column++) {                         \
        benchmark_times_line = 0;

[[maybe_unused]]
static inline double benchmark_time(void) {
    struct timespec time;

#ifdef __linux__
    clock_gettime(CLOCK_MONOTONIC, &time);
#else
    timespec_get(&time, TIME_UTC);
#endif

    return (double) time.tv_sec + (double) time.tv_nsec / 1'000'000'000.0;
}

[[maybe_unused]]
static inline void format_number(unsigned long long value, char* buffer) {
    char digits[32];
    snprintf(digits, sizeof(digits), "%llu", value);

    const size_t length = strlen(digits);
    const size_t commas = (length - 1) / 3;
    const size_t output_length = length + commas;

    buffer[output_length] = '\0';
    size_t j = output_length;

    for (size_t i = length; i > 0; --i) {
        buffer[--j] = digits[i - 1];

        if (i > 1 && (length - i + 1) % 3 == 0) {
            buffer[--j] = ',';
        }
    }
}

[[maybe_unused]]
static inline double benchmark_median(double* times, int count) {
    double sorted[count];
    memcpy(sorted, times, sizeof(double) * count);

    for (int i = 1; i < count; i++) {
        const double value = sorted[i];
        int j = i - 1;

        while (j >= 0 && sorted[j] > value) {
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = value;
    }

    if (count % 2 != 0) {
        return sorted[count / 2];
    }
    return (sorted[count / 2 - 1] + sorted[count / 2]) / 2.0;
}

#define BENCHMARK(name, iterations, code) do {                                      \
    benchmark_setup();                                                              \
    const double start = benchmark_time();                                          \
    for (size_t _i = 0; _i < (iterations); _i++) {                                  \
        code;                                                                       \
    }                                                                               \
    const double elapsed = (benchmark_time() - start) * 1'000;                      \
    benchmark_times[benchmark_times_line][benchmark_times_column] = elapsed;        \
    if (benchmark_times_column == benchmark_runs - 1) {                             \
        char buffer[32];                                                            \
        format_number(iterations, buffer);                                          \
        const double median = benchmark_median(                                     \
            benchmark_times[benchmark_times_line], benchmark_runs                   \
        );                                                                          \
        printf("%-40s %12.5f %20s %10d\n", name, median, buffer, benchmark_runs);   \
    }                                                                               \
    benchmark_times_line++;                                                         \
    benchmark_teardown();                                                           \
} while (false)

#define BENCHMARK_END()                                                                                 \
    }                                                                                                   \
    printf("-------------------------------------------------------------------------------------\n");  \
    printf("Done!\n");

static volatile size_t benchmark_sink;

#define BENCHMARK_CONSUME(value) \
    (benchmark_sink = (size_t)(value))

static volatile size_t iteration_count;

[[maybe_unused]]
static void benchmark_count(void* element) {
    (void) element;
    iteration_count++;
}

#endif
