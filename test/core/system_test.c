#include "core/system.h"

#include "unity.h"

static File* standard_output;

static File* input;
static File* output;
static File* error;

void setUp() {
    if (!standard_output) standard_output = system_output();

    input = file_temp();
    output = file_temp();
    error = file_temp();

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
    uintptr process_id = system_process_create("echo", "I'm a child process!");
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_NOT_EQUAL(process_id, -1);
    TEST_ASSERT_EQUAL(0, exit_code);
}

void test_system_process_is_alive() {
    // when
    uintptr process_id = system_process_create("sleep", "3");
    // then
    TEST_ASSERT_TRUE(system_process_is_alive(process_id));
    // and
    system_process_wait(process_id);
    // then
    TEST_ASSERT_FALSE(system_process_is_alive(process_id));
}

void test_system_process_wait_timeout() {
    // when
    uintptr process_id = system_process_create("sleep", "3");
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
    uintptr process_id = system_process_create("true");
    bool timed_out = false;
    int exit_code = system_process_wait_timeout(process_id, 1000, &timed_out);
    // then
    TEST_ASSERT_EQUAL(0, exit_code);
    TEST_ASSERT_FALSE(timed_out);
}

void test_system_process_exit_code() {
    // when
    uintptr process_id = system_process_create("sh", "-c", "exit 42");
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(42, exit_code);
}

void test_system_process_suspend_resume() {
    // when
    uintptr process_id = system_process_create("sleep", "3");
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
    uintptr process_id = system_process_create("sleep", "3");
    // and
    system_process_terminate(process_id);
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(-SIGTERM, exit_code);
}

void test_system_process_kill() {
    // when
    uintptr process_id = system_process_create("sleep", "3");
    // and
    system_process_kill(process_id);
    // and
    int exit_code = system_process_wait(process_id);
    // then
    TEST_ASSERT_EQUAL(-SIGKILL, exit_code);
}

#endif

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
#endif
    return UNITY_END();
}
