#include "core/sync.h"

#ifdef __linux__

#include "core/system.h"
#include "unity.h"

static Lock* lock;
static Monitor* monitor;
static Semaphore* semaphore;
static Barrier* barrier;
static int counter;

void setUp() {
    lock = lock_new();
    monitor = monitor_new();
    semaphore = semaphore_new(0);
    barrier = barrier_new(3);
    counter = 0;
}

void tearDown() {
    lock_destroy(&lock);
    monitor_destroy(&monitor);
    semaphore_destroy(&semaphore);
    barrier_destroy(&barrier);
}

void* lock_synchronized_increment(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        lock_lock(lock);
        counter++;
        lock_unlock(lock);
    }
    return nullptr;
}

void test_simple_lock() {
    // given
    intptr thread_id_1 = system_thread_create(lock_synchronized_increment);
    intptr thread_id_2 = system_thread_create(lock_synchronized_increment);
    // when
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    // then
    TEST_ASSERT_EQUAL(2'000'000, counter);
}
static int readers = 0;
static int reads = 0;

void* reader(void*) {
    lock_lock(lock, READ_LOCK);
    readers++;
    for (int i = 0; i < 1'000'000; i++) {
        reads = counter;
    }
    lock_unlock(lock, READ_LOCK);
    return nullptr;
}

void* writer(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        lock_lock(lock, WRITE_LOCK);
        counter++;
        lock_unlock(lock, WRITE_LOCK);
    }
    return nullptr;
}

void test_read_write_lock() {
    // given
    intptr thread_id_1 = system_thread_create(reader);
    intptr thread_id_2 = system_thread_create(reader);
    intptr thread_id_3 = system_thread_create(writer);
    // when
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    system_thread_join(thread_id_3);
    // then
    TEST_ASSERT_EQUAL(2, readers);
    TEST_ASSERT_EQUAL(1'000'000, counter);
}

void* monitor_synchronized_increment(void*) {
    for (int i = 0; i < 1'000'000; i++) {
        monitor_lock(monitor);
        counter++;
        monitor_unlock(monitor);
    }
    return nullptr;
}

void test_monitor_lock_unlock() {
    // given
    intptr thread_id_1 = system_thread_create(monitor_synchronized_increment);
    intptr thread_id_2 = system_thread_create(monitor_synchronized_increment);
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

void* acquire_worker(void*) {
    semaphore_acquire(semaphore);
    counter++;
    return nullptr;
}

void test_semaphore_permits() {
    // given
    semaphore_release(semaphore);
    semaphore_release(semaphore);
    // then
    TEST_ASSERT_TRUE(semaphore_try_acquire(semaphore));
    TEST_ASSERT_TRUE(semaphore_try_acquire(semaphore));
    TEST_ASSERT_FALSE(semaphore_try_acquire(semaphore));
    // when
    semaphore_release(semaphore);
    // then
    TEST_ASSERT_TRUE(semaphore_try_acquire(semaphore));
}

void test_semaphore_blocks_and_unblocks() {
    // given
    intptr thread_id = system_thread_create(acquire_worker);
    // when
    semaphore_release(semaphore);
    system_thread_join(thread_id);
    // then
    TEST_ASSERT_EQUAL(1, counter);
}

void* barrier_work(void*) {
    barrier_wait(barrier);
    counter++;
    return nullptr;
}

void test_barrier() {
    // given
    intptr thread_id_1 = system_thread_create(barrier_work);
    intptr thread_id_2 = system_thread_create(barrier_work);
    // when
    barrier_wait(barrier);
    // and
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    // then
    TEST_ASSERT_EQUAL(2, counter);
}

DEFINE_ONCE(initializer);

void initializer() {
    counter++;
}

void test_initialize_once(void) {
    // given
    initialize_once(initializer);
    // when
    initialize_once(initializer);
    initialize_once(initializer);
    initialize_once(initializer);
    // then
    TEST_ASSERT_EQUAL(1, counter);
}

int main(void) {
    RUN_TEST(test_simple_lock);
    RUN_TEST(test_read_write_lock);
    RUN_TEST(test_monitor_lock_unlock);
    RUN_TEST(test_monitor_wait_notify);
    RUN_TEST(test_semaphore_permits);
    RUN_TEST(test_semaphore_blocks_and_unblocks);
    RUN_TEST(test_barrier);
    RUN_TEST(test_initialize_once);
}

#endif