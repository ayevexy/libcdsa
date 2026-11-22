#include "util/logging.h"

#include <string.h>
#include <stdio.h>

#include "unity.h"

#undef LOG_MODULE
#define LOG_MODULE "util.logging_test"

void setUp(void) {
    const bool success = logger_initialize((LoggerOutput) {
        .console = true,
        .file_name = "log_file.txt"
    });
    TEST_ASSERT_TRUE(success);

    logger_change_level(LOG_ALL);
    *logger_options = (LoggerOptions) {};
}

void tearDown(void) {
    fclose(fopen("log_file.txt", "w"));

    logger_finalize();
}

static const char* read_log_output(void) {
    static char buffer[4096];

    FILE* file = fopen("log_file.txt", "r");

    const int length = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[length] = '\0';

    fclose(file);

    return length > 0 ? buffer : nullptr;
}

static void test_logger_log_timestamp(void) {
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

static void test_logger_log_level(void) {
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

static void test_logger_log_module(void) {
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

static void test_logger_log_location(void) {
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

static void test_logger_log_message(void) {
    // when
    logger_info("Hello %s", "World !");
    logger_flush();
    // then
    const char* message = read_log_output();
    // and
    TEST_ASSERT_NOT_NULL(message);
}

static void test_logger_log(void) {
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

static void test_logger_log_filtered(void) {
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

static void test_logger_off(void) {
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

static void test_logger_log_pretty(void) {
    // given
    logger_finalize();
    logger_initialize((LoggerOutput) {
        .console = true,
        .file_name = "log_file.txt"
    });
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

static void test_logger_module_disabled(void) {
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