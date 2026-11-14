#include "core/math.h"

#include "core/limits.h"
#include "core/errors.h"

#include <math.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/* Signed integer functions                                                   */
/* -------------------------------------------------------------------------- */

#define DEFINE_MATH_ADD_EXACT_SIGNED(T, suffix, min, max)                   \
    T math_add_exact_##suffix(T a, T b) {                                   \
        if ((b > 0 && a > (max) - b) || (b < 0 && a < (min) - b)) {         \
            set_error(ARITHMETIC_ERROR, "addition overflow");               \
            return 0;                                                       \
        }                                                                   \
        return a + b;                                                       \
    }

#define DEFINE_MATH_SUB_EXACT_SIGNED(T, suffix, min, max)                   \
    T math_sub_exact_##suffix(T a, T b) {                                   \
        if ((b < 0 && a > (max) + b) || (b > 0 && a < (min) + b)) {         \
            set_error(ARITHMETIC_ERROR, "subtraction overflow");            \
            return 0;                                                       \
        }                                                                   \
        return a - b;                                                       \
    }

#define DEFINE_MATH_MUL_EXACT_SIGNED(T, suffix, min, max)                   \
    T math_mul_exact_##suffix(T a, T b) {                                   \
        if (a > 0) {                                                        \
            if ((b > 0 && a > (max) / b) || (b < 0 && b < (min) / a))       \
                goto overflow;                                              \
        } else if (a < 0) {                                                 \
            if ((b > 0 && a < (min) / b) || (b < 0 && a < (max) / b))       \
                goto overflow;                                              \
        }                                                                   \
        return a * b;                                                       \
        overflow:                                                           \
            set_error(ARITHMETIC_ERROR, "multiplication overflow");         \
            return 0;                                                       \
    }

#define DEFINE_MATH_DIV_EXACT_SIGNED(T, suffix, min)                        \
    T math_div_exact_##suffix(T a, T b) {                                   \
        if (b == 0) {                                                       \
            set_error(ARITHMETIC_ERROR, "division by zero");                \
            return 0;                                                       \
        }                                                                   \
        if (a == (min) && b == -1) {                                        \
            set_error(ARITHMETIC_ERROR, "division overflow");               \
            return 0;                                                       \
        }                                                                   \
        return a / b;                                                       \
    }

#define DEFINE_MATH_GCD_SIGNED(T, suffix)                                   \
    T math_gcd_##suffix(T a, T b) {                                         \
        a = math_abs_##suffix(a);                                           \
        b = math_abs_##suffix(b);                                           \
        while (b != 0) {                                                    \
            T remainder = a % b;                                            \
            a = b;                                                          \
            b = remainder;                                                  \
        }                                                                   \
        return a;                                                           \
    }

#define DEFINE_MATH_LCM_SIGNED(T, suffix)                                   \
    T math_lcm_##suffix(T a, T b) {                                         \
        a = math_abs_##suffix(a);                                           \
        b = math_abs_##suffix(b);                                           \
        if (a == 0 || b == 0) return 0;                                     \
        return (a / math_gcd_##suffix(a, b)) * b;                           \
    }

#define DEFINE_MATH_FACTORIAL_SIGNED(T, suffix, min, max)                   \
    T math_factorial_##suffix(T n) {                                        \
        if (n < 0) return min;                                              \
        T result = 1;                                                       \
        for (T i = 2; i <= n; i++) {                                        \
            if (result > (max) / i) return max;                             \
            result *= i;                                                    \
        }                                                                   \
        return result;                                                      \
    }

#define DEFINE_MATH_COMBINATIONS_SIGNED(T, suffix, max)                     \
    T math_combinations_##suffix(T n, T r) {                                \
        if (r > n || n < 0 || r < 0) return 0;                              \
        if (r > n - r) r = n - r;                                           \
        T result = 1;                                                       \
        for (T i = 1; i <= r; i++) {                                        \
            T numerator = n - r + i;                                        \
            T denominator = i;                                              \
            T divisor = math_gcd_##suffix(numerator, denominator);          \
            numerator /= divisor;                                           \
            denominator /= divisor;                                         \
            divisor = math_gcd_##suffix(result, denominator);               \
            result /= divisor;                                              \
            denominator /= divisor;                                         \
            if (result > (max) / numerator) return max;                     \
            result *= numerator;                                            \
            result /= denominator;                                          \
        }                                                                   \
        return result;                                                      \
    }

