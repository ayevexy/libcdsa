#include "core/math.h"

#include "core/limits.h"
#include "core/errors.h"
#include <math.h>

#define DEFINE_SIGNED_MATH(T, suffix, min, max)                             \
                                                                            \
T math_add_##suffix(T a, T b) {                                             \
    if ((b > 0 && a > (max) - b) || (b < 0 && a < (min) - b)) {             \
        set_error(ARITHMETIC_ERROR, "addition overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a + b;                                                           \
}                                                                           \
                                                                            \
T math_sub_##suffix(T a, T b) {                                             \
    if ((b < 0 && a > (max) + b) || (b > 0 && a < (min) + b)) {             \
        set_error(ARITHMETIC_ERROR, "subtraction overflow");                \
        return 0;                                                           \
    }                                                                       \
    return a - b;                                                           \
}                                                                           \
                                                                            \
T math_mul_##suffix(T a, T b) {                                             \
    if (a > 0) {                                                            \
        if ((b > 0 && a > (max) / b) || (b < 0 && b < (min) / a))           \
            goto overflow;                                                  \
    } else if (a < 0) {                                                     \
        if ((b > 0 && a < (min) / b) || (b < 0 && a < (max) / b))           \
            goto overflow;                                                  \
    }                                                                       \
    return a * b;                                                           \
                                                                            \
    overflow:                                                               \
        set_error(ARITHMETIC_ERROR, "multiplication overflow");             \
        return 0;                                                           \
}                                                                           \
                                                                            \
T math_div_##suffix(T a, T b) {                                             \
    if (b == 0) {                                                           \
        set_error(ARITHMETIC_ERROR, "division by zero");                    \
        return 0;                                                           \
    }                                                                       \
    if (a == (min) && b == -1) {                                            \
        set_error(ARITHMETIC_ERROR, "division overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a / b;                                                           \
}                                                                           \
                                                                            \
T math_gcd_##suffix(T a, T b) {                                             \
    a = math_abs_##suffix(a), b = math_abs_##suffix(b);                     \
    while (b != 0) {                                                        \
        T remainder = a % b;                                                \
        a = b;                                                              \
        b = remainder;                                                      \
    }                                                                       \
    return a;                                                               \
}                                                                           \
                                                                            \
T math_lcm_##suffix(T a, T b) {                                             \
    a = math_abs_##suffix(a), b = math_abs_##suffix(b);                     \
    if (a == 0 || b == 0) {                                                 \
        return 0;                                                           \
    }                                                                       \
    return (a / math_gcd_##suffix(a, b)) * b;                               \
}                                                                           \
                                                                            \
T math_factorial_##suffix(T number) {                                       \
    if (number < 0) {                                                       \
        set_error(ARITHMETIC_ERROR, "factorial negative");                  \
        return 0;                                                           \
    }                                                                       \
    T result = 1;                                                           \
                                                                            \
    for (T i = 2; i <= number; i++) {                                       \
        if (result > max / i) {                                             \
            set_error(ARITHMETIC_ERROR, "factorial overflow");              \
            return 0;                                                       \
        }                                                                   \
        result *= i;                                                        \
    }                                                                       \
    return result;                                                          \
}                                                                           \
                                                                            \
bool math_is_prime_##suffix(T number) {                                     \
    if (number < 2)       return false;                                     \
    if (number == 2)      return true;                                      \
    if (number % 2 == 0)  return false;                                     \
                                                                            \
    for (T i = 3; i <= number / i; i += 2) {                                \
        if (number % i == 0) return false;                                  \
    }                                                                       \
    return true;                                                            \
}                                                                           \
                                                                            \
T math_abs_##suffix(T number) {                                             \
    return number < 0 ? -number : number;                                   \
}                                                                           \
                                                                            \
T math_min_##suffix(T a, T b) {                                             \
    return a < b ? a : b;                                                   \
}                                                                           \
                                                                            \
T math_max_##suffix(T a, T b) {                                             \
    return a > b ? a : b;                                                   \
}                                                                           \

#define DEFINE_UNSIGNED_MATH(T, suffix, max)                                \
                                                                            \
T math_add_##suffix(T a, T b) {                                             \
    if (a > (max) - b) {                                                    \
        set_error(ARITHMETIC_ERROR, "addition overflow");                   \
        return 0;                                                           \
    }                                                                       \
    return a + b;                                                           \
}                                                                           \
                                                                            \
T math_sub_##suffix(T a, T b) {                                             \
    if (a < b) {                                                            \
        set_error(ARITHMETIC_ERROR, "subtraction underflow");               \
        return 0;                                                           \
    }                                                                       \
    return a - b;                                                           \
}                                                                           \
                                                                            \
T math_mul_##suffix(T a, T b) {                                             \
    if (b != 0 && a > (max) / b) {                                          \
        set_error(ARITHMETIC_ERROR, "multiplication overflow");             \
        return 0;                                                           \
    }                                                                       \
    return a * b;                                                           \
}                                                                           \
                                                                            \
