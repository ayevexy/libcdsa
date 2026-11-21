#include "collections/deque.h"
#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 10'000'000
#define BENCHMARK_RUNS 11

static Deque* deque = nullptr;
static int num = 10;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;

static void benchmark_setup(void) {
    if (!instantiate) return;

    deque = deque_new(DEQUE_OPTIONS(int));

    if (!populate) return;

    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        deque_add_last(deque, &num);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    iteration_count = 0;

    if (!deque) return;

    deque_destroy(&deque);
}

int main(void) {
    BENCHMARK_INIT();

    BENCHMARK("deque_new()", 1, deque = deque_new(DEQUE_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("deque_add_first()", ELEMENT_COUNT, deque_add_first(deque, &num));
    BENCHMARK("deque_add_last()", ELEMENT_COUNT, deque_add_last(deque, &num));

    populate = true;
    BENCHMARK("deque_remove_first()", ELEMENT_COUNT, deque_remove_first(deque));
    BENCHMARK("deque_remove_last()", ELEMENT_COUNT, deque_remove_last(deque));

    BENCHMARK("deque_contains() [found]", 1, result += deque_contains(deque, &num));
    BENCHMARK("deque_contains() [missing]", 1, result += deque_contains(deque, &(int){0}));

    BENCHMARK("deque_get_first()", ELEMENT_COUNT, result += (bytes) deque_get_first(deque));
    BENCHMARK("deque_get_last()", ELEMENT_COUNT, result += (bytes) deque_get_last(deque));

    BENCHMARK("for_each() {}", 1, for_each (int* element, deque) benchmark_count(element));
    BENCHMARK("deque_for_each()", 1, deque_for_each(deque, benchmark_count));

    populate = false;
    BENCHMARK("deque_destroy()", 1, deque_destroy(&deque));

    instantiate = populate = false;
    BENCHMARK_END();
    return 0;
}