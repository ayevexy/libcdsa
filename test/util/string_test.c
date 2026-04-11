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

void test_compare_strings_with_same_length() {
    // given
    StringView string = string_view("aaaaa"); // 'a' is greater than 'A' in ASCII
    StringView other_string = string_view("AAAAA");
    // when
    int greater = string_compare(string, other_string);
    int equals = string_compare(string, string);
    int lesser = string_compare(other_string, string);
    // then
    TEST_ASSERT(greater > 0);
    TEST_ASSERT(equals == 0);
    TEST_ASSERT(lesser < 0);
}

void test_compare_strings_with_different_lengths() {
    // given
    StringView string = string_view("aAaAA");
    StringView other_string = string_view("AaA");
    // when
    int greater = string_compare(string, other_string);
    int lesser = string_compare(other_string, string);
    // then
    TEST_ASSERT(greater > 0);
    TEST_ASSERT(lesser < 0);
}

void test_compare_strings_with_same_length_ignore_case() {
    // given
    StringView string = string_view("aAaAa"); // 'a' is greater than 'A' in ASCII
    StringView other_string = string_view("AAaAA");
    // when
    int equals = string_compare_ignore_case(string, other_string);
    // then
    TEST_ASSERT(equals == 0);
}

void test_compare_strings_with_different_lengths_ignore_case() {
    // given
    StringView string = string_view("aAaAA");
    StringView other_string = string_view("AaA");
    // when
    int greater = string_compare_ignore_case(string, other_string);
    int lesser = string_compare_ignore_case(other_string, string);
    // then
    TEST_ASSERT(greater > 0);
    TEST_ASSERT(lesser < 0);
}

void test_string_equals() {
    // given
    StringView string = string_view("Hello World!");
    StringView other_string = string_view("HELLO WORLD!");
    // then
    TEST_ASSERT_TRUE(string_equals(string, string));
    TEST_ASSERT_FALSE(string_equals(string, other_string));
}

void test_string_equals_ignore_case() {
    // given
    StringView string = string_view("Hello World!");
    StringView other_string = string_view("HELLO WORLD!");
    // then
    TEST_ASSERT_TRUE(string_equals(string, string));
    TEST_ASSERT_TRUE(string_equals_ignore_case(string, other_string));
}

void test_string_index_of_char() {
    // given
    StringView string = string_view("Hello World!");
    // when
    int index = string_index_of_char(string, 'l');
    int not_found = string_index_of_char(string, '9');
    // then
    TEST_ASSERT_EQUAL(2, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_last_index_of_char() {
    // given
    StringView string = string_view("Hello World!");
    // when
    int index = string_last_index_of_char(string, 'l');
    int not_found = string_last_index_of_char(string, '9');
    // then
    TEST_ASSERT_EQUAL(9, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_index_of_substring() {
    // given
    StringView string = string_view("Hello World!");
    // when
    int index = string_index_of_substring(string, string_view("ll"));
    int not_found = string_index_of_substring(string, string_view("aaa"));
    // then
    TEST_ASSERT_EQUAL(2, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_last_index_of_substring() {
    // given
    StringView string = string_view("Hello Worlld!");
    // when
    int index = string_last_index_of_substring(string, string_view("ll"));
    int not_found = string_last_index_of_substring(string, string_view("aaa"));
    // then
    TEST_ASSERT_EQUAL(9, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_contains_substring() {
    // given
    StringView string = string_view("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_contains(string, string_view("llo Wor")));
    TEST_ASSERT_FALSE(string_contains(string, string_view("Mars")));
}

void test_string_starts_with_prefix() {
    // given
    StringView string = string_view("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_starts_with(string, string_view("Hello")));
    TEST_ASSERT_FALSE(string_starts_with(string, string_view("World!")));
}

void test_string_ends_with_prefix() {
    // given
    StringView string = string_view("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_ends_with(string, string_view("World!")));
    TEST_ASSERT_FALSE(string_ends_with(string, string_view("Hello")));
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
    RUN_TEST(test_compare_strings_with_same_length);
    RUN_TEST(test_compare_strings_with_different_lengths);
    RUN_TEST(test_compare_strings_with_same_length_ignore_case);
    RUN_TEST(test_compare_strings_with_different_lengths_ignore_case);
    RUN_TEST(test_string_equals);
    RUN_TEST(test_string_equals_ignore_case);
    RUN_TEST(test_string_index_of_char);
    RUN_TEST(test_string_last_index_of_char);
    RUN_TEST(test_string_index_of_substring);
    RUN_TEST(test_string_last_index_of_substring);
    RUN_TEST(test_string_contains_substring);
    RUN_TEST(test_string_starts_with_prefix);
    RUN_TEST(test_string_ends_with_prefix);
    return UNITY_END();
}