#define DEFINE_MATH_PERMUTATIONS_SIGNED(T, suffix, max)                     \
    T math_permutations_##suffix(T n, T r) {                                \
        if (r > n || n < 0 || r < 0) return 0;                              \
        T result = 1;                                                       \
        for (T i = 0; i < r; i++) {                                         \
            T factor = n - i;                                               \
            if (result > (max) / factor) return max;                        \
            result *= factor;                                               \
        }                                                                   \
        return result;                                                      \
    }

/* -------------------------------------------------------------------------- */
/* Unsigned integer functions                                                 */
/* -------------------------------------------------------------------------- */

#define DEFINE_MATH_ADD_EXACT_UNSIGNED(T, suffix, max)                      \
    T math_add_exact_##suffix(T a, T b) {                                   \
        if (a > (max) - b) {                                                \
            set_error(ARITHMETIC_ERROR, "addition overflow");               \
            return 0;                                                       \
        }                                                                   \
        return a + b;                                                       \
    }

#define DEFINE_MATH_SUB_EXACT_UNSIGNED(T, suffix)                           \
    T math_sub_exact_##suffix(T a, T b) {                                   \
        if (a < b) {                                                        \
            set_error(ARITHMETIC_ERROR, "subtraction underflow");           \
            return 0;                                                       \
        }                                                                   \
        return a - b;                                                       \
    }

#define DEFINE_MATH_MUL_EXACT_UNSIGNED(T, suffix, max)                      \
    T math_mul_exact_##suffix(T a, T b) {                                   \
        if (b != 0 && a > (max) / b) {                                      \
            set_error(ARITHMETIC_ERROR, "multiplication overflow");         \
            return 0;                                                       \
        }                                                                   \
        return a * b;                                                       \
    }

#define DEFINE_MATH_DIV_EXACT_UNSIGNED(T, suffix)                           \
    T math_div_exact_##suffix(T a, T b) {                                   \
        if (b == 0) {                                                       \
            set_error(ARITHMETIC_ERROR, "division by zero");                \
            return 0;                                                       \
        }                                                                   \
        return a / b;                                                       \
    }

#define DEFINE_MATH_GCD_UNSIGNED(T, suffix)                                 \
    T math_gcd_##suffix(T a, T b) {                                         \
        while (b != 0) {                                                    \
            T remainder = a % b;                                            \
            a = b;                                                          \
            b = remainder;                                                  \
        }                                                                   \
        return a;                                                           \
    }

#define DEFINE_MATH_LCM_UNSIGNED(T, suffix)                                 \
    T math_lcm_##suffix(T a, T b) {                                         \
        if (a == 0 || b == 0) return 0;                                     \
        return (a / math_gcd_##suffix(a, b)) * b;                           \
    }

#define DEFINE_MATH_FACTORIAL_UNSIGNED(T, suffix, max)                      \
    T math_factorial_##suffix(T n) {                                        \
        T result = 1;                                                       \
        for (T i = 2; i <= n; i++) {                                        \
            if (result > (max) / i) return max;                             \
            result *= i;                                                    \
        }                                                                   \
        return result;                                                      \
    }

#define DEFINE_MATH_COMBINATIONS_UNSIGNED(T, suffix, max)                   \
    T math_combinations_##suffix(T n, T r) {                                \
        if (r > n) return 0;                                                \
        if (r > n - r) r = n - r;                                           \
        T result = 1;                                                       \
        for (T i = 1; i <= r; i++) {                                        \
            T numerator = n - r + i;                                        \
            T denominator = i;                                              \
            T divisor = math_gcd_##suffix(numerator, denominator);          \
            numerator /= divisor;                                           \
            denominator /= divisor;                                         \
            divisor = math_gcd_##suffix(result, denominator);               \
            result /= divisor;                                              \
            denominator /= divisor;                                         \
            if (result > (max) / numerator) return max;                     \
            result *= numerator;                                            \
            result /= denominator;                                          \
        }                                                                   \
        return result;                                                      \
    }

