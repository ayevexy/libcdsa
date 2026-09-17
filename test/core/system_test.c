#include "core/system.h"

#include "core/file.h"
#include "unity.h"

static File* standard_output;

static File* input;
static File* output;
static File* error;

void setUp() {
    if (!standard_output) standard_output = system_output();

    input = file_create_temporary();
    output = file_create_temporary();
    error = file_create_temporary();

    system_change_input(input);
    system_change_output(output);
    system_change_error(error);
}

void tearDown() {
    file_close(input);
    file_close(output);
    file_close(error);
}

void test_system_read() {
    // given
    file_write_char(input, 'a');
    file_rewind(input);
    // when
    char c = system_read();
    // then
    TEST_ASSERT_EQUAL('a', c);
}

void test_system_read_line() {
    // given
    file_write_string(input, "Hello World!\n");
    file_rewind(input);
    // when
    String string = system_read_line();
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_system_write() {
    // when
    system_write("Hello World!");
    system_write("H%dll%d W%drld!", 3, 0, 0);
    file_rewind(output);
    // then
    String string = file_read_line(output);
    TEST_ASSERT_EQUAL_STRING("Hello World!H3ll0 W0rld!", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_system_write_line() {
    // when
    system_write_line("Hello World!");
    system_write_line("H%dll%d W%drld!", 3, 0, 0);
    file_rewind(output);
    // then
    String string = file_read_line(output);
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", string_data(string));
    string_destroy(&string);
    // and
    string = file_read_line(output);
    TEST_ASSERT_EQUAL_STRING("H3ll0 W0rld!\n", string_data(string));
    string_destroy(&string);
}

void test_system_write_error() {
    // when
    system_write_error("Hello World!");
    system_write_error("H%dll%d W%drld!", 3, 0, 0);
    file_rewind(error);
    // then
    String string = file_read_line(error);
    TEST_ASSERT_EQUAL_STRING("Hello World!H3ll0 W0rld!", string_data(string));
    // clean up
    string_destroy(&string);
}

void test_system_write_error_line() {
    // when
    system_write_error_line("Hello World!");
    system_write_error_line("H%dll%d W%drld!", 3, 0, 0);
    file_rewind(error);
    // then
    String string = file_read_line(error);
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", string_data(string));
    string_destroy(&string);
    // and
    string = file_read_line(error);
    TEST_ASSERT_EQUAL_STRING("H3ll0 W0rld!\n", string_data(string));
    string_destroy(&string);
}

#ifdef __linux

void test_system_environment_variable() {
    // given
    system_set_environment_variable("LIBCDSA_TEST_ENV", "0");
    // when
    const char* env = system_get_environment_variable("LIBCDSA_TEST_ENV");
    // then
    TEST_ASSERT_EQUAL_STRING("0", env);
    // and
    system_remove_environment_variable("LIBCDSA_TEST_ENV");
    TEST_ASSERT_NULL(system_get_environment_variable("LIBCDSA_TEST_ENV"));
}

#endif

void test_system_platform_info() {
    system_change_output(standard_output);
    system_write_line(system_platform_name());
    system_write_line(system_platform_version());
    system_write_line(system_platform_architecture());
    TEST_PASS();
}

#ifdef __linux

#include <signal.h>

void test_system_process_creation() {
    // when
    intptr process_id = system_process_create("echo", "I'm a child process!");
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_NOT_EQUAL(process_id, -1);
    TEST_ASSERT_EQUAL(0, exit_code);
}

void test_system_process_is_alive() {
    // when
    intptr process_id = system_process_create("sleep", "3");
    // then
    TEST_ASSERT_TRUE(system_process_is_alive(process_id));
    // and
    system_process_wait(process_id);
    // then
    TEST_ASSERT_FALSE(system_process_is_alive(process_id));
}

void test_system_process_wait_timeout() {
    // when
    intptr process_id = system_process_create("sleep", "3");
    bool timed_out = false;
    int exit_code = system_process_wait_timeout(process_id, 10, &timed_out);
    // then
    TEST_ASSERT_EQUAL(-1, exit_code);
    TEST_ASSERT_TRUE(timed_out);
    // cleanup
    system_process_kill(process_id);
    system_process_wait(process_id);
}

void test_system_process_wait_timeout_completed() {
    // when
    intptr process_id = system_process_create("true");
    bool timed_out = false;
    int exit_code = system_process_wait_timeout(process_id, 1000, &timed_out);
    // then
    TEST_ASSERT_EQUAL(0, exit_code);
    TEST_ASSERT_FALSE(timed_out);
}

void test_system_process_exit_code() {
    // when
    intptr process_id = system_process_create("sh", "-c", "exit 42");
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(42, exit_code);
}

void test_system_process_suspend_resume() {
    // when
    intptr process_id = system_process_create("sleep", "3");
    // and
    system_process_suspend(process_id);
    // then
    TEST_ASSERT_TRUE(system_process_is_alive(process_id));
    // when
    system_process_resume(process_id);
    // then
    TEST_ASSERT_TRUE(system_process_is_alive(process_id));
    // cleanup
    system_process_kill(process_id);
    system_process_wait(process_id);
}

void test_system_process_terminate() {
    // when
    intptr process_id = system_process_create("sleep", "3");
    // and
    system_process_terminate(process_id);
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(-SIGTERM, exit_code);
}

void test_system_process_kill() {
    // when
    intptr process_id = system_process_create("sleep", "3");
    // and
    system_process_kill(process_id);
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(-SIGKILL, exit_code);
}

void* count_to_ten(void* raw_counter) {
    int* counter = raw_counter;
    while (*counter < 10) {
        (*counter)++;
    }
    return counter;
}

void test_system_thread_creation() {
    // given
    int counter = 0;
    // when
    intptr thread_id = system_thread_create(count_to_ten, &counter);
    // and
    int* result = system_thread_join(thread_id);
    // then
    TEST_ASSERT_EQUAL(10, counter);
    TEST_ASSERT_EQUAL(10, *result);
    TEST_ASSERT_EQUAL_PTR(result, &counter);
}

#include <stdatomic.h>

void* increment(void* raw_counter) {
    atomic_int* counter = raw_counter;
    atomic_fetch_add(counter, 1);
    return counter;
}

void test_system_thread_creation_multiple() {
    // given
    atomic_int counter = 0;
    // when
    intptr thread_id_1 = system_thread_create(increment, &counter);
    intptr thread_id_2 = system_thread_create(increment, &counter);
    intptr thread_id_3 = system_thread_create(increment, &counter);
    // and
    system_thread_join(thread_id_1);
    system_thread_join(thread_id_2);
    system_thread_join(thread_id_3);
    // then
    TEST_ASSERT_EQUAL(3, counter);
}

void* count_to_ten_seconds(void* raw_counter) {
    int* counter = raw_counter;
    while (*counter < 10) {
        system_thread_sleep(1000);
        (*counter)++;
    }
    return counter;
}

void test_system_thread_interrupt() {
    // given
    int counter = 0;
    // when
    intptr thread_id = system_thread_create(count_to_ten_seconds, &counter);
    system_thread_sleep(100);
    // and
    system_thread_interrupt(thread_id);
    system_thread_join(thread_id);
    // then
    TEST_ASSERT_EQUAL(0, counter);
}

#endif

void test_system_execute() {
    // when
    int exit_code = system_execute("echo \"Hello World!\"");
    // then
    TEST_ASSERT_EQUAL(0, exit_code);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_system_read);
    RUN_TEST(test_system_read_line);
    RUN_TEST(test_system_write);
    RUN_TEST(test_system_write_line);
    RUN_TEST(test_system_write_error);
    RUN_TEST(test_system_write_error_line);
#ifdef __linux__
    RUN_TEST(test_system_environment_variable);
#endif
    RUN_TEST(test_system_platform_info);
#ifdef __linux
    RUN_TEST(test_system_process_creation);
    RUN_TEST(test_system_process_is_alive);
    RUN_TEST(test_system_process_wait_timeout);
    RUN_TEST(test_system_process_wait_timeout_completed);
    RUN_TEST(test_system_process_exit_code);
    RUN_TEST(test_system_process_suspend_resume);
    RUN_TEST(test_system_process_terminate);
    RUN_TEST(test_system_process_kill);
    RUN_TEST(test_system_thread_creation);
    RUN_TEST(test_system_thread_creation_multiple);
    RUN_TEST(test_system_thread_interrupt);
#endif
    RUN_TEST(test_system_execute);
    return UNITY_END();
}
