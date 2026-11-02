#include "core/math.h"

#include "core/limits.h"
#include "core/errors.h"

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
}

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