#define DEFINE_MATH_PERMUTATIONS_UNSIGNED(T, suffix, max)                   \
    T math_permutations_##suffix(T n, T r) {                                \
        if (r > n) return 0;                                                \
        T result = 1;                                                       \
        for (T i = 0; i < r; i++) {                                         \
            T factor = n - i;                                               \
            if (result > (max) / factor) return max;                        \
            result *= factor;                                               \
        }                                                                   \
        return result;                                                      \
    }

/* -------------------------------------------------------------------------- */
/* Common integer functions                                                   */
/* -------------------------------------------------------------------------- */

#define DEFINE_MATH_IS_PRIME(T, suffix)                                     \
    bool math_is_prime_##suffix(T n) {                                      \
        if (n < 2) return false;                                            \
        if (n == 2) return true;                                            \
        if (n % 2 == 0) return false;                                       \
        for (T i = 3; i <= n / i; i += 2) {                                 \
            if (n % i == 0) return false;                                   \
        }                                                                   \
        return true;                                                        \
    }

#define DEFINE_MATH_IS_POWER_OF_TWO_SIGNED(T, suffix)                       \
    bool math_is_power_of_two_##suffix(T n) {                               \
        return n > 0 && (n & (n - 1)) == 0;                                 \
    }

#define DEFINE_MATH_IS_POWER_OF_TWO_UNSIGNED(T, suffix)                     \
    bool math_is_power_of_two_##suffix(T n) {                               \
        return n != 0 && (n & (n - 1)) == 0;                                \
    }

#define DEFINE_MATH_ABS_SIGNED(T, suffix)                                   \
    T math_abs_##suffix(T n) {                                              \
        return n < 0 ? -n : n;                                              \
    }

#define DEFINE_MATH_ABS_UNSIGNED(T, suffix)                                 \
    T math_abs_##suffix(T n) {                                              \
        return n;                                                           \
    }

#define DEFINE_MATH_MIN_INTEGER(T, suffix)                                  \
    T math_min_##suffix(T a, T b) {                                         \
        return a < b ? a : b;                                               \
    }

#define DEFINE_MATH_MAX_INTEGER(T, suffix)                                  \
    T math_max_##suffix(T a, T b) {                                         \
        return a > b ? a : b;                                               \
    }

/* -------------------------------------------------------------------------- */
/* Floating-point functions                                                   */
/* -------------------------------------------------------------------------- */

#define DEFINE_MATH_POW(T, suffix, t)                                       \
    T math_pow_##suffix(T base, T exponent) {                               \
        return pow##t(base, exponent);                                      \
    }

#define DEFINE_MATH_LOG_NATURAL(T, suffix, t)                               \
    T math_log_natural_##suffix(T n) {                                      \
        return log##t(n);                                                   \
    }

#define DEFINE_MATH_LOG_BASE(T, suffix, t)                                  \
    T math_log_base_##suffix(T n, T base) {                                 \
        return log##t(n) / log##t(base);                                    \
    }

#define DEFINE_MATH_LOG2(T, suffix, t)                                      \
    T math_log2_##suffix(T n) {                                             \
        return log2##t(n);                                                  \
    }

#define DEFINE_MATH_LOG10(T, suffix, t)                                     \
    T math_log10_##suffix(T n) {                                            \
        return log10##t(n);                                                 \
    }

#define DEFINE_MATH_LOG1P(T, suffix, t)                                     \
    T math_log1p_##suffix(T n) {                                            \
        return log1p##t(n);                                                 \
    }

#define DEFINE_MATH_EXP(T, suffix, t)                                       \
    T math_exp_##suffix(T n) {                                              \
        return exp##t(n);                                                   \
    }

#define DEFINE_MATH_EXP2(T, suffix, t)                                      \
    T math_exp2_##suffix(T n) {                                             \
        return exp2##t(n);                                                  \
    }

#define DEFINE_MATH_EXPM1(T, suffix, t)                                     \
    T math_expm1_##suffix(T n) {                                            \
        return expm1##t(n);                                                 \
    }

#define DEFINE_MATH_SQRT(T, suffix, t)                                      \
    T math_sqrt_##suffix(T n) {                                             \
        return sqrt##t(n);                                                  \
    }

#define DEFINE_MATH_CBRT(T, suffix, t)                                      \
    T math_cbrt_##suffix(T n) {                                             \
        return cbrt##t(n);                                                  \
    }

