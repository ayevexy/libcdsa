#include "core/file.h"

#include "unity.h"

static const char* file_name = "test_file.txt";

void setUp() {
    file_delete_if_exists(file_name);
    file_delete_if_exists("test_dir/file_1.txt");
    file_delete_if_exists("test_dir/file_2.txt");
    file_delete_if_exists("test_dir");
    file_delete_if_exists("source.txt");
    file_delete_if_exists("destination.txt");
}

void tearDown() {

}

void test_file_create() {
    // when
    File* file = file_create(file_name);
    // then
    TEST_ASSERT_NOT_NULL(file);
    // clean up
    file_close(file);
    file_delete(file_name);
}

void test_file_create_temporary() {
    // when
    File* file = file_create_temporary();
    // then
    TEST_ASSERT_NOT_NULL(file);
    // clean up
    file_close(file);
}

void test_file_create_directory() {
    // when
    file_create_directory("test_dir");
    // then
    TEST_ASSERT_TRUE(file_exists("test_dir"));
    // clean up
    file_delete("test_dir");
}

void test_file_list_directory() {
    // given
    file_create_directory("test_dir");
    file_close(file_create("test_dir/file_1.txt"));
    file_close(file_create("test_dir/file_2.txt"));
    // when
    Array(String) files = file_list_directory("test_dir");
    // then
    TEST_ASSERT_EQUAL(2, array_length(files));
    TEST_ASSERT_EQUAL_STRING("file_2.txt", string_data(files[0]));
    TEST_ASSERT_EQUAL_STRING("file_1.txt", string_data(files[1]));
    // clean up
    array_destroy(&files);
    file_delete("test_dir/file_1.txt");
    file_delete("test_dir/file_2.txt");
    file_delete("test_dir");
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

#ifdef __linux__

void test_file_truncate() {
    // given
    File* file = file_create(file_name);
    file_write_string(file, "Hello World!\n");
    file_rewind(file);
    // when
    file_truncate(file, 0);
    // then
    TEST_ASSERT_EQUAL(0, file_size(file));
    // clean up
    file_close(file);
    file_delete(file_name);
}

#endif

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

#ifdef __linux__

void test_file_info() {
    // given
    File* file = file_create(file_name);
    file_write_string(file, "Hello World!\n");
    file_close(file);
    // when
    FileInfo file_metadata = file_info(file_name);
    // then
    TEST_ASSERT_EQUAL(13, file_metadata.size);
    TEST_ASSERT_TRUE(file_metadata.is_regular);
    TEST_ASSERT_FALSE(file_metadata.is_directory);
    TEST_ASSERT_FALSE(file_metadata.is_symbolic_link);
}

#endif

void test_file_copy() {
    // given
    File* source = file_create("source.txt");
    file_write_string(source, "Hello World!\n");
    file_close(source);
    // when
    file_copy("source.txt", "destination.txt");
    // then
    File* destination = file_open("destination.txt", FILE_READ);
    String string = file_read_line(destination);
    TEST_ASSERT_EQUAL_STRING("Hello World!\n", string_data(string));
    // clean up
    string_destroy(&string);
    file_close(destination);
    file_delete("source.txt");
    file_delete("destination.txt");
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
    RUN_TEST(test_file_create);
    RUN_TEST(test_file_create_temporary);
    RUN_TEST(test_file_create_directory);
    RUN_TEST(test_file_list_directory);
    RUN_TEST(test_file_read_and_write_data);
    RUN_TEST(test_file_read_and_write_char);
    RUN_TEST(test_file_read_line_and_write_string);
#ifdef __linux__
    RUN_TEST(test_file_truncate);
#endif
    RUN_TEST(test_file_exists);
    RUN_TEST(test_file_size);
#ifdef __linux__
    RUN_TEST(test_file_info);
#endif
    RUN_TEST(test_file_copy);
    RUN_TEST(test_file_move);
    RUN_TEST(test_file_delete);
    RUN_TEST(test_file_delete_if_exists);
    return UNITY_END();
}