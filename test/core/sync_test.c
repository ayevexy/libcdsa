#include "core/sync.h"

#ifdef __linux__

#include "core/system.h"
#include "unity.h"

static Monitor* monitor;
static int counter;

void setUp() {
    monitor = monitor_new();
    counter = 0;
}

void tearDown() {
    monitor_destroy(&monitor);
}

void* synchronized_increment(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        monitor_lock(monitor);
        counter++;
        monitor_unlock(monitor);
    }
    return nullptr;
}

void test_monitor_lock_unlock() {
    // given
    intptr thread_id_1 = system_thread_create(synchronized_increment);
    intptr thread_id_2 = system_thread_create(synchronized_increment);
    // when
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    // then
    TEST_ASSERT_EQUAL(2'000'000, counter);
}

void* notifying_producer(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        monitor_lock(monitor);
        counter++;
        monitor_notify(monitor);
        monitor_unlock(monitor);
    }
    return nullptr;
}

void* waiting_consumer(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        monitor_lock(monitor);
        while (counter == 0) {
            monitor_wait(monitor);
        }
        counter--;
        monitor_unlock(monitor);
    }
    return nullptr;
}

void test_monitor_wait_notify() {
    // given
    intptr thread_id_1 = system_thread_create(notifying_producer);
    intptr thread_id_2 = system_thread_create(waiting_consumer);
    // when
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    // then
    TEST_ASSERT_EQUAL(0, counter);
}

int main(void) {
    RUN_TEST(test_monitor_lock_unlock);
    RUN_TEST(test_monitor_wait_notify);
}

#endif