#define DEFINE_MATH_HYPOT(T, suffix, t)                                     \
    T math_hypot_##suffix(T x, T y) {                                       \
        return hypot##t(x, y);                                              \
    }

#define DEFINE_MATH_SIN(T, suffix, t)                                       \
    T math_sin_##suffix(T n) {                                              \
        return sin##t(n);                                                   \
    }

#define DEFINE_MATH_COS(T, suffix, t)                                       \
    T math_cos_##suffix(T n) {                                              \
        return cos##t(n);                                                   \
    }

#define DEFINE_MATH_TAN(T, suffix, t)                                       \
    T math_tan_##suffix(T n) {                                              \
        return tan##t(n);                                                   \
    }

#define DEFINE_MATH_ASIN(T, suffix, t)                                      \
    T math_asin_##suffix(T n) {                                             \
        return asin##t(n);                                                  \
    }

#define DEFINE_MATH_ACOS(T, suffix, t)                                      \
    T math_acos_##suffix(T n) {                                             \
        return acos##t(n);                                                  \
    }

#define DEFINE_MATH_ATAN(T, suffix, t)                                      \
    T math_atan_##suffix(T n) {                                             \
        return atan##t(n);                                                  \
    }

#define DEFINE_MATH_ATAN2(T, suffix, t)                                     \
    T math_atan2_##suffix(T y, T x) {                                       \
        return atan2##t(y, x);                                              \
    }

#define DEFINE_MATH_SINH(T, suffix, t)                                      \
    T math_sinh_##suffix(T n) {                                             \
        return sinh##t(n);                                                  \
    }

#define DEFINE_MATH_COSH(T, suffix, t)                                      \
    T math_cosh_##suffix(T n) {                                             \
        return cosh##t(n);                                                  \
    }

#define DEFINE_MATH_TANH(T, suffix, t)                                      \
    T math_tanh_##suffix(T n) {                                             \
        return tanh##t(n);                                                  \
    }

#define DEFINE_MATH_ASINH(T, suffix, t)                                     \
    T math_asinh_##suffix(T n) {                                            \
        return asinh##t(n);                                                 \
    }

#define DEFINE_MATH_ACOSH(T, suffix, t)                                     \
    T math_acosh_##suffix(T n) {                                            \
        return acosh##t(n);                                                 \
    }

#define DEFINE_MATH_ATANH(T, suffix, t)                                     \
    T math_atanh_##suffix(T n) {                                            \
        return atanh##t(n);                                                 \
    }

#define DEFINE_MATH_ABS_FLOAT(T, suffix, t)                                 \
    T math_abs_##suffix(T n) {                                              \
        return fabs##t(n);                                                  \
    }

#define DEFINE_MATH_MIN_FLOAT(T, suffix, t)                                 \
    T math_min_##suffix(T a, T b) {                                         \
        return fmin##t(a, b);                                               \
    }

#define DEFINE_MATH_MAX_FLOAT(T, suffix, t)                                 \
    T math_max_##suffix(T a, T b) {                                         \
        return fmax##t(a, b);                                               \
    }

#define DEFINE_MATH_SIGN(T, suffix)                                         \
    T math_sign_##suffix(T n) {                                             \
        if (n > 0) return 1;                                                \
        if (n < 0) return -1;                                               \
        return 0;                                                           \
    }

#define DEFINE_MATH_COPYSIGN(T, suffix, t)                                  \
    T math_copysign_##suffix(T value, T sign) {                             \
        return copysign##t(value, sign);                                    \
    }

#define DEFINE_MATH_SCALBN(T, suffix, t)                                    \
    T math_scalbn_##suffix(T n, int exponent) {                             \
        return scalbn##t(n, exponent);                                      \
    }

#define DEFINE_MATH_SCALBLN(T, suffix, t)                                   \
    T math_scalbln_##suffix(T n, long exponent) {                           \
        return scalbln##t(n, exponent);                                     \
    }

#define DEFINE_MATH_FREXP(T, suffix, t)                                     \
    T math_frexp_##suffix(T n, int* exponent) {                             \
        return frexp##t(n, exponent);                                       \
    }

