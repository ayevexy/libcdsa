#include "util/temporal.h"

#include "unity.h"

#define TEST_ASSERT_DATE_TIME(year_, month_, day_, hour_, minute_, second_, date_time)      \
    do {                                                                                    \
        DateTime date_time_ = (date_time);                                                  \
        TEST_ASSERT_EQUAL(year_, date_time_.year);                                          \
        TEST_ASSERT_EQUAL(month_, date_time_.month);                                        \
        TEST_ASSERT_EQUAL(day_, date_time_.day);                                            \
        TEST_ASSERT_EQUAL(hour_, date_time_.hour);                                          \
        TEST_ASSERT_EQUAL(minute_, date_time_.minute);                                      \
        TEST_ASSERT_EQUAL(second_, date_time_.second);                                      \
    } while (false)

void setUp() {

}

void tearDown() {

}

void test_date_time_now() {
    // then
    TEST_ASSERT_EQUAL(NO_ERROR, attempt(date_time_now()));
}

void test_date_time_of() {
    // then
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 23, 30, date_time_of(2026, 10, 24, 22, 23, 30));
    TEST_ASSERT_DATE_TIME(2024, 2, 29, 7, 10, 23, date_time_of(2024, 02, 29, 7, 10, 23));
    TEST_ASSERT_DATE_TIME(2023, 2, 28, 7, 10, 23, date_time_of(2023, 02, 28, 7, 10, 23));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(-1'000'000'000, 10, 24, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(1'000'000'000, 10, 24, 22, 23, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 13, 24, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 0, 24, 22, 23, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 32, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 00, 22, 23, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, 24, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, -1, 23, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, 22, 60, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, 22, -1, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, 22, 23, 60)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 10, 24, 22, 23, -1)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 2, 30, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 2, 29, 22, 23, 30)));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 4, 31, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 6, 31, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 9, 31, 22, 23, 30)));
    TEST_ASSERT_EQUAL(DATE_TIME_INVALID_ERROR, attempt(date_time_of(2026, 11, 31, 22, 23, 30)));
}

void test_date_time_parse() {
    // then
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 23, 30, date_time_parse("2026-10-24T22:23:30"));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_FORMAT_ERROR, attempt(date_time_parse("3452345")));
    TEST_ASSERT_EQUAL(DATE_TIME_FORMAT_ERROR, attempt(date_time_parse("2026-10-24T22:23:30abc")));
}

void test_date_time_week_day() {
    // then
    TEST_ASSERT_EQUAL(FRIDAY, date_time_week_day(date_time_of(2026, 10, 23, 22, 23, 30)));
    TEST_ASSERT_EQUAL(SATURDAY, date_time_week_day(date_time_of(2026, 10, 24, 22, 23, 30)));
    TEST_ASSERT_EQUAL(SUNDAY, date_time_week_day(date_time_of(2026, 10, 25, 22, 23, 30)));
}

void test_date_time_year_day() {
    // then
    TEST_ASSERT_EQUAL(296, date_time_year_day(date_time_of(2026, 10, 23, 22, 23, 30)));
    TEST_ASSERT_EQUAL(297, date_time_year_day(date_time_of(2026, 10, 24, 22, 23, 30)));
    TEST_ASSERT_EQUAL(298, date_time_year_day(date_time_of(2026, 10, 25, 22, 23, 30)));
}

void test_date_time_is_leap_year() {
    // then
    TEST_ASSERT_TRUE(date_time_is_leap_year(date_time_of(2024, 02, 29, 07, 10, 23)));
    TEST_ASSERT_FALSE(date_time_is_leap_year(date_time_of(2023, 02, 28, 07, 10, 23)));
}

void test_date_time_is_after() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // then
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2025, 10, 24, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2026, 9, 24, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2026, 10, 23, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 15, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 22, 10, 30)));
    TEST_ASSERT_TRUE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 22, 23, 25)));
    // and
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2027, 10, 24, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2026, 11, 24, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2026, 10, 29, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 23, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 22, 31, 30)));
    TEST_ASSERT_FALSE(date_time_is_after(date_time, date_time_of(2026, 10, 24, 22, 31, 47)));
}

void test_date_time_is_before() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // then
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2027, 10, 24, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2026, 11, 24, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2026, 10, 29, 22, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 23, 23, 30)));
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 22, 31, 30)));
    TEST_ASSERT_TRUE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 22, 31, 47)));
    // and
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2025, 10, 24, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2026, 9, 24, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2026, 10, 23, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 15, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 22, 10, 30)));
    TEST_ASSERT_FALSE(date_time_is_before(date_time, date_time_of(2026, 10, 24, 22, 23, 25)));
}

void test_date_time_is_equal() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // then
    TEST_ASSERT_TRUE(date_time_is_equal(date_time, date_time_of(2026, 10, 24, 22, 23, 30)));
    TEST_ASSERT_FALSE(date_time_is_equal(date_time, date_time_of(2027, 11, 25, 23, 24, 31)));
}

