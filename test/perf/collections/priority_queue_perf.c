#include "collections/priority_queue.h"

#include "collections/callbacks.h"
#include "collections/for_each.h"

#include "benchmark_utilities.h"

#define ELEMENT_COUNT 10'000'000
#define BENCHMARK_RUNS 11

static PriorityQueue* priority_queue = nullptr;

static int num = 10;
static bytes result = 0;

static bool instantiate = false;
static bool populate = false;

static void benchmark_setup(void) {
    if (!instantiate) return;

    priority_queue = priority_queue_new(PRIORITY_QUEUE_OPTIONS(int));

    if (!populate) return;

    for (int i = 0; i < ELEMENT_COUNT; i++) {
        priority_queue_enqueue(priority_queue, &num);
    }
}

static void benchmark_teardown(void) {
    BENCHMARK_CONSUME(result);
    BENCHMARK_CONSUME(iteration_count);

    result = 0;
    iteration_count = 0;

    if (!priority_queue) return;

    priority_queue_destroy(&priority_queue);
}

int main(void) {
    BENCHMARK_INIT();

    BENCHMARK("priority_queue_new()", 1, priority_queue = priority_queue_new(PRIORITY_QUEUE_OPTIONS(int)));

    instantiate = true;
    BENCHMARK("priority_queue_enqueue()", ELEMENT_COUNT, priority_queue_enqueue(priority_queue, &num));

    populate = true;
    BENCHMARK("priority_queue_dequeue()", ELEMENT_COUNT, priority_queue_dequeue(priority_queue));

    BENCHMARK("priority_queue_contains() [found]", 1, result += priority_queue_contains(priority_queue, &num));
    BENCHMARK("priority_queue_contains() [missing]", 1, result += priority_queue_contains(priority_queue, &(int){0}));

    BENCHMARK("priority_queue_peek()", ELEMENT_COUNT, result += (bytes) priority_queue_peek(priority_queue));

    BENCHMARK("for_each() {}", 1, for_each (int* element, priority_queue) benchmark_count(element));
    BENCHMARK("priority_queue_for_each()", 1, priority_queue_for_each(priority_queue, benchmark_count));

    populate = false;
    BENCHMARK("priority_queue_enqueue() [ascending]", ELEMENT_COUNT,
        priority_queue_enqueue(priority_queue, &(int){(int) iteration_count++})
    );
    BENCHMARK("priority_queue_enqueue() [descending]", ELEMENT_COUNT,
        priority_queue_enqueue(priority_queue, &(int){(int) (ELEMENT_COUNT - iteration_count++)})
    );

    BENCHMARK("priority_queue_destroy()", 1, priority_queue_destroy(&priority_queue));

    instantiate = populate = false;
    BENCHMARK_END();
    return 0;
}