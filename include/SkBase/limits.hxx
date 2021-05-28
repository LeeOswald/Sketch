#pragma once


#include "../Sketch.hxx"


#ifdef SKETCH_STL_AVAILABLE

#include <climits>

#else

#pragma region C limits

#ifndef CHAR_BIT
#define CHAR_BIT  8
#endif

#ifndef SCHAR_MIN
#define SCHAR_MIN (-128)
#endif

#ifndef SCHAR_MAX
#define SCHAR_MAX 127
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 0xff
#endif

#ifndef CHAR_MIN
#if !defined(_CHAR_UNSIGNED) && !defined(__CHAR_UNSIGNED__)
#define CHAR_MIN    SCHAR_MIN
#define CHAR_MAX    SCHAR_MAX
#else
#define CHAR_MIN    0
#define CHAR_MAX    UCHAR_MAX
#endif
#endif

#ifndef MB_LEN_MAX
#define MB_LEN_MAX  5
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN   0
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX   0xffff
#endif

#ifndef SHRT_MIN
#define SHRT_MIN    (-32768)
#endif

#ifndef SHRT_MAX
#define SHRT_MAX    32767
#endif

#ifndef USHRT_MAX
#define USHRT_MAX   0xffff
#endif

#ifndef INT_MIN
#define INT_MIN     (-INT_MAX-1)
#endif

#ifndef INT_MAX
#define INT_MAX     2147483647
#endif

#ifndef UINT_MAX
#define UINT_MAX    0xffffffff
#endif

#ifndef LONG_MIN
#define LONG_MIN    (-LONG_MAX-1L)
#endif

#ifndef LONG_MAX
#define LONG_MAX    2147483647L
#endif

#ifndef ULONG_MAX
#define ULONG_MAX   0xffffffffUL
#endif

#ifndef LLONG_MAX
#define LLONG_MAX   9223372036854775807i64
#endif

#ifndef LLONG_MIN
#define LLONG_MIN   (-LLONG_MAX-1)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX  0xffffffffffffffffui64
#endif

#pragma endregion

#endif


#ifdef SKETCH_STL_AVAILABLE

#include <cfloat>

#else

#pragma region C float

#ifndef FLT_DIG
#define FLT_DIG         6
#define FLT_EPSILON     1.192092896e-07F
#define FLT_GUARD       0
#define FLT_MANT_DIG    24
#define FLT_MAX         3.402823466e+38F
#define FLT_MAX_10_EXP  38
#define FLT_MAX_EXP     128
#define FLT_MIN         1.175494351e-38F
#define FLT_MIN_10_EXP  (-37)
#define FLT_MIN_EXP     (-125)
#define FLT_NORMALIZE   0
#define FLT_RADIX       2
#define FLT_ROUNDS      1
#define FLT_TRUE_MIN    1.401298464e-45F
#endif


#ifndef DBL_DIG
#define DBL_DIG         15
#define DBL_EPSILON     2.2204460492503131e-016
#define DBL_MANT_DIG    53
#define DBL_MAX         1.7976931348623158e+308
#define DBL_MAX_10_EXP  308
#define DBL_MAX_EXP     1024
#define DBL_MIN         2.2250738585072014e-308
#define DBL_MIN_10_EXP  (-307)
#define DBL_MIN_EXP     (-1021)
#define DBL_ROUNDS      1
#define DBL_RADIX       2
#define DBL_TRUE_MIN    4.9406564584124654e-324
#endif


#ifndef LDBL_DIG
#define LDBL_DIG        DBL_DIG
#define LDBL_EPSILON    DBL_EPSILON
#define LDBL_MANT_DIG   DBL_MANT_DIG
#define LDBL_MAX        DBL_MAX
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#define LDBL_MAX_EXP    DBL_MAX_EXP
#define LDBL_MIN        DBL_MIN
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP
#define LDBL_MIN_EXP    DBL_MIN_EXP
#define LDBL_ROUNDS     DBL_ROUNDS
#define LDBL_RADIX      DBL_RADIX
#define LDBL_TRUE_MIN   DBL_TRUE_MIN
#endif

#pragma endregion

#endif

#ifdef SKETCH_STL_AVAILABLE
#include <limits>
#endif


