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

void test_string_trim() {
    // given
    StringView string = string_view("  Hello World!  ");
    // when
    String trimmed = string_trim(string);
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("Hello World!", trimmed.data, trimmed.length);
    TEST_ASSERT_EQUAL(strlen("Hello World!"), trimmed.length);
}

void test_string_trim_start() {
    // given
    StringView string = string_view("  Hello World!  ");
    // when
    String trimmed = string_trim_start(string);
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("Hello World!  ", trimmed.data, trimmed.length);
    TEST_ASSERT_EQUAL(strlen("Hello World!  "), trimmed.length);
}

void test_string_trim_end() {
    // given
    StringView string = string_view("  Hello World!  ");
    // when
    String trimmed = string_trim_end(string);
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("  Hello World!", trimmed.data, trimmed.length);
    TEST_ASSERT_EQUAL(strlen("  Hello World!"), trimmed.length);
}

void test_string_substring() {
    // given
    StringView string = string_view("Hello World!");
    // when
    StringView substring = string_substring(string, 1, 4);
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("ello", substring.data, substring.length);
    TEST_ASSERT_EQUAL(strlen("ello"), substring.length);
}

static void substring_index_out_of_bounds_test_helper(int start, int length) {
    // given
    StringView string = string_view("Hello World!");
    // when
    StringView substring; Error error = attempt(substring = string_substring(string, start, length));
    // then
    TEST_ASSERT_NULL(substring.data);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_string_substring_length_less_than_zero_fails() {
    substring_index_out_of_bounds_test_helper(0, -1);
}

void test_string_substring_negative_start_index_fails() {
    substring_index_out_of_bounds_test_helper(-1, 3);
}

void test_string_substring_start_index_greater_than_length_fails() {
    substring_index_out_of_bounds_test_helper(30, 3);
}

void test_string_clone() {
    // given
    StringView string = string_view("Hello World!");
    // when
    StringView copy_string = string_clone(string);
    // then
    TEST_ASSERT_TRUE(string_equals(string, copy_string));
}

void test_string_concat() {
    // given
    StringView hello = string_view("Hello ");
    StringView world = string_view("World!");
    // when
    StringOwned hello_world = string_concat(hello, world);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", hello_world.data);
    TEST_ASSERT_EQUAL(hello.length + world.length, hello_world.length);
    // clean up
    string_destroy(&hello_world);
}

void test_string_repeat() {
    // given
    StringView hello = string_view("Hello");
    // when
    StringOwned hello_n_times = string_repeat(hello, 3);
    // then
    TEST_ASSERT_EQUAL_STRING("HelloHelloHello", hello_n_times.data);
    TEST_ASSERT_EQUAL(strlen("HelloHelloHello"), hello_n_times.length);
}

void test_string_repeat_fails_if_times_is_negative() {
    // given
    StringView hello = string_view("Hello");
    // when
    StringOwned hello_n_times; Error error = attempt(hello_n_times = string_repeat(hello, -1));
    // then
    TEST_ASSERT_NULL(hello_n_times.data);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_string_join() {
    // given
    StringView hello = string_view("Hello");
    StringView world = string_view("World");
    StringView exclamation = string_view("!!!");
    // when
    StringOwned full_hello_world = string_join(string_view(" "), hello, world, exclamation, string_null());
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World !!!", full_hello_world.data);
    TEST_ASSERT_EQUAL(strlen("Hello World !!!"), full_hello_world.length);
}

void test_string_split() {
    // given
    StringView string = string_view("Hello World !!!");
    // when
    StringView* strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("Hello", strings[0].data, strings[0].length);
    TEST_ASSERT_EQUAL(strlen("Hello"), strings[0].length);
    // and
    TEST_ASSERT_EQUAL_STRING_LEN("World", strings[1].data, strings[1].length);
    TEST_ASSERT_EQUAL(strlen("World"), strings[1].length);
    // and
    TEST_ASSERT_EQUAL_STRING_LEN("!!!", strings[2].data, strings[2].length);
    TEST_ASSERT_EQUAL(strlen("!!!"), strings[2].length);
    // and
    TEST_ASSERT_NULL(strings[3].data);
    // clean up
    strings_memory_dealloc(strings);
}

void test_string_split_single_word() {
    // given
    StringView string = string_view("Hello");
    // when
    StringView* strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("Hello", strings[0].data, strings[0].length);
    TEST_ASSERT_EQUAL(strlen("Hello"), strings[0].length);
    // and
    TEST_ASSERT_NULL(strings[1].data);
    // clean up
    strings_memory_dealloc(strings);
}

void test_string_split_empty() {
    // given
    StringView string = string_view("");
    // when
    StringView* strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("", strings[0].data, strings[0].length);
    TEST_ASSERT_NULL(strings[1].data);
    // clean up
    strings_memory_dealloc(strings);
}

void test_string_to_uppercase() {
    // given
    StringView string = string_view("Hello World!");
    // when
    StringOwned uppercase = string_to_uppercase(string);
    // then
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD!", uppercase.data);
    // clean up
    string_destroy(&uppercase);
}

void test_string_to_lowercase() {
    // given
    StringView string = string_view("Hello World!");
    // when
    StringOwned lowercase = string_to_lowercase(string);
    // then
    TEST_ASSERT_EQUAL_STRING("hello world!", lowercase.data);
    // clean up
    string_destroy(&lowercase);
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
    RUN_TEST(test_string_trim);
    RUN_TEST(test_string_trim_start);
    RUN_TEST(test_string_trim_end);
    RUN_TEST(test_string_substring);
    RUN_TEST(test_string_substring_length_less_than_zero_fails);
    RUN_TEST(test_string_substring_negative_start_index_fails);
    RUN_TEST(test_string_substring_start_index_greater_than_length_fails);
    RUN_TEST(test_string_clone);
    RUN_TEST(test_string_concat);
    RUN_TEST(test_string_repeat);
    RUN_TEST(test_string_repeat_fails_if_times_is_negative);
    RUN_TEST(test_string_join);
    RUN_TEST(test_string_split);
    RUN_TEST(test_string_split_single_word);
    RUN_TEST(test_string_split_empty);
    RUN_TEST(test_string_to_uppercase);
    RUN_TEST(test_string_to_lowercase);
    return UNITY_END();
}