#define DEFINE_MATH_LDEXP(T, suffix, t)                                     \
    T math_ldexp_##suffix(T n, int exponent) {                              \
        return ldexp##t(n, exponent);                                       \
    }

#define DEFINE_MATH_NEXT_AFTER(T, suffix, t)                                \
    T math_next_after_##suffix(T from, T to) {                              \
        return nextafter##t(from, to);                                      \
    }

#define DEFINE_MATH_NEXT_TOWARD(T, suffix, t)                               \
    T math_next_toward_##suffix(T from, ldouble to) {                       \
        return nexttoward##t(from, to);                                     \
    }

#define DEFINE_MATH_REMAINDER(T, suffix, t)                                 \
    T math_remainder_##suffix(T a, T b) {                                   \
        return remainder##t(a, b);                                          \
    }

#define DEFINE_MATH_MOD(T, suffix, t)                                       \
    T math_mod_##suffix(T a, T b) {                                         \
        return fmod##t(a, b);                                               \
    }

#define DEFINE_MATH_FDIM(T, suffix, t)                                      \
    T math_fdim_##suffix(T a, T b) {                                        \
        return fdim##t(a, b);                                               \
    }

#define DEFINE_MATH_FMA(T, suffix, t)                                       \
    T math_fma_##suffix(T a, T b, T c) {                                    \
        return fma##t(a, b, c);                                             \
    }

#define DEFINE_MATH_ROUND(T, suffix, t)                                     \
    T math_round_##suffix(T n) {                                            \
        return round##t(n);                                                 \
    }

#define DEFINE_MATH_FLOOR(T, suffix, t)                                     \
    T math_floor_##suffix(T n) {                                            \
        return floor##t(n);                                                 \
    }

#define DEFINE_MATH_CEIL(T, suffix, t)                                      \
    T math_ceil_##suffix(T n) {                                             \
        return ceil##t(n);                                                  \
    }

#define DEFINE_MATH_TRUNC(T, suffix, t)                                     \
    T math_trunc_##suffix(T n) {                                            \
        return trunc##t(n);                                                 \
    }

#define DEFINE_MATH_IS_NAN(T, suffix)                                       \
    bool math_is_nan_##suffix(T n) {                                        \
        return isnan(n);                                                    \
    }

#define DEFINE_MATH_IS_INFINITE(T, suffix)                                  \
    bool math_is_infinite_##suffix(T n) {                                   \
        return isinf(n);                                                    \
    }

#define DEFINE_MATH_IS_FINITE(T, suffix)                                    \
    bool math_is_finite_##suffix(T n) {                                     \
        return isfinite(n);                                                 \
    }

#define DEFINE_MATH_IS_NORMAL(T, suffix)                                    \
    bool math_is_normal_##suffix(T n) {                                     \
        return isnormal(n);                                                 \
    }

#define DEFINE_MATH_IS_CLOSE(T, suffix, t)                                  \
    bool math_is_close_##suffix(T a, T b, T tolerance) {                    \
        return fabs##t(a - b) <= tolerance;                                 \
    }

#define DEFINE_MATH_LERP(T, suffix)                                         \
    T math_lerp_##suffix(T start, T end, T amount) {                        \
        return start + (end - start) * amount;                              \
    }

#define DEFINE_MATH_PERCENTAGE(T, suffix, t)                                \
    T math_percentage_##suffix(T value, T percentage) {                     \
        return (value / 100.0##t) * percentage;                             \
    }

#define DEFINE_MATH_RANDOM(T, suffix, t)                                    \
    T math_random_##suffix(void) {                                          \
        return (T) rand() / ((T) RAND_MAX + 1.0##t);                        \
    }

#define DEFINE_MATH_TO_RADIANS(T, suffix, t)                                \
    T math_to_radians_##suffix(T degrees) {                                 \
        return degrees * MATH_PI(T) / 180.0##t;                             \
    }

#define DEFINE_MATH_TO_DEGREES(T, suffix, t)                                \
    T math_to_degrees_##suffix(T radians) {                                 \
        return radians * 180.0##t / MATH_PI(T);                             \
    }

/* -------------------------------------------------------------------------- */
/* Number type functions                                                      */
/* -------------------------------------------------------------------------- */

