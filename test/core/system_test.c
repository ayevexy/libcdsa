#include "core/system.h"

#include "../test_utilities.h"
#include "unity.h"

static FILE* input;
static FILE* output;
static FILE* error;

void setUp() {
    input = tmpfile();
    output = tmpfile();
    error = tmpfile();

    system_input = input;
    system_output = output;
    system_error = error;
}

void tearDown() {
    fclose(input);
    fclose(output);
    fclose(error);
}

void test_system_read() {
    // given
    fprintf(input, "a");
    rewind(input);
    // when
    char c = system_read();
    // then
    TEST_ASSERT_EQUAL('a', c);
}

void test_system_read_line() {
    // given
    fprintf(input, "Hello World!\n");
    rewind(input);
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
    rewind(output);
    // then
    char buffer[256];
    fgets(buffer, 256, output);
    TEST_ASSERT_EQUAL_STRING("Hello World!H3ll0 W0rld!", buffer);
}

void test_system_write_line() {
    // when
    system_write_line("Hello World!");
    system_write_line("H%dll%d W%drld!", 3, 0, 0);
    rewind(output);
    // then
    char buffer[256];
    fgets(buffer, 256, output);
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", buffer);
    // and
    fgets(buffer, 256, output);
    TEST_ASSERT_EQUAL_STRING("H3ll0 W0rld!\n", buffer);
}

void test_system_write_error() {
    // when
    system_write_error("Hello World!");
    system_write_error("H%dll%d W%drld!", 3, 0, 0);
    rewind(error);
    // then
    char buffer[256];
    fgets(buffer, 256, error);
    TEST_ASSERT_EQUAL_STRING("Hello World!H3ll0 W0rld!", buffer);
}

void test_system_write_error_line() {
    // when
    system_write_error_line("Hello World!");
    system_write_error_line("H%dll%d W%drld!", 3, 0, 0);
    rewind(error);
    // then
    char buffer[256];
    fgets(buffer, 256, error);
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", buffer);
    // and
    fgets(buffer, 256, error);
    TEST_ASSERT_EQUAL_STRING("H3ll0 W0rld!\n", buffer);
}

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

void test_system_platform_info() {
    system_output = stdout;
    system_write_line(system_platform_name());
    system_write_line(system_platform_version());
    system_write_line(system_platform_architecture());
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_system_read);
    RUN_TEST(test_system_read_line);
    RUN_TEST(test_system_write);
    RUN_TEST(test_system_write_line);
    RUN_TEST(test_system_write_error);
    RUN_TEST(test_system_write_error_line);
    RUN_TEST(test_system_environment_variable);
    RUN_TEST(test_system_platform_info);
    return UNITY_END();
}