namespace sketch
{

enum float_round_style
{
    round_indeterminate = -1,
    round_toward_zero = 0,
    round_to_nearest = 1,
    round_toward_infinity = 2,
    round_toward_neg_infinity = 3
};


enum float_denorm_style
{
    denorm_indeterminate = -1,
    denorm_absent = 0,
    denorm_present = 1
};


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

struct _Num_base
{
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_exact = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_integer = false;
    static constexpr bool is_modulo = false;
    static constexpr bool is_signed = false;
    static constexpr bool is_specialized = false;
    static constexpr bool tinyness_before = false;
    static constexpr bool traps = false;
    static constexpr float_round_style round_style = round_toward_zero;
    static constexpr int digits = 0;
    static constexpr int digits10 = 0;

    static constexpr int max_digits10 = 0;

    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int radix = 0;
};


template<class T>
class numeric_limits
    : public _Num_base
{
public:
    static constexpr T min() noexcept
    {
        return (T());
    }

    static constexpr T max() noexcept
    {
        return (T());
    }

    static constexpr T lowest() noexcept
    {
        return (T());
    }

    static constexpr T epsilon() noexcept
    {
        return (T());
    }

    static constexpr T round_error() noexcept
    {
        return (T());
    }

    static constexpr T denorm_min() noexcept
    {
        return (T());
    }

    static constexpr T infinity() noexcept
    {
        return (T());
    }

    static constexpr T quiet_NaN() noexcept
    {
        return (T());
    }

    static constexpr T signaling_NaN() noexcept
    {
        return (T());
    }
};

template<class T>
class numeric_limits<const T>
    : public numeric_limits<T>
{
};

template<class T>
class numeric_limits<volatile T>
    : public numeric_limits<T>
{
};

template<class T>
class numeric_limits<const volatile T>
    : public numeric_limits<T>
{
};


struct _Num_int_base
    : public _Num_base
{	// base for integer types
    static constexpr bool is_bounded = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_modulo = true;
    static constexpr bool is_specialized = true;
    static constexpr int radix = 2;
};


struct _Num_float_base
    : public _Num_base
{
    static constexpr float_denorm_style has_denorm = denorm_present;
    static constexpr bool has_denorm_loss = true;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_exact = false;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_modulo = false;
    static constexpr bool is_signed = true;
    static constexpr bool is_specialized = true;
    static constexpr bool tinyness_before = true;
    static constexpr bool traps = false;
    static constexpr float_round_style round_style = round_to_nearest;
    static constexpr int radix = FLT_RADIX;
};

template<>
struct numeric_limits<bool>
    : public _Num_int_base
{
    typedef bool T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return false; }
    static constexpr T max() noexcept { return true; }
    static constexpr T lowest() noexcept { return false; }
    static constexpr const int digits = 1;
    static constexpr const int digits10 = 0;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = false;
    static constexpr const T __max = true;
    static constexpr const T __lowest = false;
};

template<>
struct numeric_limits<char>
    : public _Num_int_base
{
    typedef char  T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return CHAR_MIN; }
    static constexpr T max() noexcept { return CHAR_MAX; }
    static constexpr T lowest() noexcept { return CHAR_MIN; }
    static constexpr const int digits = CHAR_BIT + (CHAR_MIN >> (CHAR_BIT - 1));
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = (CHAR_MIN >> (CHAR_BIT - 1)) != 0;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = CHAR_MIN;
    static constexpr const T __max = CHAR_MAX;
    static constexpr const T __lowest = CHAR_MIN;
};

template<>
struct numeric_limits<wchar_t>
    : public _Num_int_base
{
    typedef wchar_t T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return WCHAR_MIN; }
    static constexpr T max() noexcept { return WCHAR_MAX; }
    static constexpr T lowest() noexcept { return WCHAR_MIN; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT - (WCHAR_MIN ? 1 : 0);
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = (WCHAR_MIN ? 1 : 0);
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;
};

template<>
struct numeric_limits<signed char>
    : public _Num_int_base
{
    typedef signed char T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return SCHAR_MIN; }
    static constexpr T max() noexcept { return SCHAR_MAX; }
    static constexpr T lowest() noexcept { return SCHAR_MIN; }
    static constexpr const int digits = CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = true;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;
};

template<>
struct numeric_limits<short>
    : public _Num_int_base
{
    typedef short T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return SHRT_MIN; }
    static constexpr T max() noexcept { return SHRT_MAX; }
    static constexpr T lowest() noexcept { return SHRT_MIN; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = true;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = SHRT_MIN;
    static constexpr const T __max = SHRT_MAX;
    static constexpr const T __lowest = SHRT_MIN;
};

