#include "util/string.h"

#include "util/errors.h"
#include "unity.h"
#include <string.h>

void setUp() {

}

void tearDown() {

}

void test_create_string() {
    // given
    const char* raw_string = "Hello World!";
    // when
    StringOwned string = string_new(raw_string);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", string.data);
    TEST_ASSERT_EQUAL(strlen(raw_string), string.length);
}

void test_destroy_string() {
    // given
    StringOwned string = string_new("Hello World!");
    // when
    string_destroy(&string);
    // then
    TEST_ASSERT_NULL(string.data);
}

void test_create_string_view() {
    // given
    const char* raw_string = "Hello World!";
    // when
    StringView string = string_view(raw_string);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", string.data);
    TEST_ASSERT_EQUAL(strlen(raw_string), string.length);
}

void test_create_formatted_string() {
    // given
    const char* format = "H%dllo W%drld!";
    int e = 3, o = 0;
    // when
    StringOwned string = string_format(format, e, o);
    // then
    TEST_ASSERT_EQUAL_STRING("H3llo W0rld!", string.data);
    TEST_ASSERT_EQUAL(strlen(format) - 2, string.length);
}

void test_get_char_at_index_from_string() {
    // given
    StringView string = string_view("Hello World!");
    // when
    char c = string_char_at(string, 3);
    // then
    TEST_ASSERT_EQUAL('l', c);
}

static void get_chat_at_index_out_of_bounds_test_helper(int index) {
    // given
    StringView string = string_view("Hello World!");
    // when
    char c; Error error = attempt(c = string_char_at(string, index));
    // then
    TEST_ASSERT_EQUAL('\0', c);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_get_char_from_string_index_above_bounds_fails() {
    get_chat_at_index_out_of_bounds_test_helper(235);
}

void test_get_char_from_string_negative_index_fails() {
    get_chat_at_index_out_of_bounds_test_helper(-1);
}

void test_string_is_empty() {
    // given
    StringView string = string_view("");
    // when
    bool empty = string_is_empty(string);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_string_is_not_empty() {
    // given
    StringView string = string_view("Hello World!");
    // when
    bool empty = string_is_empty(string);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_string_is_blank() {
    // given
    StringView string = string_view("    ");
    // when
    bool blank = string_is_blank(string);
    // then
    TEST_ASSERT_TRUE(blank);
}

void test_string_is_not_blank() {
    // given
    StringView string = string_view("    Hello World!    ");
    // when
    bool blank = string_is_blank(string);
    // then
    TEST_ASSERT_FALSE(blank);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_string);
    RUN_TEST(test_destroy_string);
    RUN_TEST(test_create_string_view);
    RUN_TEST(test_create_formatted_string);
    RUN_TEST(test_get_char_at_index_from_string);
    RUN_TEST(test_get_char_from_string_index_above_bounds_fails);
    RUN_TEST(test_get_char_from_string_negative_index_fails);
    RUN_TEST(test_string_is_empty);
    RUN_TEST(test_string_is_not_empty);
    RUN_TEST(test_string_is_blank);
    RUN_TEST(test_string_is_not_blank);
    return UNITY_END();
}