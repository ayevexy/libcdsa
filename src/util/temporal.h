#ifndef LIBCDSA_TEMPORAL_H
#define LIBCDSA_TEMPORAL_H

/**
 * @file temporal.h
 * @brief Temporal API.
 */

#include "core/errors.h"
#include "core/string.h"

/**
 * @brief Time units.
 */
typedef enum {
    YEAR,
    MONTH,
    WEEK,
    DAY,
    HOUR,
    MINUTE,
    SECOND
} TimeUnit;

/**
 * @brief Calendar's months.
 */
typedef enum {
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} Month;

/**
 * @brief Days of the week.
 */
typedef enum {
    SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} WeekDay;

/**
 * @brief Represents the local calendar's date, without timezone.
 */
typedef struct {
    int year;
    int month;
    int day;
} Date;

/**
 * @brief Date constants.
 */
constexpr Date DATE_MIN = { .year = 1,    .month = 1,  .day = 1 };
constexpr Date DATE_MAX = { .year = 9999, .month = 12, .day = 31};

/**
 * @brief Represents the local time of the day, without timezone.
 */
typedef struct {
    int hour;
    int minute;
    int second;
} Time;

/**
 * @brief Time constants.
 */
constexpr Time TIME_MIN       = { .hour = 0,  .minute = 0,  .second = 0 };
constexpr Time TIME_MIDNIGHT  = { .hour = 0,  .minute = 0,  .second = 0 };
constexpr Time TIME_NOON      = { .hour = 12, .minute = 0,  .second = 0 };
constexpr Time TIME_MAX       = { .hour = 23, .minute = 59, .second = 59 };

/**
 * @brief Represents a local calendar's date alongside the time of the day, without timezone.
 */
typedef struct {
    union {
        Date date;
        struct { int year; int month; int day; };
    };
    union {
        Time time;
        struct { int hour; int minute; int second; };
    };
} DateTime;

/**
 * @brief DateTime constants.
 */
constexpr DateTime DATE_TIME_MIN = { .date = DATE_MIN, .time = TIME_MIN };
constexpr DateTime DATE_TIME_MAX = { .date = DATE_MAX, .time = TIME_MAX };

/**
 * @brief Date/time errors constants.
 */
extern const Error DATE_TIME_ERROR;
extern const Error DATE_TIME_INVALID_ERROR;
extern const Error DATE_TIME_FORMAT_ERROR;

/**
 * @brief Supported date/time formats.
 */
#define ISO_8601_DATE_TIME_FORMAT        "%Y-%m-%dT%H:%M:%S"          // 2026-10-25T17:30:45
#define ISO_DATE_TIME_FORMAT             "%Y-%m-%d %H:%M:%S"          // 2026-10-25 17:30:45
#define US_DATE_TIME_FORMAT              "%m/%d/%Y %H:%M:%S"          // 10/25/2026 17:30:45
#define EUROPEAN_DATE_TIME_FORMAT        "%d/%m/%Y %H:%M:%S"          // 25/10/2026 17:30:45
#define ASCTIME_DATE_TIME_FORMAT         "%a %b %d %H:%M:%S %Y"       // Sun Oct 25 17:30:45 2026
#define FULL_DATE_TIME_FORMAT            "%A, %B %d, %Y %H:%M:%S"     // Sunday, October 25, 2026 17:30:45

#define US_AM_PM_DATE_TIME_FORMAT        "%m/%d/%Y %I:%M:%S %p"       // 10/25/2026 05:30:45 PM
#define EUROPEAN_AM_PM_DATE_TIME_FORMAT  "%d/%m/%Y %I:%M:%S %p"       // 25/10/2026 05:30:45 PM
#define FULL_AM_PM_DATE_TIME_FORMAT      "%A, %B %d, %Y %I:%M:%S %p"  // Sunday, October 25, 2026 05:30:45 PM

#define ISO_DATE_FORMAT                  "%Y-%m-%d"                   // 2026-10-25
#define US_DATE_FORMAT                   "%m/%d/%Y"                   // 10/25/2026
#define EUROPEAN_DATE_FORMAT             "%d/%m/%Y"                   // 25/10/2026
#define ASCTIME_DATE_FORMAT              "%a %b %d %Y"                // Sun Oct 25 2026
#define FULL_DATE_FORMAT                 "%A, %B %d, %Y"              // Sunday, October 25, 2026

#define ISO_TIME_FORMAT                  "%H:%M:%S"                   // 17:30:45
#define US_TIME_FORMAT                   "%I:%M:%S %p"                // 05:30:45 PM

/**
 * @brief Returns the current local date and time.
 *
 * @return the current date and time
 * 
 * @exception DATE_TIME_ERROR if failed to get the current date and time
 *
 * @note This function is thread-safe on Linux.
 */
DateTime date_time_now(void);

