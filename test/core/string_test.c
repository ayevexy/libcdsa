#include "core/string.h"

#include "core/errors.h"

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
    String string = string_new(raw_string);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", string->data);
    TEST_ASSERT_EQUAL(strlen(raw_string), string->length);
    // clean up
    string_destroy(&string);
}

void test_destroy_string() {
    // given
    String string = string_new("Hello World!");
    // when
    string_destroy(&string);
    // then
    TEST_ASSERT_NULL(string);
}

void test_reference_string() {
    // given
    const char* s1 = "Hello World!";
    char* s2 = "Hello World!";
    // when
    String s1_ref = string_ref(s1);
    String s2_ref = string_ref(s2);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", s1_ref->data);
    TEST_ASSERT_EQUAL(strlen(s1), s1_ref->length);
    // and
    TEST_ASSERT_EQUAL_STRING("Hello World!", s2_ref->data);
    TEST_ASSERT_EQUAL(strlen(s2), s2_ref->length);
}

void test_create_formatted_string() {
    // given
    String format = string_ref("H%dllo W%drld!");
    // when
    String string = string_format(format, 3, 0);
    // then
    TEST_ASSERT_EQUAL_STRING("H3llo W0rld!", string->data);
    TEST_ASSERT_EQUAL(strlen(format->data) - 2, string->length);
    // clean up
    string_destroy(&string);
}

void test_get_char_at_index_from_string() {
    // given
    String string = string_ref("Hello World!");
    // when
    char c = string_char_at(string, 3);
    // then
    TEST_ASSERT_EQUAL('l', c);
}

static void get_chat_at_index_out_of_bounds_test_helper(int index) {
    // given
    String string = string_ref("Hello World!");
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
    String string = string_ref("");
    // when
    bool empty = string_is_empty(string);
    // then
    TEST_ASSERT_TRUE(empty);
}

void test_string_is_not_empty() {
    // given
    String string = string_ref("Hello World!");
    // when
    bool empty = string_is_empty(string);
    // then
    TEST_ASSERT_FALSE(empty);
}

void test_string_is_blank() {
    // given
    String string = string_ref("    ");
    // when
    bool blank = string_is_blank(string);
    // then
    TEST_ASSERT_TRUE(blank);
}

void test_string_is_not_blank() {
    // given
    String string = string_ref("    Hello World!    ");
    // when
    bool blank = string_is_blank(string);
    // then
    TEST_ASSERT_FALSE(blank);
}

void test_compare_strings_with_same_length() {
    // given
    String string = string_ref("aaaaa"); // 'a' is greater than 'A' in ASCII
    String other_string = string_ref("AAAAA");
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
    String string = string_ref("aAaAA");
    String other_string = string_ref("AaA");
    // when
    int greater = string_compare(string, other_string);
    int lesser = string_compare(other_string, string);
    // then
    TEST_ASSERT(greater > 0);
    TEST_ASSERT(lesser < 0);
}

void test_compare_strings_with_same_length_ignore_case() {
    // given
    String string = string_ref("aAaAa"); // 'a' is greater than 'A' in ASCII
    String other_string = string_ref("AAaAA");
    // when
    int equals = string_compare_ignore_case(string, other_string);
    // then
    TEST_ASSERT(equals == 0);
}

void test_compare_strings_with_different_lengths_ignore_case() {
    // given
    String string = string_ref("aAaAA");
    String other_string = string_ref("AaA");
    // when
    int greater = string_compare_ignore_case(string, other_string);
    int lesser = string_compare_ignore_case(other_string, string);
    // then
    TEST_ASSERT(greater > 0);
    TEST_ASSERT(lesser < 0);
}

void test_string_equals() {
    // given
    String string = string_ref("Hello World!");
    String other_string = string_ref("HELLO WORLD!");
    // then
    TEST_ASSERT_TRUE(string_equals(string, string));
    TEST_ASSERT_FALSE(string_equals(string, other_string));
}

void test_string_equals_ignore_case() {
    // given
    String string = string_ref("Hello World!");
    String other_string = string_ref("HELLO WORLD!");
    // then
    TEST_ASSERT_TRUE(string_equals(string, string));
    TEST_ASSERT_TRUE(string_equals_ignore_case(string, other_string));
}

