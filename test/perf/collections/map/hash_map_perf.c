#include "collections/map/hash_map.h"
#include "collections/map/map_entry.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 10'000'000
#define BENCHMARK_RUNS 11

typedef struct {
    int key;
    int value;
} Entry;

static HashMap* hash_map = nullptr;
static Entry entries[ELEMENT_COUNT];
static int idx = 0;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;

static void initialize_entries(void) {
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        entries[i].key = i;
        entries[i].value = i;
    }
}

static void benchmark_count_helper(void* key, void*) {
    benchmark_count(key);
}

static void benchmark_setup(void) {
    if (!instantiate) return;

    hash_map = hash_map_new(HASH_MAP_OPTIONS(int, int));

    if (!populate) return;

    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        hash_map_put(hash_map, &entries[i].key, &entries[i].value);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    idx = 0;
    iteration_count = 0;

    if (!hash_map) return;

    hash_map_destroy(&hash_map);
}

int main(void) {
    initialize_entries();
    BENCHMARK_INIT();

    BENCHMARK("hash_map_new()", 1, hash_map = hash_map_new(HASH_MAP_OPTIONS(int, int)));

    instantiate = true;
    BENCHMARK("hash_map_put()", ELEMENT_COUNT, (hash_map_put(hash_map, &entries[idx].key, &entries[idx].value), idx++));

    populate = true;
    BENCHMARK("hash_map_remove()", ELEMENT_COUNT, hash_map_remove(hash_map, &entries[idx++].key));

    BENCHMARK("hash_map_contains_key() [found]", ELEMENT_COUNT, result += hash_map_contains_key(hash_map, &(int){ELEMENT_COUNT - 1}));
    BENCHMARK("hash_map_contains_key() [missing]", ELEMENT_COUNT, result += hash_map_contains_key(hash_map, &(int){-1}));

    BENCHMARK("for_each() {}", 1, for_each (MapEntry* entry, hash_map) benchmark_count(entry));
    BENCHMARK("hash_map_for_each()", 1, hash_map_for_each(hash_map, benchmark_count_helper));

    populate = false;
    BENCHMARK("hash_map_destroy()", 1, hash_map_destroy(&hash_map));

    instantiate = populate = false;
    BENCHMARK_END();
    return 0;
}