void test_date_time_plus() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // then
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 23, 30, date_time_plus(date_time, 0, SECOND));
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 24, 30, date_time_plus(date_time, 1, MINUTE));
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 23, 23, 30, date_time_plus(date_time, 1, HOUR));
    TEST_ASSERT_DATE_TIME(2026, 10, 25, 22, 23, 30, date_time_plus(date_time, 1, DAY));
    TEST_ASSERT_DATE_TIME(2026, 10, 31, 22, 23, 30, date_time_plus(date_time, 1, WEEK));
    TEST_ASSERT_DATE_TIME(2026, 11, 24, 22, 23, 30, date_time_plus(date_time, 1, MONTH));
    TEST_ASSERT_DATE_TIME(2027, 10, 24, 22, 23, 30, date_time_plus(date_time, 1, YEAR));
    // and
    TEST_ASSERT_DATE_TIME(2026, 10, 27, 22, 23, 30, date_time_plus(date_time, 72, HOUR));
    TEST_ASSERT_DATE_TIME(2026, 10, 27, 22, 23, 30, date_time_plus(date_time, 3, DAY));
}

void test_date_time_minus() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // then
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 23, 30, date_time_minus(date_time, 0, SECOND));
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 22, 22, 30, date_time_minus(date_time, 1, MINUTE));
    TEST_ASSERT_DATE_TIME(2026, 10, 24, 21, 23, 30, date_time_minus(date_time, 1, HOUR));
    TEST_ASSERT_DATE_TIME(2026, 10, 23, 22, 23, 30, date_time_minus(date_time, 1, DAY));
    TEST_ASSERT_DATE_TIME(2026, 10, 17, 22, 23, 30, date_time_minus(date_time, 1, WEEK));
    TEST_ASSERT_DATE_TIME(2026, 9, 24, 22, 23, 30, date_time_minus(date_time, 1, MONTH));
    TEST_ASSERT_DATE_TIME(2025, 10, 24, 22, 23, 30, date_time_minus(date_time, 1, YEAR));
    // and
    TEST_ASSERT_DATE_TIME(2026, 10, 21, 22, 23, 30, date_time_minus(date_time, 72, HOUR));
    TEST_ASSERT_DATE_TIME(2026, 10, 21, 22, 23, 30, date_time_minus(date_time, 3, DAY));
}

void test_date_time_difference() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    DateTime other = date_time_of(2000, 5, 19, 11, 47, 10);
    // then
    TEST_ASSERT_EQUAL(-26, date_time_difference(date_time, other, YEAR));
    TEST_ASSERT_EQUAL(-317, date_time_difference(date_time, other, MONTH));
    TEST_ASSERT_EQUAL(-1379, date_time_difference(date_time, other, WEEK));
    TEST_ASSERT_EQUAL(-9'654, date_time_difference(date_time, other, DAY));
    TEST_ASSERT_EQUAL(-231'706, date_time_difference(date_time, other, HOUR));
    TEST_ASSERT_EQUAL(-13'902'396, date_time_difference(date_time, other, MINUTE));
    TEST_ASSERT_EQUAL(-834'143'780, date_time_difference(date_time, other, SECOND));
    // and
    TEST_ASSERT_EQUAL(26, date_time_difference(other, date_time, YEAR));
    TEST_ASSERT_EQUAL(317, date_time_difference(other, date_time, MONTH));
    TEST_ASSERT_EQUAL(1379, date_time_difference(other, date_time, WEEK));
    TEST_ASSERT_EQUAL(9'654, date_time_difference(other, date_time, DAY));
    TEST_ASSERT_EQUAL(231'706, date_time_difference(other, date_time, HOUR));
    TEST_ASSERT_EQUAL(13'902'396, date_time_difference(other, date_time, MINUTE));
    TEST_ASSERT_EQUAL(834'143'780, date_time_difference(other, date_time, SECOND));
}

void test_date_time_format() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // when
    String format_a = date_time_format(date_time, FULL_DATE_TIME_FORMAT);
    String format_b = date_time_format(date_time, ISO_DATE_TIME_FORMAT);
    // then
    TEST_ASSERT_EQUAL_STRING("Saturday, October 24, 2026 22:23:30", string_data(format_a));
    TEST_ASSERT_EQUAL_STRING("2026-10-24 22:23:30", string_data(format_b));
    // and
    TEST_ASSERT_EQUAL(DATE_TIME_FORMAT_ERROR, attempt(date_time_format(date_time, "%z %d %a")));
    // clean up
    string_destroy(&format_a, &format_b);
}

void test_date_time_to_string() {
    // given
    DateTime date_time = date_time_of(2026, 10, 24, 22, 23, 30);
    // when
    String string = date_time_to_string(date_time);
    // then
    TEST_ASSERT_EQUAL_STRING("2026-10-24T22:23:30", string_data(string));
    // clean up
    string_destroy(&string);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_date_time_now);
    RUN_TEST(test_date_time_of);
    RUN_TEST(test_date_time_parse);
    RUN_TEST(test_date_time_week_day);
    RUN_TEST(test_date_time_year_day);
    RUN_TEST(test_date_time_is_leap_year);
    RUN_TEST(test_date_time_is_after);
    RUN_TEST(test_date_time_is_before);
    RUN_TEST(test_date_time_is_equal);
    RUN_TEST(test_date_time_plus);
    RUN_TEST(test_date_time_minus);
    RUN_TEST(test_date_time_difference);
    RUN_TEST(test_date_time_to_string);
    RUN_TEST(test_date_time_format);
    return UNITY_END();
}