template<>
struct numeric_limits<int>
    : public _Num_int_base
{
    typedef int T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return INT_MIN; }
    static constexpr T max() noexcept { return INT_MAX; }
    static constexpr T lowest() noexcept { return INT_MIN; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = true;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = INT_MIN;
    static constexpr const T __max = INT_MAX;
    static constexpr const T __lowest = INT_MIN;
};

template<>
struct numeric_limits<long>
    : public _Num_int_base
{
    typedef long T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return LONG_MIN; }
    static constexpr T max() noexcept { return LONG_MAX; }
    static constexpr T lowest() noexcept { return LONG_MIN; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = true;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = LONG_MIN;
    static constexpr const T __max = LONG_MAX;
    static constexpr const T __lowest = LONG_MIN;
};

template<>
struct numeric_limits<long long>
    : public _Num_int_base
{
    typedef long long T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return LLONG_MIN; }
    static constexpr T max() noexcept { return LLONG_MAX; }
    static constexpr T lowest() noexcept { return LLONG_MIN; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = true;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = LLONG_MIN;
    static constexpr const T __max = LLONG_MAX;
    static constexpr const T __lowest = LLONG_MIN;
};

template<>
struct numeric_limits<unsigned char>
    : public _Num_int_base
{
    typedef unsigned char T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return 0; }
    static constexpr T max() noexcept { return UCHAR_MAX; }
    static constexpr T lowest() noexcept { return 0; }
    static constexpr const int digits = CHAR_BIT - 1;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = 0;
    static constexpr const T __max = UCHAR_MAX;
    static constexpr const T __lowest = 0;
};

#ifdef _NATIVE_WCHAR_T_DEFINED

template<>
struct numeric_limits<unsigned short>
    : public _Num_int_base
{
    typedef unsigned short T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return 0; }
    static constexpr T max() noexcept { return USHRT_MAX; }
    static constexpr T lowest() noexcept { return 0; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = 0;
    static constexpr const T __max = USHRT_MAX;
    static constexpr const T __lowest = 0;
};

#endif

template<>
struct numeric_limits<unsigned int>
    : public _Num_int_base
{
    typedef unsigned int T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return 0; }
    static constexpr T max() noexcept { return UINT_MAX; }
    static constexpr T lowest() noexcept { return 0; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = 0;
    static constexpr const T __max = UINT_MAX;
    static constexpr const T __lowest = 0;
};

template<>
struct numeric_limits<unsigned long>
    : public _Num_int_base
{
    typedef unsigned long T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return 0; }
    static constexpr T max() noexcept { return ULONG_MAX; }
    static constexpr T lowest() noexcept { return 0; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = 0;
    static constexpr const T __max = ULONG_MAX;
    static constexpr const T __lowest = 0;
};

template<>
struct numeric_limits<unsigned long long>
    : public _Num_int_base
{
    typedef unsigned long long T;

    static constexpr const bool is_specialized = true;
    static constexpr T min() noexcept { return 0; }
    static constexpr T max() noexcept { return ULLONG_MAX; }
    static constexpr T lowest() noexcept { return 0; }
    static constexpr const int digits = sizeof(T) * CHAR_BIT;
    static constexpr const int digits10 = digits * 301 / 1000;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_integer = true;
    static constexpr const bool is_exact = true;
    static constexpr const int radix = 2;
    static constexpr T epsilon() noexcept { return 0; }
    static constexpr T round_error() noexcept { return 0; }
    static constexpr const int min_exponent = 0;
    static constexpr const int min_exponent10 = 0;
    static constexpr const int max_exponent = 0;
    static constexpr const int max_exponent10 = 0;
    static constexpr const bool has_infinity = false;
    static constexpr const bool has_quiet_NaN = false;
    static constexpr const bool has_signaling_NaN = false;
    static constexpr const float_denorm_style has_denorm = denorm_absent;
    static constexpr const bool has_denorm_loss = false;
    static constexpr T infinity() noexcept { return 0; }
    static constexpr T quiet_NaN() noexcept { return 0; }
    static constexpr T signaling_NaN() noexcept { return 0; }
    static constexpr T denorm_min() noexcept { return 0; }
    static constexpr const bool is_iec559 = false;
    static constexpr const bool is_bounded = true;
    static constexpr const bool is_modulo = true;
    static constexpr const bool traps = false;
    static constexpr const bool tinyness_before = false;
    static constexpr const float_round_style round_style = round_toward_zero;

    static constexpr const T __min = 0;
    static constexpr const T __max = ULLONG_MAX;
    static constexpr const T __lowest = 0;
};