#define DEFINE_MATH_MEAN(T, suffix)                                         \
    double math_mean_##suffix(T* values, uint size) {                       \
        double result = 0;                                                  \
        for (uint i = 0; i < size; i++) {                                   \
            result += values[i];                                            \
        }                                                                   \
        return result / size;                                               \
    }

#define DEFINE_MATH_MEDIAN(T, suffix)                                       \
    double math_median_##suffix(T* values, uint size) {                     \
        if (size == 0) {                                                    \
            return NAN;                                                     \
        }                                                                   \
        const size_t middle = size / 2;                                     \
                                                                            \
        return size % 2 == 0                                                \
            ? values[middle - 1] / 2.0 + values[middle] / 2.0               \
            : values[middle];                                               \
    }

/* -------------------------------------------------------------------------- */
/* Integer type generation                                                    */
/* -------------------------------------------------------------------------- */

#define DEFINE_SIGNED_MATH(T, suffix, min, max)                             \
    DEFINE_MATH_ADD_EXACT_SIGNED(T, suffix, min, max)                       \
    DEFINE_MATH_SUB_EXACT_SIGNED(T, suffix, min, max)                       \
    DEFINE_MATH_MUL_EXACT_SIGNED(T, suffix, min, max)                       \
    DEFINE_MATH_DIV_EXACT_SIGNED(T, suffix, min)                            \
    DEFINE_MATH_GCD_SIGNED(T, suffix)                                       \
    DEFINE_MATH_LCM_SIGNED(T, suffix)                                       \
    DEFINE_MATH_FACTORIAL_SIGNED(T, suffix, min, max)                       \
    DEFINE_MATH_COMBINATIONS_SIGNED(T, suffix, max)                         \
    DEFINE_MATH_PERMUTATIONS_SIGNED(T, suffix, max)                         \
    DEFINE_MATH_MEAN(T, suffix)                                             \
    DEFINE_MATH_MEDIAN(T, suffix)                                           \
    DEFINE_MATH_IS_PRIME(T, suffix)                                         \
    DEFINE_MATH_IS_POWER_OF_TWO_SIGNED(T, suffix)                           \
    DEFINE_MATH_ABS_SIGNED(T, suffix)                                       \
    DEFINE_MATH_MIN_INTEGER(T, suffix)                                      \
    DEFINE_MATH_MAX_INTEGER(T, suffix)

#define DEFINE_UNSIGNED_MATH(T, suffix, max)                                \
    DEFINE_MATH_ADD_EXACT_UNSIGNED(T, suffix, max)                          \
    DEFINE_MATH_SUB_EXACT_UNSIGNED(T, suffix)                               \
    DEFINE_MATH_MUL_EXACT_UNSIGNED(T, suffix, max)                          \
    DEFINE_MATH_DIV_EXACT_UNSIGNED(T, suffix)                               \
    DEFINE_MATH_GCD_UNSIGNED(T, suffix)                                     \
    DEFINE_MATH_LCM_UNSIGNED(T, suffix)                                     \
    DEFINE_MATH_FACTORIAL_UNSIGNED(T, suffix, max)                          \
    DEFINE_MATH_COMBINATIONS_UNSIGNED(T, suffix, max)                       \
    DEFINE_MATH_PERMUTATIONS_UNSIGNED(T, suffix, max)                       \
    DEFINE_MATH_MEAN(T, suffix)                                             \
    DEFINE_MATH_MEDIAN(T, suffix)                                           \
    DEFINE_MATH_IS_PRIME(T, suffix)                                         \
    DEFINE_MATH_IS_POWER_OF_TWO_UNSIGNED(T, suffix)                         \
    DEFINE_MATH_ABS_UNSIGNED(T, suffix)                                     \
    DEFINE_MATH_MIN_INTEGER(T, suffix)                                      \
    DEFINE_MATH_MAX_INTEGER(T, suffix)

/* -------------------------------------------------------------------------- */
/* Floating-point type generation                                             */
/* -------------------------------------------------------------------------- */