/**
 * @brief Creates a date-time from its components.
 *
 * @param year the year (1-9999)
 * @param month the month (1–12)
 * @param day the day of the month
 * @param hour the hour (0–23)
 * @param minute the minute (0–59)
 * @param second the second (0–59)
 *
 * @return the created date-time
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
DateTime date_time_of(int year, int month, int day, int hour, int minute, int second);

/**
 * @brief Creates a date-time from a date and a time
 *
 * @param date the date
 * @param time the time
 *
 * @return the created date-time
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
DateTime date_time_from(Date date, Time time);

/**
 * @brief Parses a string in the format of YYYY-MM-DDTHH:MM:SS into a date-time.
 *
 * @param string the string to parse
 *
 * @return the parsed date-time
 *
 * @exception DATE_TIME_FORMAT_ERROR if failed to parse the string
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
#define date_time_parse(string) date_time_parse(dispatch_string_type(string))

DateTime (date_time_parse)(struct String string);

/**
 * @brief Returns the day of the week.
 *
 * @param date_time the date-time
 *
 * @return the day of the week
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
WeekDay date_time_week_day(DateTime date_time);

/**
 * @brief Returns the day of the year, starting 1.
 *
 * @param date_time the date-time
 *
 * @return the day of the year
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
int date_time_year_day(DateTime date_time);

/**
 * @brief Determines whether the year of a date-time is a leap year.
 *
 * @param date_time the date-time
 *
 * @return true if the year is a leap year, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
bool date_time_is_leap_year(DateTime date_time);

/**
 * @brief Determines whether one date-time is after another
 *
 * @param date_time the date-time being tested
 * @param other the date-time to compare against
 *
 * @return true if date_time is later than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date_time or other is invalid
 */
bool date_time_is_after(DateTime date_time, DateTime other);

/**
 * @brief Determines whether one date-time is before another
 *
 * @param date_time the date-time being tested
 * @param other the date-time to compare against
 *
 * @return true if date_time is earlier than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date_time or other is invalid
 */
bool date_time_is_before(DateTime date_time, DateTime other);

/**
 * @brief Determines whether two date-times are equal
 *
 * @param date_time the first date-time
 * @param other the second date-time
 *
 * @return true if the date-times are equal, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date_time or other is invalid
 */
bool date_time_is_equal(DateTime date_time, DateTime other);

/**
 * @brief Checks whether a date-time is valid.
 *
 * @param date_time the date-time
 *
 * @return true if valid, false otherwise
 */
bool date_time_is_valid(DateTime date_time);

/**
 * @brief Adds a value to a date-time.
 *
 * @param date_time the date-time
 * @param value the amount to add
 * @param unit the temporal unit of the amount
 *
 * @return the resulting date-time
 *
 * @note When adding years or months, the day is clamped to the last valid
 *       day of the resulting month if necessary.
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting date-time
 */
DateTime date_time_plus(DateTime date_time, int value, TimeUnit unit);

/**
 * @brief Subtracts a value from a date-time.
 *
 * @param date_time the date-time
 * @param value the amount to subtract
 * @param unit the temporal unit of the amount
 *
 * @return the resulting date-time
 *
 * @note When subtracting years or months, the day is clamped to the last valid
 *       day of the resulting month if necessary.
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting date-time
 */
DateTime date_time_minus(DateTime date_time, int value, TimeUnit unit);

/**
 * @brief Formats a date-time using one of the supported formats.
 *
 * @param date_time the date-time
 * @param format the format string
 *
 * @return the formatted date-time
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 * @exception NULL_POINTER_ERROR if format is null
 * @exception DATE_TIME_FORMAT_ERROR if the format is not supported
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String date_time_format(DateTime date_time, const char* format);

/**
 * @brief Converts a date-time to its default string representation.
 *
 * @param date_time the date-time
 *
 * @return the string representation of the date-time
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String date_time_to_string(DateTime date_time);

/**
 * @brief Returns the current local date.
 *
 * @return the current date
 *
 * @exception DATE_TIME_ERROR if failed to get the current date
 *
 * @note This function is thread-safe on Linux.
 */
Date date_now(void);

/**
 * @brief Creates a date from its components.
 *
 * @param year the year (1-9999)
 * @param month the month (1–12)
 * @param day the day of the month
 *
 * @return the created date
 *
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 */
Date date_of(int year, int month, int day);

/**
 * @brief Parses a string in the format of YYYY-MM-DD into a date.
 *
 * @param string the string to parse
 *
 * @return the parsed date
 *
 * @exception DATE_TIME_FORMAT_ERROR if failed to parse the string
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 */
#define date_parse(string) date_parse(dispatch_string_type(string))

Date (date_parse)(struct String string);

/**
 * @brief Returns the day of the week.
 *
 * @param date the date
 *
 * @return the day of the week
 *
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 */
WeekDay date_week_day(Date date);

/**
 * @brief Returns the day of the year, starting 1.
 *
 * @param date the date
 *
 * @return the day of the year
 *
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 */
int date_year_day(Date date);

/**
 * @brief Determines whether the year of a date-time is a leap year.
 *
 * @param date the date-time
 *
 * @return true if the year is a leap year, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if the date-time value is invalid
 */
bool date_is_leap_year(Date date);

/**
 * @brief Determines whether one date is after another
 *
 * @param date the date being tested
 * @param other the date to compare against
 *
 * @return true if date is later than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date or other is invalid
 */