void test_string_index_of_char() {
    // given
    String string = string_ref("Hello World!");
    // when
    int index = string_index_of(string, 'l');
    int not_found = string_index_of(string, '9');
    // then
    TEST_ASSERT_EQUAL(2, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_last_index_of_char() {
    // given
    String string = string_ref("Hello World!");
    // when
    int index = string_last_index_of(string, 'l');
    int not_found = string_last_index_of(string, '9');
    // then
    TEST_ASSERT_EQUAL(9, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_index_of_substring() {
    // given
    String string = string_ref("Hello World!");
    // when
    int index = string_index_of(string, "ll");
    int not_found = string_index_of(string, "aaa");
    // then
    TEST_ASSERT_EQUAL(2, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_last_index_of_substring() {
    // given
    String string = string_ref("Hello Worlld!");
    // when
    int index = string_last_index_of(string, "ll");
    int not_found = string_last_index_of(string, "aaa");
    // then
    TEST_ASSERT_EQUAL(9, index);
    TEST_ASSERT_EQUAL(-1, not_found);
}

void test_string_contains_substring() {
    // given
    String string = string_ref("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_contains(string, "llo Wor"));
    TEST_ASSERT_FALSE(string_contains(string, "Mars"));
}

void test_string_starts_with_prefix() {
    // given
    String string = string_ref("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_starts_with(string, "Hello"));
    TEST_ASSERT_FALSE(string_starts_with(string, "World!"));
}

void test_string_ends_with_prefix() {
    // given
    String string = string_ref("Hello World!");
    // then
    TEST_ASSERT_TRUE(string_ends_with(string, "World!"));
    TEST_ASSERT_FALSE(string_ends_with(string, "Hello"));
}

void test_string_trim() {
    // given
    String string = string_ref("  Hello World!  ");
    // when
    String trimmed = string_trim(string);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", trimmed->data);
    TEST_ASSERT_EQUAL(strlen("Hello World!"), trimmed->length);
    // clean up
    string_destroy(&trimmed);
}

void test_string_trim_start() {
    // given
    String string = string_ref("  Hello World!  ");
    // when
    String trimmed = string_trim_start(string);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!  ", trimmed->data);
    TEST_ASSERT_EQUAL(strlen("Hello World!  "), trimmed->length);
    // clean up
    string_destroy(&trimmed);
}

void test_string_trim_end() {
    // given
    String string = string_ref("  Hello World!  ");
    // when
    String trimmed = string_trim_end(string);
    // then
    TEST_ASSERT_EQUAL_STRING("  Hello World!", trimmed->data);
    TEST_ASSERT_EQUAL(strlen("  Hello World!"), trimmed->length);
    // clean up
    string_destroy(&trimmed);
}

void test_string_substring() {
    // given
    String string = string_ref("Hello World!");
    // when
    String substring = string_substring(string, 1, 4);
    // then
    TEST_ASSERT_EQUAL_STRING("ello", substring->data);
    TEST_ASSERT_EQUAL(strlen("ello"), substring->length);
    // clean up
    string_destroy(&substring);
}

static void substring_index_out_of_bounds_test_helper(int start, int length) {
    // given
    String string = string_ref("Hello World!");
    // when
    String substring; Error error = attempt(substring = string_substring(string, start, length));
    // then
    TEST_ASSERT_NULL(substring);
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

void test_string_concat() {
    // given
    String hello = string_ref("Hello ");
    String world = string_ref("World!");
    // when
    String hello_world = string_concat(hello, world);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", hello_world->data);
    TEST_ASSERT_EQUAL(hello->length + world->length, hello_world->length);
    // clean up
    string_destroy(&hello_world);
}

void test_string_replace_char() {
    // given
    String hello = string_ref("Hello World!");
    // when
    String replaced = string_replace(hello, 'o', '0');
    // then
    TEST_ASSERT_EQUAL_STRING("Hell0 W0rld!", replaced->data);
    // clean up
    string_destroy(&replaced);
}

void test_string_replace_substring() {
    // given
    String hello = string_ref("Hello Worlld!");
    // when
    String replaced = string_replace(hello, "ll", "123");
    // then
    TEST_ASSERT_EQUAL_STRING("He123o Wor123d!", replaced->data);
    TEST_ASSERT_EQUAL(strlen("He123o Wor123d!"), replaced->length);
    // clean up
    string_destroy(&replaced);
}

void test_string_repeat() {
    // given
    String hello = string_ref("Hello");
    // when
    String hello_n_times = string_repeat(hello, 3);
    // then
    TEST_ASSERT_EQUAL_STRING("HelloHelloHello", hello_n_times->data);
    TEST_ASSERT_EQUAL(strlen("HelloHelloHello"), hello_n_times->length);
    // clean up
    string_destroy(&hello_n_times);
}

void test_string_repeat_fails_if_times_is_negative() {
    // given
    String hello = string_ref("Hello");
    // when
    String hello_n_times; Error error = attempt(hello_n_times = string_repeat(hello, -1));
    // then
    TEST_ASSERT_NULL(hello_n_times);
    TEST_ASSERT_EQUAL(ILLEGAL_ARGUMENT_ERROR, error);
}

void test_string_join() {
    // given
    String hello = string_ref("Hello");
    String world = string_ref("World");
    String exclamation = string_ref("!!!");
    // when
    String full_hello_world = string_join(" ", hello, world, exclamation);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World !!!", full_hello_world->data);
    TEST_ASSERT_EQUAL(strlen("Hello World !!!"), full_hello_world->length);
    // clean up
    string_destroy(&full_hello_world);
}

void test_string_split() {
    // given
    String string = string_ref("Hello World !!!");
    // when
    Array(String) strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING("Hello", strings[0]->data);
    TEST_ASSERT_EQUAL(strlen("Hello"), strings[0]->length);
    // and
    TEST_ASSERT_EQUAL_STRING("World", strings[1]->data);
    TEST_ASSERT_EQUAL(strlen("World"), strings[1]->length);
    // and
    TEST_ASSERT_EQUAL_STRING("!!!", strings[2]->data);
    TEST_ASSERT_EQUAL(strlen("!!!"), strings[2]->length);
    // clean up
    array_destroy(&strings);
}

void test_string_split_single_word() {
    // given
    String string = string_ref("Hello");
    // when
    Array(String) strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING("Hello", strings[0]->data);
    TEST_ASSERT_EQUAL(strlen("Hello"), strings[0]->length);
    // clean up
    array_destroy(&strings);
}

void test_string_split_empty() {
    // given
    String string = string_ref("");
    // when
    Array(String) strings = string_split(string, ' ');
    // then
    TEST_ASSERT_EQUAL_STRING_LEN("", strings[0]->data, strings[0]->length);
    // clean up
    array_destroy(&strings);
}

void test_string_to_uppercase() {
    // given
    String string = string_ref("Hello World!");
    // when
    String uppercase = string_to_uppercase(string);
    // then
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD!", uppercase->data);
    // clean up
    string_destroy(&uppercase);
}

void test_string_to_lowercase() {
    // given
    String string = string_ref("Hello World!");
    // when
    String lowercase = string_to_lowercase(string);
    // then
    TEST_ASSERT_EQUAL_STRING("hello world!", lowercase->data);
    // clean up
    string_destroy(&lowercase);
}

void test_string_value_of() {
    TEST_ASSERT_EQUAL_STRING("true", string_value_of(true)->data);
    TEST_ASSERT_EQUAL_STRING("\n", string_value_of((char) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((short) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((unsigned short) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((int) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((unsigned int) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((long) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((unsigned long) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((long long) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10", string_value_of((unsigned long long) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10.000000", string_value_of((float) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10.000000", string_value_of((double) 10)->data);
    TEST_ASSERT_EQUAL_STRING("10.000000", string_value_of((long double) 10)->data);
}

/* --------------------------------------------------------------------------------- */

void test_create_and_destroy_string_builder() {
    // when
    StringBuilder *sb = string_builder_new();
    // then
    TEST_ASSERT_NOT_NULL(sb->buffer);
    TEST_ASSERT_EQUAL(0, sb->length);
    TEST_ASSERT_EQUAL(10, sb->capacity);
    // cleanup
    string_builder_destroy(&sb);
    TEST_ASSERT_NULL(sb);
}

void test_string_builder_append() {
    // given
    StringBuilder* sb = string_builder_new();
    // when
    string_builder_append(sb, "Hello");
    // then
    TEST_ASSERT_EQUAL_STRING("Hello", sb->buffer);
    TEST_ASSERT_EQUAL(strlen("Hello"), sb->length);
    TEST_ASSERT_EQUAL(10, sb->capacity);
    // cleanup
    string_builder_destroy(&sb);
}

void test_string_builder_append_exceeding_capacity_resize_it() {
    // given
    StringBuilder* sb = string_builder_new();
    // when
    string_builder_append(sb, "Hello ");
    string_builder_append(sb, "World");
    string_builder_append(sb, "!");
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", sb->buffer);
    TEST_ASSERT_EQUAL(strlen("Hello World!"), sb->length);
    TEST_ASSERT_EQUAL(20, sb->capacity);
    // cleanup
    string_builder_destroy(&sb);
}

void test_string_builder_insert() {
    // given
    StringBuilder* sb = string_builder_new();
    // and
    string_builder_append(sb, "Hello");
    string_builder_append(sb, "World");
    string_builder_append(sb, "!");
    // when
    string_builder_insert(sb, 5, " ");
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", sb->buffer);
    TEST_ASSERT_EQUAL(strlen("Hello World!"), sb->length);
    TEST_ASSERT_EQUAL(20, sb->capacity);
    // cleanup
    string_builder_destroy(&sb);
}

static void insert_index_out_of_bounds_test_helper(int index) {
    // given
    StringBuilder* sb = string_builder_new();
    // when
    Error error = attempt(string_builder_insert(sb, index, "Hello"));
    // then
    TEST_ASSERT_EQUAL_STRING("", sb->buffer);
    TEST_ASSERT_EQUAL(INDEX_OUT_OF_BOUNDS_ERROR, error);
}

void test_string_builder_insert_index_above_bounds_fails() {
    insert_index_out_of_bounds_test_helper(10);
}

void test_string_builder_insert_negative_index_fails() {
    insert_index_out_of_bounds_test_helper(-1);
}

void test_string_builder_to_string() {
    // given
    StringBuilder* sb = string_builder_new();
    // and
    string_builder_append(sb, "Hello ");
    string_builder_append(sb, "World");
    string_builder_append(sb, "!");
    // when
    String string = string_builder_to_string(sb);
    // then
    TEST_ASSERT_EQUAL_STRING("Hello World!", string->data);
    TEST_ASSERT_EQUAL(strlen("Hello World!"), string->length);
    // cleanup
    string_builder_destroy(&sb);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_string);
    RUN_TEST(test_destroy_string);
    RUN_TEST(test_reference_string);
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
    RUN_TEST(test_string_concat);
    RUN_TEST(test_string_replace_char);
    RUN_TEST(test_string_replace_substring);
    RUN_TEST(test_string_repeat);
    RUN_TEST(test_string_repeat_fails_if_times_is_negative);
    RUN_TEST(test_string_join);
    RUN_TEST(test_string_split);
    RUN_TEST(test_string_split_single_word);
    RUN_TEST(test_string_split_empty);
    RUN_TEST(test_string_to_uppercase);
    RUN_TEST(test_string_to_lowercase);
    RUN_TEST(test_string_value_of);
    /* ---------------------------------------------------- */
    RUN_TEST(test_create_and_destroy_string_builder);
    RUN_TEST(test_string_builder_append);
    RUN_TEST(test_string_builder_append_exceeding_capacity_resize_it);
    RUN_TEST(test_string_builder_insert);
    RUN_TEST(test_string_builder_insert_index_above_bounds_fails);
    RUN_TEST(test_string_builder_insert_negative_index_fails);
    RUN_TEST(test_string_builder_to_string);
    return UNITY_END();
}