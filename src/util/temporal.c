#include "temporal.h"

#include "core/constraints.h"
#include <time.h>
#include <stdio.h>

const Error DATE_TIME_ERROR          = ERROR("DATE_TIME_ERROR", "date/time calculation failed");
const Error DATE_TIME_INVALID_ERROR  = ERROR("DATE_TIME_INVALID_ERROR", "invalid date/time value");
const Error DATE_TIME_FORMAT_ERROR   = ERROR("DATE_TIME_FORMAT_ERROR", "invalid date/time format");

#define require_valid_date_time(date_time) \
    require(date_time_is_valid(date_time), DATE_TIME_INVALID_ERROR)

static bool is_leap_year(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static int days_in_month(int year, int month) {
    constexpr int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
    if (month == FEBRUARY && is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

DateTime date_time_now(void) {
    const time_t now = time(nullptr);

    if (now == -1) {
        set_error(DATE_TIME_ERROR, "failed to get current date time");
        return (DateTime) {};
    }
#ifdef __linux__
    const struct tm* local_time = localtime_r(&now, &(struct tm){});
#else
    const struct tm* local_time = localtime(&now);
#endif
    if (!local_time) {
        set_error(DATE_TIME_ERROR, "failed to get current date time");
        return (DateTime) {};
    }

    return (DateTime) {
        .year = local_time->tm_year + 1900,
        .month = local_time->tm_mon + 1,
        .day = local_time->tm_mday,
        .hour = local_time->tm_hour,
        .minute = local_time->tm_min,
        .second = local_time->tm_sec
    };
}

#define require_is_year(year)             require(year >= 1 && year <= 9999, DATE_TIME_INVALID_ERROR, "invalid year")
#define require_is_month(month)           require(month >= 1 && month <= 12, DATE_TIME_INVALID_ERROR, "invalid month")
#define require_is_day(day, month, year)  require(day >= 1 && day <= days_in_month(year, month), DATE_TIME_INVALID_ERROR, "invalid day")
#define require_is_hour(hour)             require(hour >= 0 && hour <= 23, DATE_TIME_INVALID_ERROR, "invalid hour")
#define require_is_minute(minute)         require(minute >= 0 && minute <= 59, DATE_TIME_INVALID_ERROR, "invalid minute")
#define require_is_second(second)         require(second >= 0 && second <= 59, DATE_TIME_INVALID_ERROR, "invalid second")

DateTime date_time_of(int year, int month, int day, int hour, int minute, int second) {
    if (require_is_year(year) || require_is_month(month) || require_is_day(day, month, year)) return (DateTime) {};
    if (require_is_hour(hour) || require_is_minute(minute) || require_is_second(second)) return (DateTime) {};

    return (DateTime) {
        .year = year, .month = month, .day = day,
        .hour = hour, .minute = minute, .second = second
    };
}

DateTime date_time_from(Date date, Time time) {
    return date_time_of(
        date.year, date.month, date.day,
        time.hour, time.minute, time.second
    );
}

DateTime (date_time_parse)(StringView string) {
    int year, month, day, hour, minute, second, consumed;

    const int count = sscanf(string.data, "%d-%d-%dT%d:%d:%d%n",
        &year, &month, &day, &hour, &minute, &second, &consumed
    );

    if (count != 6 || consumed != string.length) {
        set_error(DATE_TIME_FORMAT_ERROR, "failed to parse date time string '%s'", string.data);
        return (DateTime) {};
    }

    return date_time_of(year, month, day, hour, minute, second);
}

WeekDay date_time_week_day(DateTime date_time) {
    if (require_valid_date_time(date_time)) return -1;

    int year = date_time.year;
    int month = date_time.month;

    if (month < 3) {
        month += 12;
        year--;
    }

    const int century = year / 100;
    const int year_of_century = year % 100;

    const int day = (
        date_time.day
        + (13 * (month + 1)) / 5
        + year_of_century
        + year_of_century / 4
        + century / 4
        + 5 * century
    ) % 7;

    return (WeekDay) ((day + 6) % 7 + 1);
}

int date_time_year_day(DateTime date_time) {
    if (require_valid_date_time(date_time)) return -1;

    int year_day = date_time.day;

    for (int month = 1; month < date_time.month; month++) {
        year_day += days_in_month(date_time.year, month);
    }

    return year_day;
}

bool date_time_is_leap_year(DateTime date_time) {
    if (require_valid_date_time(date_time)) return false;

    return is_leap_year(date_time.year);
}

bool date_time_is_after(DateTime date_time, DateTime other) {
    if (require_valid_date_time(date_time)) return false;
    if (require_valid_date_time(other)) return false;

    if (date_time.year != other.year)      return date_time.year > other.year;
    if (date_time.month != other.month)    return date_time.month > other.month;
    if (date_time.day != other.day)        return date_time.day > other.day;
    if (date_time.hour != other.hour)      return date_time.hour > other.hour;
    if (date_time.minute != other.minute)  return date_time.minute > other.minute;

    return date_time.second > other.second;
}

bool date_time_is_before(DateTime date_time, DateTime other) {
    if (require_valid_date_time(date_time)) return false;
    if (require_valid_date_time(other)) return false;

    if (date_time.year != other.year)      return date_time.year < other.year;
    if (date_time.month != other.month)    return date_time.month < other.month;
    if (date_time.day != other.day)        return date_time.day < other.day;
    if (date_time.hour != other.hour)      return date_time.hour < other.hour;
    if (date_time.minute != other.minute)  return date_time.minute < other.minute;

    return date_time.second < other.second;
}

bool date_time_is_equal(DateTime date_time, DateTime other) {
    if (require_valid_date_time(date_time)) return false;
    if (require_valid_date_time(other)) return false;

    return date_time.year == other.year
        && date_time.month == other.month
        && date_time.day == other.day
        && date_time.hour == other.hour
        && date_time.minute == other.minute
        && date_time.second == other.second;
}

bool date_time_is_valid(DateTime date_time) {
    if (date_time.year < 1 || date_time.year > 9999) return false;
    if (date_time.month < 1 || date_time.month > 12) return false;
    if (date_time.day < 1 || date_time.day > days_in_month(date_time.year, date_time.month)) return false;
    if (date_time.hour < 0 || date_time.hour > 23) return false;
    if (date_time.minute < 0 || date_time.minute > 59) return false;
    if (date_time.second < 0 || date_time.second > 59) return false;

    return true;
}

DateTime date_time_plus(DateTime date_time, int value, TimeUnit unit) {
    if (require_valid_date_time(date_time)) return (DateTime) {};

    switch (unit) {
        case YEAR:    date_time.year += value;     break;
        case MONTH:   date_time.month += value;    break;
        case WEEK:    date_time.day += value * 7;  break;
        case DAY:     date_time.day += value;      break;
        case HOUR:    date_time.hour += value;     break;
        case MINUTE:  date_time.minute += value;   break;
        case SECOND:  date_time.second += value;   break;
        default:      set_error(ILLEGAL_ARGUMENT_ERROR, "invalid time unit"); return (DateTime) {};
    }

    if (unit == YEAR || unit == MONTH) {
        while (date_time.month > 12) {
            date_time.month -= 12;
            date_time.year++;
        }
        while (date_time.month < 1) {
            date_time.month += 12;
            date_time.year--;
        }

        const int max_day = days_in_month(date_time.year, date_time.month);

        if (date_time.day > max_day) {
            date_time.day = max_day;
        }
    }

    struct tm local_time = {
        .tm_year = date_time.year - 1900,
        .tm_mon = date_time.month - 1,
        .tm_mday = date_time.day,
        .tm_hour = date_time.hour,
        .tm_min = date_time.minute,
        .tm_sec = date_time.second,
        .tm_isdst = -1
    };

    if (mktime(&local_time) == -1) {
        set_error(DATE_TIME_ERROR, "failed to calculate resulting date/time");
        return (DateTime) {};
    }

    return date_time_of(
        local_time.tm_year + 1900,
        local_time.tm_mon + 1,
        local_time.tm_mday,
        local_time.tm_hour,
        local_time.tm_min,
        local_time.tm_sec
    );
}

DateTime date_time_minus(DateTime date_time, int value, TimeUnit unit) {
    return date_time_plus(date_time, (-1) * value, unit);
}

static int64 date_time_to_seconds(DateTime date_time) {
    int year = date_time.year;
    const int month = date_time.month;

    year -= month <= 2;

    const int era = year / 400;
    const int year_of_era = year - era * 400;
    const int month_of_year = month + (month > 2 ? -3 : 9);

    const int day_of_year = (153 * month_of_year + 2) / 5 + date_time.date.day - 1;

    const int day_of_era = year_of_era * 365 + year_of_era / 4
                         - year_of_era / 100 + day_of_year;

    return (int64) era * 146097 * 86400
         + (int64) day_of_era * 86400
         + date_time.hour * 3600
         + date_time.minute * 60
         + date_time.second;
}

static int date_time_compare(DateTime date_time, DateTime other) {
    if (date_time_is_after(date_time, other)) return 1;
    if (date_time_is_before(date_time, other)) return -1;
    if (date_time_is_equal(date_time, other)) return 0;

    return 0;
}

long date_time_difference(DateTime date_time, DateTime other, TimeUnit unit) {
    if (require_valid_date_time(date_time)) return -1;
    if (require_valid_date_time(other)) return -1;

    if (unit == YEAR || unit == MONTH) {
        int months = (other.year - date_time.year) * 12 + other.month - date_time.month;

        const DateTime adjusted = date_time_plus(date_time, months, MONTH);
        const int comparison = date_time_compare(adjusted, other);

        if ((months > 0 && comparison > 0) || (months < 0 && comparison < 0)) {
            months += months > 0 ? -1 : 1;
        }
        return unit == YEAR ? months / 12 : months;
    }

    const int64 seconds = date_time_to_seconds(other) - date_time_to_seconds(date_time);

    switch (unit) {
        case WEEK:    return seconds / 604800;
        case DAY:     return seconds / 86400;
        case HOUR:    return seconds / 3600;
        case MINUTE:  return seconds / 60;
        case SECOND:  return seconds;
        default:      set_error(ILLEGAL_ARGUMENT_ERROR, "invalid time unit"); return 0;
    }
}

String date_time_format(DateTime date_time, const char* format) {
    if (require_valid_date_time(date_time)) return nullptr;
    if (require_non_null(format)) return nullptr;

    static const char* const date_time_formats[] = {
        ISO_8601_DATE_TIME_FORMAT,
        ISO_DATE_TIME_FORMAT,
        US_DATE_TIME_FORMAT,
        EUROPEAN_DATE_TIME_FORMAT,
        ASCTIME_DATE_TIME_FORMAT,
        FULL_DATE_TIME_FORMAT,

        US_AM_PM_DATE_TIME_FORMAT,
        EUROPEAN_AM_PM_DATE_TIME_FORMAT,
        FULL_AM_PM_DATE_TIME_FORMAT,

        ISO_DATE_FORMAT,
        US_DATE_FORMAT,
        EUROPEAN_DATE_FORMAT,
        ASCTIME_DATE_FORMAT,
        FULL_DATE_FORMAT,

        ISO_TIME_FORMAT,
        US_TIME_FORMAT
    };

    bool found = false;
    for (int i = 0; i < (int) (sizeof(date_time_formats) / sizeof(*date_time_formats)); i++) {
        if (string_compare(date_time_formats[i], format) == 0) {
            found = true;
            break;
        }
    }

    if (!found) {
        set_error(DATE_TIME_FORMAT_ERROR, "invalid date time format");
        return nullptr;
    }

    struct tm local_time = {
        .tm_year = date_time.year - 1900,
        .tm_mon = date_time.month - 1,
        .tm_mday = date_time.day,
        .tm_hour = date_time.hour,
        .tm_min = date_time.minute,
        .tm_sec = date_time.second,
        .tm_wday = date_time_week_day(date_time) - 1,
        .tm_yday = date_time_year_day(date_time) - 1
    };

    constexpr int BUFFER_SIZE = 64;
    char buffer[BUFFER_SIZE];

    if (strftime(buffer, BUFFER_SIZE, format, &local_time) == 0) {
        // Unreachable with the predefined formats and buffer size
        return string_ref("");
    }

    return string_new(buffer);
}

String date_time_to_string(DateTime date_time) {
    if (require_valid_date_time(date_time)) return nullptr;

    return string_format("%04d-%02d-%02dT%02d:%02d:%02d",
        date_time.year, date_time.month, date_time.day,
        date_time.hour, date_time.minute, date_time.second
    );
}

Date date_now(void) {
    return date_time_now().date;
}

Date date_of(int year, int month, int day) {
    return date_time_of(year, month, day, 0, 0, 0).date;
}

Date (date_parse)(StringView string) {
    int year, month, day, consumed;

    const int count = sscanf(string.data, "%d-%d-%d%n", &year, &month, &day, &consumed);

    if (count != 3 || consumed != string.length) {
        set_error(DATE_TIME_FORMAT_ERROR, "failed to parse date time string '%s'", string.data);
        return (Date) {};
    }

    return date_time_of(year, month, day, 0, 0, 0).date;
}

WeekDay date_week_day(Date date) {
    return date_time_week_day(date_time_from(date, TIME_MIDNIGHT));
}

int date_year_day(Date date) {
    return date_time_year_day(date_time_from(date, TIME_MIDNIGHT));
}

bool date_is_leap_year(Date date) {
    return date_time_is_leap_year(date_time_from(date, TIME_MIDNIGHT));
}

bool date_is_after(Date date, Date other) {
    return date_time_is_after(
        date_time_from(date, TIME_MIDNIGHT),
        date_time_from(other, TIME_MIDNIGHT)
    );
}

bool date_is_before(Date date, Date other) {
    return date_time_is_before(
        date_time_from(date, TIME_MIDNIGHT),
        date_time_from(other, TIME_MIDNIGHT)
    );
}

bool date_is_equal(Date date, Date other) {
    return date_time_is_equal(
        date_time_from(date, TIME_MIDNIGHT),
        date_time_from(other, TIME_MIDNIGHT)
    );
}

bool date_is_valid(Date date) {
    return date_time_is_valid(date_time_from(date, TIME_MIDNIGHT));
}

#define require_unit_date(unit) \
    require(unit == YEAR || unit == MONTH || unit == WEEK || unit == DAY, ILLEGAL_ARGUMENT_ERROR, "unsupported unit %d", unit)

Date date_plus(Date date, int value, TimeUnit unit) {
    if (require_unit_date(unit)) return (Date) {};

    return date_time_plus(date_time_from(date, TIME_MIDNIGHT), value, unit).date;
}

Date date_minus(Date date, int value, TimeUnit unit) {
    if (require_unit_date(unit)) return (Date) {};

    return date_time_minus(date_time_from(date, TIME_MIDNIGHT), value, unit).date;
}

long date_difference(Date date, Date other, TimeUnit unit) {
    if (require_unit_date(unit)) return -1;

    return date_time_difference(
        date_time_from(date, TIME_MIDNIGHT),
        date_time_from(other, TIME_MIDNIGHT),
        unit
    );
}

String date_format(Date date, const char* format) {
    return date_time_format(date_time_from(date, TIME_MIDNIGHT), format);
}

String date_to_string(Date date) {
    if (require_valid_date_time(date_time_from(date, TIME_MIDNIGHT))) return nullptr;

    return string_format("%04d-%02d-%02d", date.year, date.month, date.day);
}

Time time_now(void) {
    return date_time_now().time;
}

Time time_of(int hour, int minute, int second) {
    return date_time_of(1, 1, 1, hour, minute, second).time;
}

Time (time_parse)(StringView string) {
    int hour, minute, second, consumed;

    const int count = sscanf(string.data, "%d:%d:%d%n", &hour, &minute, &second, &consumed);

    if (count != 3 || consumed != string.length) {
        set_error(DATE_TIME_FORMAT_ERROR, "failed to parse date time string '%s'", string.data);
        return (Time) {};
    }

    return date_time_of(1, 1, 1, hour, minute, second).time;
}

bool time_is_after(Time time, Time other) {
    return date_time_is_after(
        date_time_from(DATE_MIN, time),
        date_time_from(DATE_MIN, other)
    );
}

bool time_is_before(Time time, Time other) {
    return date_time_is_before(
        date_time_from(DATE_MIN, time),
        date_time_from(DATE_MIN, other)
    );
}

bool time_is_equal(Time time, Time other) {
    return date_time_is_equal(
        date_time_from(DATE_MIN, time),
        date_time_from(DATE_MIN, other)
    );
}

bool time_is_valid(Time time) {
    return date_time_is_valid(date_time_from(DATE_MIN, time));
}

#define require_unit_time(unit) \
    require(unit == HOUR || unit == MINUTE || unit == SECOND, ILLEGAL_ARGUMENT_ERROR, "unsupported unit %d", unit)

Time time_plus(Time time, int value, TimeUnit unit) {
    if (require_unit_time(unit)) return (Time) {};

    return date_time_plus(date_time_from(DATE_MIN, time), value, unit).time;
}

Time time_minus(Time time, int value, TimeUnit unit) {
    if (require_unit_time(unit)) return (Time) {};

    return date_time_minus(date_time_from(DATE_MIN, time), value, unit).time;
}

long time_difference(Time time, Time other, TimeUnit unit) {
    if (require_unit_time(unit)) return -1;

    return date_time_difference(
        date_time_from(DATE_MIN, time),
        date_time_from(DATE_MIN, other),
        unit
    );
}

String time_format(Time time, const char* format) {
    return date_time_format(date_time_from(DATE_MIN, time), format);
}

String time_to_string(Time time) {
    if (require_valid_date_time(date_time_from(DATE_MIN, time))) return nullptr;

    return string_format("%02d:%02d:%02d", time.hour, time.minute, time.second);
}