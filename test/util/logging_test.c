#include "util/logging.h"

#include <string.h>
#include <stdio.h>

#include "unity.h"

#undef LOG_MODULE
#define LOG_MODULE "util.logging_test"

void setUp() {
    const bool success = logger_initialize("log_file.txt");
    TEST_ASSERT_TRUE(success);

    logger_change_level(LOG_ALL);
    *logger_options = (LoggerOptions) {};
}

void tearDown() {
    fclose(fopen("log_file.txt", "w"));

    logger_finalize();
}

static const char* read_log_output() {
    static char buffer[4096];

    FILE* file = fopen("log_file.txt", "r");

    const int length = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[length] = '\0';

    fclose(file);

    return length > 0 ? buffer : nullptr;
}

void test_logger_log_timestamp() {
    // given
    logger_options->timestamp = true;
    // when
    logger_info("");
    logger_flush();
    // then
    const char* timestamp = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(timestamp);
}

void test_logger_log_level() {
    // given
    logger_options->level = true;
    // when
    logger_info("");
    logger_flush();
    // then
    const char* level = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(level);
}

void test_logger_log_module() {
    // given
    logger_options->module = true;
    // when
    logger_info("");
    logger_flush();
    // then
    const char* module = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(module);
}

void test_logger_log_location() {
    // given
    logger_options->location = true;
    // when
    logger_info("");
    logger_flush();
    // then
    const char* location = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(location);
}

void test_logger_log_message() {
    // when
    logger_info("Hello %s", "World !");
    logger_flush();
    // then
    const char* message = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(message);
}

void test_logger_log() {
    // given
    *logger_options = (LoggerOptions) {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true
    };
    // when
    logger_info("Hello %s", "World !");
    logger_flush();
    // then
    const char* log = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(log);
}

void test_logger_log_filtered(void) {
    // given
    logger_change_level(LOG_INFO);
    // and
    *logger_options = (LoggerOptions) {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true
    };
    // when
    logger_trace("Entering function...");
    logger_debug("Variable value: %d", 42);
    logger_info("Nothing happened...");
    logger_warn("Something unexpected happened...");
    logger_error("An operation failed...");
    logger_fatal("Unrecoverable error occurred...");
    logger_flush();
    // then
    const char* logs = read_log_output();
    // and
    TEST_ASSERT_NULL(strstr(logs, "TRACE"));
    TEST_ASSERT_NULL(strstr(logs, "DEBUG"));
    TEST_ASSERT_NOT_NULL(strstr(logs, "INFO"));
    TEST_ASSERT_NOT_NULL(strstr(logs, "WARN"));
    TEST_ASSERT_NOT_NULL(strstr(logs, "ERROR"));
    TEST_ASSERT_NOT_NULL(strstr(logs, "FATAL"));
}

void test_logger_off() {
    // given
    logger_change_level(LOG_OFF);
    // and
    *logger_options = (LoggerOptions) {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true
    };
    // when
    logger_fatal("Should not appear");
    // then
    const char* log = read_log_output();
    // and
    TEST_ASSERT_NULL(log);
}

void test_logger_log_pretty() {
    // given
    logger_finalize();
    logger_initialize(nullptr);
    // and
    *logger_options = (LoggerOptions) {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true,
        .colors = true,
        .padding = true
    };
    // when
    logger_trace("Entering function...");
    logger_debug("Variable value: %d", 42);
    logger_info("Nothing happened...");
    logger_warn("Something unexpected happened...");
    logger_error("An operation failed...");
    logger_fatal("Unrecoverable error occurred...");
    logger_flush();
    // clean up
    logger_finalize();
}

void test_logger_module_disabled() {
    // given
    #undef LOG_MODULE
    #define LOG_MODULE "disabled"
    // and
    *logger_options = (LoggerOptions) {
        .timestamp = true,
        .level = true,
        .module = true,
        .location = true
    };
    // when
    logger_trace("Entering function...");
    logger_debug("Variable value: %d", 42);
    logger_info("Nothing happened...");
    logger_warn("Something unexpected happened...");
    logger_error("An operation failed...");
    logger_fatal("Unrecoverable error occurred...");
    logger_flush();
    // then
    const char* log = read_log_output();
    // and
    TEST_ASSERT_NULL(log);
    // clean up
    #undef LOG_MODULE
    #define LOG_MODULE "test.logging"
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_logger_log_timestamp);
    RUN_TEST(test_logger_log_level);
    RUN_TEST(test_logger_log_module);
    RUN_TEST(test_logger_log_location);
    RUN_TEST(test_logger_log_message);
    RUN_TEST(test_logger_log);
    RUN_TEST(test_logger_log_filtered);
    RUN_TEST(test_logger_off);
    RUN_TEST(test_logger_log_pretty);
    RUN_TEST(test_logger_module_disabled);
    return UNITY_END();
}