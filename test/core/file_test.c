#include "core/file.h"

#include "unity.h"

static const char* file_name = "test_file.txt";

void setUp() {

}

void tearDown() {

}

void test_file_creation() {
    // when
    File* file = file_create(file_name);
    // then
    TEST_ASSERT_NOT_NULL(file);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_read_and_write_data() {
    // given
    File* file = file_create(file_name);
    uint8 input[] = { 1, 2, 3, 4, 5 };
    uint8 output[] = { 0, 0, 0, 0, 0 };
    // when
    file_write(file, input, 5);
    file_rewind(file);
    file_read(file, output, 5);
    // then
    TEST_ASSERT_EQUAL_UINT8_ARRAY(input, output, 5);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_read_and_write_char() {
    // given
    File* file = file_create(file_name);
    char self = file_write_char(file, 'a');
    file_rewind(file);
    // when
    char c = file_read_char(file);
    // then
    TEST_ASSERT_EQUAL('a', c);
    TEST_ASSERT_EQUAL('a', self);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_read_line_and_write_string() {
    // given
    File* file = file_create(file_name);
    int chars = file_write_string(file, "Hello World!\n");
    file_rewind(file);
    // when
    String string = file_read_line(file);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", string_data(string));
    TEST_ASSERT_EQUAL(chars, 13);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_exists() {
    // given
    File* file = file_create(file_name);
    file_close(file);
    // then
    TEST_ASSERT_TRUE(file_exists(file_name));
    TEST_ASSERT_FALSE(file_exists("nonexistent.txt"));
    // clean up
    file_delete(file_name);
}

void test_file_size() {
    // given
    File* file = file_create(file_name);
    file_write_string(file, "Hello World!\n");
    // when
    bytes size = file_size(file);
    // then
    TEST_ASSERT_EQUAL(13, size);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_move() {
    // given
    const char* new_file_name = "new_file.txt";
    // and
    File* file = file_create(file_name);
    file_close(file);
    // when
    file_move(file_name, new_file_name);
    // then
    TEST_ASSERT_FALSE(file_exists(file_name));
    TEST_ASSERT_TRUE(file_exists(new_file_name));
    // clean up
    file_delete(new_file_name);
}

void test_file_delete() {
    // given
    File* file = file_create(file_name);
    file_close(file);
    // when
    file_delete(file_name);
    // then
    TEST_ASSERT_FALSE(file_exists(file_name));
}

void test_file_delete_if_exists() {
    // given
    File* file = file_create(file_name);
    file_close(file);
    // then
    TEST_ASSERT_TRUE(file_delete_if_exists(file_name));
    TEST_ASSERT_FALSE(file_delete_if_exists("nonexistent.txt"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_file_creation);
    RUN_TEST(test_file_read_and_write_data);
    RUN_TEST(test_file_read_and_write_char);
    RUN_TEST(test_file_read_line_and_write_string);
    RUN_TEST(test_file_exists);
    RUN_TEST(test_file_size);
    RUN_TEST(test_file_move);
    RUN_TEST(test_file_delete);
    RUN_TEST(test_file_delete_if_exists);
    return UNITY_END();
}