T math_div_##suffix(T a, T b) {                                             \
    if (b == 0) {                                                           \
        set_error(ARITHMETIC_ERROR, "division by zero");                    \
        return 0;                                                           \
    }                                                                       \
    return a / b;                                                           \
}                                                                           \
                                                                            \
T math_gcd_##suffix(T a, T b) {                                             \
    while (b != 0) {                                                        \
        T remainder = a % b;                                                \
        a = b;                                                              \
        b = remainder;                                                      \
    }                                                                       \
    return a;                                                               \
}                                                                           \
                                                                            \
T math_lcm_##suffix(T a, T b) {                                             \
    if (a == 0 || b == 0) {                                                 \
        return 0;                                                           \
    }                                                                       \
    return (a / math_gcd_##suffix(a, b)) * b;                               \
}                                                                           \
                                                                            \
T math_factorial_##suffix(T number) {                                       \
    T result = 1;                                                           \
                                                                            \
    for (T i = 2; i <= number; i++) {                                       \
        if (result > max / i) {                                             \
            set_error(ARITHMETIC_ERROR, "factorial overflow");              \
            return 0;                                                       \
        }                                                                   \
        result *= i;                                                        \
    }                                                                       \
    return result;                                                          \
}                                                                           \
                                                                            \
bool math_is_prime_##suffix(T number) {                                     \
    if (number < 2)       return false;                                     \
    if (number == 2)      return true;                                      \
    if (number % 2 == 0)  return false;                                     \
                                                                            \
    for (T i = 3; i <= number / i; i += 2) {                                \
        if (number % i == 0) return false;                                  \
    }                                                                       \
    return true;                                                            \
}                                                                           \
                                                                            \
T math_abs_##suffix(T number) {                                             \
    return number;                                                          \
}                                                                           \
                                                                            \
T math_min_##suffix(T a, T b) {                                             \
    return a < b ? a : b;                                                   \
}                                                                           \
                                                                            \
T math_max_##suffix(T a, T b) {                                             \
    return a > b ? a : b;                                                   \
}

#if CHAR_MIN < 0
DEFINE_SIGNED_MATH(char, char, CHAR_MIN_VALUE, CHAR_MAX_VALUE)
#else
DEFINE_UNSIGNED_MATH(char, char, CHAR_MAX_VALUE)
#endif

DEFINE_SIGNED_MATH(schar, schar, SCHAR_MIN_VALUE, SCHAR_MAX_VALUE)
DEFINE_UNSIGNED_MATH(uchar, uchar, UCHAR_MAX_VALUE)

DEFINE_SIGNED_MATH(short, short, SHORT_MIN_VALUE, SHORT_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ushort, ushort, USHORT_MAX_VALUE)

DEFINE_SIGNED_MATH(int, int, INT_MIN_VALUE, INT_MAX_VALUE)
DEFINE_UNSIGNED_MATH(uint, uint, UINT_MAX_VALUE)

DEFINE_SIGNED_MATH(long, long, LONG_MIN_VALUE, LONG_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ulong, ulong, ULONG_MAX_VALUE)

DEFINE_SIGNED_MATH(llong, llong, LLONG_MIN_VALUE, LLONG_MAX_VALUE)
DEFINE_UNSIGNED_MATH(ullong, ullong, ULLONG_MAX_VALUE)

float math_abs_float(float number) {
    return fabsf(number);
}

float math_min_float(float a, float b) {
    return fminf(a, b);
}

float math_max_float(float a, float b) {
    return fmaxf(a, b);
}

float math_sign_float(float number) {
    if (number > 0) return 1;
    if (number < 0) return -1;
    return 0;
}

float math_round_float(float number) {
    return roundf(number);
}

float math_floor_float(float number) {
    return floorf(number);
}

float math_ceil_float(float number) {
    return ceilf(number);
}

float math_trunc_float(float number) {
    return truncf(number);
}

double math_abs_double(double number) {
    return fabs(number);
}

double math_min_double(double a, double b) {
    return fmin(a, b);
}

double math_max_double(double a, double b) {
    return fmax(a, b);
}

double math_sign_double(double number) {
    if (number > 0) return 1;
    if (number < 0) return -1;
    return 0;
}

double math_round_double(double number) {
    return round(number);
}

double math_floor_double(double number) {
    return floor(number);
}

double math_ceil_double(double number) {
    return ceil(number);
}

double math_trunc_double(double number) {
    return trunc(number);
}

ldouble math_abs_ldouble(ldouble number) {
    return fabsl(number);
}

ldouble math_min_ldouble(ldouble a, ldouble b) {
    return fminl(a, b);
}

ldouble math_max_ldouble(ldouble a, ldouble b) {
    return fmaxl(a, b);
}

ldouble math_sign_ldouble(ldouble number) {
    if (number > 0) return 1;
    if (number < 0) return -1;
    return 0;
}

ldouble math_round_ldouble(ldouble number) {
    return roundl(number);
}

ldouble math_floor_ldouble(ldouble number) {
    return floorl(number);
}

ldouble math_ceil_ldouble(ldouble number) {
    return ceill(number);
}

ldouble math_trunc_ldouble(ldouble number) {
    return truncl(number);
}