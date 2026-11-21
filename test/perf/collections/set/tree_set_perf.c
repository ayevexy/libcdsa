#include "collections/set/tree_set.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 1'000'000
#define BENCHMARK_RUNS 11

static TreeSet* tree_set = nullptr;
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

    tree_set = tree_set_new(TREE_SET_OPTIONS(int));

    if (!populate) return;

    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        tree_set_add(tree_set, &numbers[i]);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    idx = 0;
    iteration_count = 0;

    if (!tree_set) return;

    tree_set_destroy(&tree_set);
}

int main(void) {
    initialize_numbers();
    BENCHMARK_INIT();

    BENCHMARK("tree_set_new()", 1, tree_set = tree_set_new(TREE_SET_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("tree_set_add()", ELEMENT_COUNT, tree_set_add(tree_set, &numbers[idx++]));

    populate = true;
    BENCHMARK("tree_set_remove()", ELEMENT_COUNT, tree_set_remove(tree_set, &numbers[idx++]));

    BENCHMARK("tree_set_contains() [found]", ELEMENT_COUNT, result += tree_set_contains(tree_set, &(int){ELEMENT_COUNT - 1}));
    BENCHMARK("tree_set_contains() [missing]", ELEMENT_COUNT, result += tree_set_contains(tree_set, &(int){-1}));

    BENCHMARK("for_each() {}", 1, for_each (int* element, tree_set) benchmark_count(element));
    BENCHMARK("tree_set_for_each()", 1, tree_set_for_each(tree_set, benchmark_count));

    populate = false;
    BENCHMARK("tree_set_destroy()", 1, tree_set_destroy(&tree_set));

    BENCHMARK_END();
    return 0;
}