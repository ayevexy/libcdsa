#ifndef LIBCDSA_LIMITS_H
#define LIBCDSA_LIMITS_H

/**
 * @file limits.h
 * @brief Typed aliases for standard C limit macros.
 */

#include "core/types.h"
#include <limits.h>
#include <float.h>

/**
 * @brief Integer types limit values.
 */

#define CHAR_BIT_VALUE                 ((int) CHAR_BIT)
#define MB_LEN_MAX_VALUE               ((int) MB_LEN_MAX)

#define CHAR_MAX_VALUE                 ((char) CHAR_MAX)
#define CHAR_MIN_VALUE                 ((char) CHAR_MIN)

#define SCHAR_MAX_VALUE                ((schar) SCHAR_MAX)
#define SCHAR_MIN_VALUE                ((schar) SCHAR_MIN)
#define UCHAR_MAX_VALUE                ((uchar) UCHAR_MAX)

#define SHORT_MAX_VALUE                ((short) SHRT_MAX)
#define SHORT_MIN_VALUE                ((short) SHRT_MIN)
#define USHORT_MAX_VALUE               ((ushort) USHRT_MAX)

#define INT_MAX_VALUE                  ((int) INT_MAX)
#define INT_MIN_VALUE                  ((int) INT_MIN)
#define UINT_MAX_VALUE                 ((uint) UINT_MAX)

#define LONG_MAX_VALUE                 ((long) LONG_MAX)
#define LONG_MIN_VALUE                 ((long) LONG_MIN)
#define ULONG_MAX_VALUE                ((ulong) ULONG_MAX)

#define LLONG_MAX_VALUE                ((llong) LLONG_MAX)
#define LLONG_MIN_VALUE                ((llong) LLONG_MIN)
#define ULLONG_MAX_VALUE               ((ullong) ULLONG_MAX)


/**
 * @brief Floating-point general properties.
 */

#define FLOAT_RADIX_VALUE              ((int) FLT_RADIX)
#define FLOAT_EVAL_METHOD_VALUE        ((int) FLT_EVAL_METHOD)
#define FLOAT_ROUNDS_VALUE             ((int) FLT_ROUNDS)
#define DECIMAL_DIG_VALUE              ((int) DECIMAL_DIG)

/**
 * @brief Float type limit values.
 */

#define FLOAT_MANT_DIG_VALUE           ((int) FLT_MANT_DIG)
#define FLOAT_DIG_VALUE                ((int) FLT_DIG)
#define FLOAT_DECIMAL_DIG_VALUE        ((int) FLT_DECIMAL_DIG)

#define FLOAT_MIN_VALUE                ((float) FLT_MIN)
#define FLOAT_MAX_VALUE                ((float) FLT_MAX)
#define FLOAT_EPSILON_VALUE            ((float) FLT_EPSILON)
#define FLOAT_TRUE_MIN_VALUE           ((float) FLT_TRUE_MIN)

#define FLOAT_MIN_EXP_VALUE            ((int) FLT_MIN_EXP)
#define FLOAT_MAX_EXP_VALUE            ((int) FLT_MAX_EXP)
#define FLOAT_MIN_10_EXP_VALUE         ((int) FLT_MIN_10_EXP)
#define FLOAT_MAX_10_EXP_VALUE         ((int) FLT_MAX_10_EXP)

/**
 * @brief Double type limit values.
 */

#define DOUBLE_MANT_DIG_VALUE          ((int) DBL_MANT_DIG)
#define DOUBLE_DIG_VALUE               ((int) DBL_DIG)
#define DOUBLE_DECIMAL_DIG_VALUE       ((int) DBL_DECIMAL_DIG)

#define DOUBLE_MIN_VALUE               ((double) DBL_MIN)
#define DOUBLE_MAX_VALUE               ((double) DBL_MAX)
#define DOUBLE_EPSILON_VALUE           ((double) DBL_EPSILON)
#define DOUBLE_TRUE_MIN_VALUE          ((double) DBL_TRUE_MIN)

#define DOUBLE_MIN_EXP_VALUE           ((int) DBL_MIN_EXP)
#define DOUBLE_MAX_EXP_VALUE           ((int) DBL_MAX_EXP)
#define DOUBLE_MIN_10_EXP_VALUE        ((int) DBL_MIN_10_EXP)
#define DOUBLE_MAX_10_EXP_VALUE        ((int) DBL_MAX_10_EXP)

/**
 * @brief Long double type limit values.
 */

#define LDOUBLE_MANT_DIG_VALUE         ((int) LDBL_MANT_DIG)
#define LDOUBLE_DIG_VALUE              ((int) LDBL_DIG)
#define LDOUBLE_DECIMAL_DIG_VALUE      ((int) LDBL_DECIMAL_DIG)

#define LDOUBLE_MIN_VALUE              ((ldouble) LDBL_MIN)
#define LDOUBLE_MAX_VALUE              ((ldouble) LDBL_MAX)
#define LDOUBLE_EPSILON_VALUE          ((ldouble) LDBL_EPSILON)
#define LDOUBLE_TRUE_MIN_VALUE         ((ldouble) LDBL_TRUE_MIN)

#define LDOUBLE_MIN_EXP_VALUE          ((int) LDBL_MIN_EXP)
#define LDOUBLE_MAX_EXP_VALUE          ((int) LDBL_MAX_EXP)
#define LDOUBLE_MIN_10_EXP_VALUE       ((int) LDBL_MIN_10_EXP)
#define LDOUBLE_MAX_10_EXP_VALUE       ((int) LDBL_MAX_10_EXP)

#endif