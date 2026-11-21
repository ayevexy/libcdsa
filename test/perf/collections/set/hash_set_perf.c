#include "collections/set/hash_set.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 10'000'000
#define BENCHMARK_RUNS 11

static HashSet* hash_set = nullptr;
static int numbers[ELEMENT_COUNT];
static int idx = 0;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;

static void initialize_numbers(void) {
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        numbers[i] = i;
    }
}

static void benchmark_setup(void) {
    if (!instantiate) return;

    hash_set = hash_set_new(HASH_SET_OPTIONS(int));

    if (!populate) return;

    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        hash_set_add(hash_set, &numbers[i]);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    idx = 0;
    iteration_count = 0;

    if (!hash_set) return;

    hash_set_destroy(&hash_set);
}

int main(void) {
    initialize_numbers();
    BENCHMARK_INIT();

    BENCHMARK("hash_set_new()", 1, hash_set = hash_set_new(HASH_SET_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("hash_set_add()", ELEMENT_COUNT, hash_set_add(hash_set, &numbers[idx++]));

    populate = true;
    BENCHMARK("hash_set_remove()", ELEMENT_COUNT, hash_set_remove(hash_set, &numbers[idx++]));

    BENCHMARK("hash_set_contains() [found]", ELEMENT_COUNT, result += hash_set_contains(hash_set, &(int){ELEMENT_COUNT - 1}));
    BENCHMARK("hash_set_contains() [missing]", ELEMENT_COUNT, result += hash_set_contains(hash_set, &(int){-1}));

    BENCHMARK("for_each() {}", 1, for_each (int* element, hash_set) benchmark_count(element));
    BENCHMARK("hash_set_for_each()", 1, hash_set_for_each(hash_set, benchmark_count));

    populate = false;
    BENCHMARK("hash_set_destroy()", 1, hash_set_destroy(&hash_set));

    instantiate = populate = false;
    BENCHMARK_END();
    return 0;
}