bool date_is_after(Date date, Date other);

/**
 * @brief Determines whether one date is before another
 *
 * @param date the date being tested
 * @param other the date to compare against
 *
 * @return true if date is earlier than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date or other is invalid
 */
bool date_is_before(Date date, Date other);

/**
 * @brief Determines whether two date are equal
 *
 * @param date the first date
 * @param other the second date
 *
 * @return true if the dates are equal, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if date or other is invalid
 */
bool date_is_equal(Date date, Date other);

/**
 * @brief Checks whether a date is valid.
 *
 * @param date the date
 *
 * @return true if valid, false otherwise
 */
bool date_is_valid(Date date);

/**
 * @brief Adds a value to a date.
 *
 * @param date the date
 * @param value the amount to add
 * @param unit the temporal unit of the amount
 *
 * @return the resulting date
 *
 * @note When adding years or months, the day is clamped to the last valid
 *       day of the resulting month if necessary.
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting date
 */
Date date_plus(Date date, int value, TimeUnit unit);

/**
 * @brief Subtracts a value from a date.
 *
 * @param date the date
 * @param value the amount to subtract
 * @param unit the temporal unit of the amount
 *
 * @return the resulting date
 *
 * @note When subtracting years or months, the day is clamped to the last valid
 *       day of the resulting month if necessary.
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting date-time
 */
Date date_minus(Date date, int value, TimeUnit unit);

/**
 * @brief Formats a date using one of the supported formats.
 *
 * @param date the date
 * @param format the format string
 *
 * @return the formatted date
 *
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 * @exception NULL_POINTER_ERROR if format is null
 * @exception DATE_TIME_FORMAT_ERROR if the format is not supported
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String date_format(Date date, const char* format);

/**
 * @brief Converts a date to its default string representation.
 *
 * @param date the date
 *
 * @return the string representation of the date
 *
 * @exception DATE_TIME_INVALID_ERROR if the date value is invalid
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String date_to_string(Date date);

/**
 * @brief Returns the current local time.
 *
 * @return the current time
 *
 * @exception DATE_TIME_ERROR if failed to get the current time
 *
 * @note This function is thread-safe on Linux.
 */
Time time_now(void);

/**
 * @brief Creates a time from its components.
 *
 * @param hour the hour (0–23)
 * @param minute the minute (0–59)
 * @param second the second (0–59)
 *
 * @return the created time
 *
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 */
Time time_of(int hour, int minute, int second);

/**
 * @brief Parses a string in the format of HH:MM:SS into a time.
 *
 * @param string the string to parse
 *
 * @return the parsed time
 *
 * @exception DATE_TIME_FORMAT_ERROR if failed to parse the string
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 */
#define time_parse(string) time_parse(dispatch_string_type(string))

Time (time_parse)(struct String string);

/**
 * @brief Determines whether one time is after another
 *
 * @param time the time being tested
 * @param other the time to compare against
 *
 * @return true if time is later than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if time or other is invalid
 */
bool time_is_after(Time time, Time other);

/**
 * @brief Determines whether one time is before another
 *
 * @param time the time being tested
 * @param other the time to compare against
 *
 * @return true if time is earlier than other, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if time or other is invalid
 */
bool time_is_before(Time time, Time other);

/**
 * @brief Determines whether two time are equal
 *
 * @param time the first time
 * @param other the second time
 *
 * @return true if the times are equal, false otherwise
 *
 * @exception DATE_TIME_INVALID_ERROR if time or other is invalid
 */
bool time_is_equal(Time time, Time other);

/**
 * @brief Checks whether a time is valid.
 *
 * @param time the time
 *
 * @return true if valid, false otherwise
 */
bool time_is_valid(Time time);

/**
 * @brief Adds a value to a time.
 *
 * @param time the time
 * @param value the amount to add
 * @param unit the temporal unit of the amount
 *
 * @return the resulting time
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting time
 */
Time time_plus(Time time, int value, TimeUnit unit);

/**
 * @brief Subtracts a value from a time.
 *
 * @param time the time
 * @param value the amount to subtract
 * @param unit the temporal unit of the amount
 *
 * @return the resulting time
 *
 * @exception ILLEGAL_ARGUMENT_ERROR if the time unit is invalid
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 * @exception DATE_TIME_ERROR if failed to calculate the resulting time-time
 */
Time time_minus(Time time, int value, TimeUnit unit);

/**
 * @brief Formats a time using one of the supported formats.
 *
 * @param time the time
 * @param format the format string
 *
 * @return the formatted time
 *
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 * @exception NULL_POINTER_ERROR if format is null
 * @exception DATE_TIME_FORMAT_ERROR if the format is not supported
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String time_format(Time time, const char* format);

/**
 * @brief Converts a time to its default string representation.
 *
 * @param time the time
 *
 * @return the string representation of the time
 *
 * @exception DATE_TIME_INVALID_ERROR if the time value is invalid
 * @exception MEMORY_ALLOCATION_ERROR if string memory allocation fails
 */
String time_to_string(Time time);

#endif
