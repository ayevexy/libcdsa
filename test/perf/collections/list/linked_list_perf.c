#include "collections/list/linked_list.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 1'000'000
#define BENCHMARK_RUNS 11

static LinkedList* linked_list = nullptr;
static int num = 10;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;

static void benchmark_setup(void) {
    if (!instantiate) return;

    linked_list = linked_list_new(LINKED_LIST_OPTIONS(int));

    if (!populate) return;

    for (size_t i = 0; i <  ELEMENT_COUNT; i++) {
        linked_list_add_last(linked_list, &num);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    iteration_count = 0;

    if (!linked_list) return;

    linked_list_destroy(&linked_list);
}

int main(void) {
    BENCHMARK_INIT();

    BENCHMARK("linked_list_new()", 1, linked_list = linked_list_new(LINKED_LIST_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("linked_list_add_first()", ELEMENT_COUNT, linked_list_add_first(linked_list, &num));
    BENCHMARK("linked_list_add_last()", ELEMENT_COUNT, linked_list_add_last(linked_list, &num));

    populate = true;
    BENCHMARK("linked_list_remove_first()", ELEMENT_COUNT, linked_list_remove_first(linked_list));
    BENCHMARK("linked_list_remove_last()", ELEMENT_COUNT, linked_list_remove_last(linked_list));

    BENCHMARK("linked_list_contains() [found]", 1,  result += linked_list_contains(linked_list, &num));
    BENCHMARK("linked_list_contains() [missing]", 1, result += linked_list_contains(linked_list, &(int){0}));

    BENCHMARK("linked_list_get_first()", ELEMENT_COUNT, result += (bytes) linked_list_get_first(linked_list));
    BENCHMARK("linked_list_get()", 1, result += (bytes) linked_list_get(linked_list, ELEMENT_COUNT / 2));
    BENCHMARK("linked_list_get_last()", ELEMENT_COUNT, result += (bytes) linked_list_get_last(linked_list));

    BENCHMARK("for_each() {}", 1, for_each (int* element, linked_list) benchmark_count(element));
    BENCHMARK("linked_list_for_each()", 1, linked_list_for_each(linked_list, benchmark_count));

    populate = false;
    BENCHMARK("linked_list_destroy()", 1, linked_list_destroy(&linked_list));

    BENCHMARK_END();
    return 0;
}
