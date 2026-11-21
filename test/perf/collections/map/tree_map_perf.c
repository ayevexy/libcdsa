#include "collections/map/tree_map.h"
#include "collections/map/map_entry.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 1'000'000
#define BENCHMARK_RUNS 11

typedef struct {
    int key;
    int value;
} Entry;

static TreeMap* tree_map = nullptr;
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

    tree_map = tree_map_new(TREE_MAP_OPTIONS(int, int));

    if (!populate) return;

    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        tree_map_put(tree_map, &entries[i].key, &entries[i].value);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    idx = 0;
    iteration_count = 0;

    if (!tree_map) return;

    tree_map_destroy(&tree_map);
}

int main(void) {
    initialize_entries();
    BENCHMARK_INIT();

    BENCHMARK("tree_map_new()", 1, tree_map = tree_map_new(TREE_MAP_OPTIONS(int, int)));

    instantiate = true;
    BENCHMARK("tree_map_put()", ELEMENT_COUNT, (tree_map_put(tree_map, &entries[idx].key, &entries[idx].value), idx++));

    populate = true;
    BENCHMARK("tree_map_remove()", ELEMENT_COUNT, tree_map_remove(tree_map, &entries[idx++].key));

    BENCHMARK("tree_map_contains_key() [found]", ELEMENT_COUNT, result += tree_map_contains_key(tree_map, &(int){ELEMENT_COUNT - 1}));
    BENCHMARK("tree_map_contains_key() [missing]", ELEMENT_COUNT, result += tree_map_contains_key(tree_map, &(int){-1}));

    BENCHMARK("for_each() {}", 1, for_each (MapEntry* entry, tree_map) benchmark_count(entry));
    BENCHMARK("tree_map_for_each()", 1, tree_map_for_each(tree_map, benchmark_count_helper));

    populate = false;
    BENCHMARK("tree_map_destroy()", 1, tree_map_destroy(&tree_map));

    instantiate = populate = false;
    BENCHMARK_END();
    return 0;
}