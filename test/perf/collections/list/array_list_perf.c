#include "collections/list/array_list.h"

#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 10'000'000
#define ELEMENT_COUNT_SLOW 10'000
#define BENCHMARK_RUNS 11

static ArrayList* array_list = nullptr;

static int num = 10;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;
static bool populate_slow = false;

static void benchmark_setup(void) {
    if (!instantiate) return;

    array_list = array_list_new(ARRAY_LIST_OPTIONS(int));

    if (!populate) return;

    const int max = populate_slow ? ELEMENT_COUNT_SLOW : ELEMENT_COUNT;

    for (int i = 0; i < max; i++) {
        array_list_add_last(array_list, &num);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    iteration_count = 0;

    if (!array_list) return;

    array_list_destroy(&array_list);
}

int main(void) {
    BENCHMARK_INIT();

    BENCHMARK("array_list_new()", 1, array_list = array_list_new(ARRAY_LIST_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("array_list_add_first()", ELEMENT_COUNT_SLOW, array_list_add_first(array_list, &num));
    BENCHMARK("array_list_add_last()", ELEMENT_COUNT, array_list_add_last(array_list, &num));

    populate = true;
    populate_slow = true;
    BENCHMARK("array_list_remove_first()", ELEMENT_COUNT_SLOW, array_list_remove_first(array_list));

    populate_slow = false;
    BENCHMARK("array_list_remove_last()", ELEMENT_COUNT, array_list_remove_last(array_list));

    BENCHMARK("array_list_contains() [found]", 1, result += array_list_contains(array_list, &num));
    BENCHMARK("array_list_contains() [missing]", 1, result += array_list_contains(array_list, &(int){0}));

    BENCHMARK("array_list_get_first()", ELEMENT_COUNT, result += (bytes) array_list_get_first(array_list));
    BENCHMARK("array_list_get()", ELEMENT_COUNT_SLOW, result += (bytes) array_list_get(array_list, ELEMENT_COUNT / 2));
    BENCHMARK("array_list_get_last()", ELEMENT_COUNT, result += (bytes) array_list_get_last(array_list));

    BENCHMARK("for_each() {}", 1, for_each (int* element, array_list) benchmark_count(element));
    BENCHMARK("array_list_for_each()", 1, array_list_for_each(array_list, benchmark_count));

    populate = false;
    BENCHMARK("array_list_destroy()", 1, array_list_destroy(&array_list));

    instantiate = populate = populate_slow = false;
    BENCHMARK_END();
    return 0;
}