#define DEFINE_FLOAT_MATH(T, suffix, t)                                     \
    DEFINE_MATH_POW(T, suffix, t)                                           \
    DEFINE_MATH_LOG_NATURAL(T, suffix, t)                                   \
    DEFINE_MATH_LOG_BASE(T, suffix, t)                                      \
    DEFINE_MATH_LOG2(T, suffix, t)                                          \
    DEFINE_MATH_LOG10(T, suffix, t)                                         \
    DEFINE_MATH_LOG1P(T, suffix, t)                                         \
    DEFINE_MATH_EXP(T, suffix, t)                                           \
    DEFINE_MATH_EXP2(T, suffix, t)                                          \
    DEFINE_MATH_EXPM1(T, suffix, t)                                         \
    DEFINE_MATH_SQRT(T, suffix, t)                                          \
    DEFINE_MATH_CBRT(T, suffix, t)                                          \
    DEFINE_MATH_HYPOT(T, suffix, t)                                         \
    DEFINE_MATH_SIN(T, suffix, t)                                           \
    DEFINE_MATH_COS(T, suffix, t)                                           \
    DEFINE_MATH_TAN(T, suffix, t)                                           \
    DEFINE_MATH_ASIN(T, suffix, t)                                          \
    DEFINE_MATH_ACOS(T, suffix, t)                                          \
    DEFINE_MATH_ATAN(T, suffix, t)                                          \
    DEFINE_MATH_ATAN2(T, suffix, t)                                         \
    DEFINE_MATH_SINH(T, suffix, t)                                          \
    DEFINE_MATH_COSH(T, suffix, t)                                          \
    DEFINE_MATH_TANH(T, suffix, t)                                          \
    DEFINE_MATH_ASINH(T, suffix, t)                                         \
    DEFINE_MATH_ACOSH(T, suffix, t)                                         \
    DEFINE_MATH_ATANH(T, suffix, t)                                         \
    DEFINE_MATH_ABS_FLOAT(T, suffix, t)                                     \
    DEFINE_MATH_MIN_FLOAT(T, suffix, t)                                     \
    DEFINE_MATH_MAX_FLOAT(T, suffix, t)                                     \
    DEFINE_MATH_SIGN(T, suffix)                                             \
    DEFINE_MATH_COPYSIGN(T, suffix, t)                                      \
    DEFINE_MATH_SCALBN(T, suffix, t)                                        \
    DEFINE_MATH_SCALBLN(T, suffix, t)                                       \
    DEFINE_MATH_FREXP(T, suffix, t)                                         \
    DEFINE_MATH_LDEXP(T, suffix, t)                                         \
    DEFINE_MATH_NEXT_AFTER(T, suffix, t)                                    \
    DEFINE_MATH_NEXT_TOWARD(T, suffix, t)                                   \
    DEFINE_MATH_REMAINDER(T, suffix, t)                                     \
    DEFINE_MATH_MOD(T, suffix, t)                                           \
    DEFINE_MATH_FDIM(T, suffix, t)                                          \
    DEFINE_MATH_FMA(T, suffix, t)                                           \
    DEFINE_MATH_ROUND(T, suffix, t)                                         \
    DEFINE_MATH_FLOOR(T, suffix, t)                                         \
    DEFINE_MATH_CEIL(T, suffix, t)                                          \
    DEFINE_MATH_TRUNC(T, suffix, t)                                         \
    DEFINE_MATH_IS_NAN(T, suffix)                                           \
    DEFINE_MATH_IS_INFINITE(T, suffix)                                      \
    DEFINE_MATH_IS_FINITE(T, suffix)                                        \
    DEFINE_MATH_IS_NORMAL(T, suffix)                                        \
    DEFINE_MATH_IS_CLOSE(T, suffix, t)                                      \
    DEFINE_MATH_LERP(T, suffix)                                             \
    DEFINE_MATH_PERCENTAGE(T, suffix, t)                                    \
    DEFINE_MATH_RANDOM(T, suffix, t)                                        \
    DEFINE_MATH_MEAN(T, suffix)                                             \
    DEFINE_MATH_MEDIAN(T, suffix)                                           \
    DEFINE_MATH_TO_RADIANS(T, suffix, t)                                    \
    DEFINE_MATH_TO_DEGREES(T, suffix, t)

/* -------------------------------------------------------------------------- */
/* Generate implementations                                                   */
/* -------------------------------------------------------------------------- */

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

DEFINE_FLOAT_MATH(float, float, f)
DEFINE_FLOAT_MATH(double, double, /* empty */)
DEFINE_FLOAT_MATH(ldouble, ldouble, l)