#ifndef SKETCH_KM

template<>
class numeric_limits<float>
    : public _Num_float_base
{
public:
    typedef float T;

    static constexpr T min() noexcept
    {
        return FLT_MIN;
    }

    static constexpr T max() noexcept
    {
        return FLT_MAX;
    }

    static constexpr T lowest() noexcept
    {
        return (-(max)());
    }

    static constexpr T epsilon() noexcept
    {
        return FLT_EPSILON;
    }

    static constexpr T round_error() noexcept
    {
        return (0.5F);
    }

    static constexpr T denorm_min() noexcept
    {
        return FLT_TRUE_MIN;
    }

    static constexpr T infinity() noexcept
    {
        return (__builtin_huge_valf());
    }

    static constexpr T quiet_NaN() noexcept
    {
        return (__builtin_nanf("0"));
    }

    static constexpr T signaling_NaN() noexcept
    {
        return (__builtin_nansf("1"));
    }

    static constexpr int digits = FLT_MANT_DIG;
    static constexpr int digits10 = FLT_DIG;

    static constexpr int max_digits10 = 2 + FLT_MANT_DIG * 301L / 1000;

    static constexpr int max_exponent = (int)FLT_MAX_EXP;
    static constexpr int max_exponent10 = (int)FLT_MAX_10_EXP;
    static constexpr int min_exponent = (int)FLT_MIN_EXP;
    static constexpr int min_exponent10 = (int)FLT_MIN_10_EXP;
};


template<>
class numeric_limits<double>
    : public _Num_float_base
{
public:
    typedef double T;

    static constexpr T min() noexcept
    {
        return DBL_MIN;
    }

    static constexpr T max() noexcept
    {
        return DBL_MAX;
    }

    static constexpr T lowest() noexcept
    {
        return -max();
    }

    static constexpr T epsilon() noexcept
    {
        return DBL_EPSILON;
    }

    static constexpr T round_error() noexcept
    {
        return (0.5);
    }

    static constexpr T denorm_min() noexcept
    {
        return DBL_TRUE_MIN;
    }

    static constexpr T infinity() noexcept
    {
        return __builtin_huge_val();
    }

    static constexpr T quiet_NaN() noexcept
    {
        return __builtin_nan("0");
    }

    static constexpr T signaling_NaN() noexcept
    {
        return __builtin_nans("1");
    }

    static constexpr int digits = DBL_MANT_DIG;
    static constexpr int digits10 = DBL_DIG;

    static constexpr int max_digits10 = 2 + DBL_MANT_DIG * 301L / 1000;

    static constexpr int max_exponent = (int)DBL_MAX_EXP;
    static constexpr int max_exponent10 = (int)DBL_MAX_10_EXP;
    static constexpr int min_exponent = (int)DBL_MIN_EXP;
    static constexpr int min_exponent10 = (int)DBL_MIN_10_EXP;
};

template<>
class numeric_limits<long double>
    : public _Num_float_base
{
public:
    typedef long double T;

    static constexpr T min() noexcept
    {
        return LDBL_MIN;
    }

    static constexpr T max() noexcept
    {
        return LDBL_MAX;
    }

    static constexpr T lowest() noexcept
    {
        return -max();
    }

    static constexpr T epsilon() noexcept
    {
        return LDBL_EPSILON;
    }

    static constexpr T round_error() noexcept
    {
        return (0.5L);
    }

    static constexpr T denorm_min() noexcept
    {
        return LDBL_TRUE_MIN;
    }

    static constexpr T infinity() noexcept
    {
        return __builtin_huge_val();
    }

    static constexpr T quiet_NaN() noexcept
    {
        return __builtin_nan("0");
    }

    static constexpr T signaling_NaN() noexcept
    {
        return __builtin_nans("1");
    }

    static constexpr int digits = LDBL_MANT_DIG;
    static constexpr int digits10 = LDBL_DIG;

    static constexpr int max_digits10 = 2 + LDBL_MANT_DIG * 301L / 1000;

    static constexpr int max_exponent = (int)LDBL_MAX_EXP;
    static constexpr int max_exponent10 = (int)LDBL_MAX_10_EXP;
    static constexpr int min_exponent = (int)LDBL_MIN_EXP;
    static constexpr int min_exponent10 = (int)LDBL_MIN_10_EXP;
};

#endif // SKETCH_KM

} // namespace sketch {}


