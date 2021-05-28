#pragma once

#if !defined(KERNEL_MODE) && !defined(USER_MODE)
#error Either KERNEL_MODE or USER_MODE must be defined
#endif

#ifndef _MSC_VER
#error Unsupported compiler
#endif

#if defined(_WIN32) && defined(KERNEL_MODE)
#define SKETCH_WIN
#define SKETCH_WIN_KM
#define SKETCH_KM
#elif defined(_WIN32) && defined(USER_MODE)
#define SKETCH_WIN
#define SKETCH_WIN_UM
#define SKETCH_UM
#else
#error Unsupported platform
#endif

#if defined(SKETCH_WIN)
#if defined(_M_IX86)
#define SKETCH_32
#elif defined(_M_X64)
#define SKETCH_64
#endif
#endif

#if defined(SKETCH_WIN)
#if DBG || defined(_DEBUG)
#define SKETCH_DBG
#endif
#endif

/* platform headers */
#if defined(SKETCH_WIN_KM)

#include <ntifs.h>

#include <emmintrin.h>
#include <mmintrin.h>

#include <SkBase/nt/os_nt.hxx>


#elif defined(SKETCH_WIN_UM)

#include <windows.h>
#include <SkBase/win32/os_win32.hxx>

#include <cstdlib>

#endif

#include <stdarg.h>
#include <stddef.h>


#ifdef SKETCH_UM

#define SKETCH_STL_AVAILABLE
#define SKETCH_CXX_EXCEPTIONS

#endif // SKETCH_UM


#ifdef SKETCH_DBG
#define SKETCH_USE_DEBUG_ALLOCATOR
#endif



/* basic definitions */

#define SK_UNUSED(x) (x)


#ifndef _countof
template <typename T, size_t N>
char(*_sk_countof_helper(T(&a)[N]))[N];
#define _countof(a) sizeof(*_sk_countof_helper(a))
#endif


#define containing_record(a, T, f) \
  ((T*)((uint8_t*)(a) - offsetof(T, f)))


#define sk_static_assert(x)  static_assert(x, #x)


#include <SkBase/Types.hxx>


// apply (byte) offset to any type pointer and then convert it back to the desired type

template <typename _Ty>
constexpr __forceinline _Ty* offset_pointer(void* p, ptrdiff_t x) noexcept
{
    auto p_ = static_cast<uint8_t*>(p) + x;
    return static_cast<_Ty*>(static_cast<void*>(p_));
}

template <typename _Ty>
constexpr __forceinline const _Ty* offset_pointer(const void* p, ptrdiff_t x) noexcept
{
    auto p_ = static_cast<const uint8_t*>(p) + x;
    return static_cast<const _Ty*>(static_cast<const void*>(p_));
}


namespace sketch
{

typedef char16_t UChar;

} // namespace sketch{}


#ifdef SKETCH_CXX_EXCEPTIONS

namespace sketch
{

void throwOutOfMemoryException(const char* file, int line);

} // namespace sketch{}

#endif // SKETCH_CXX_EXCEPTIONS


#include <SkBase/NonCopyable.hxx>
#include <SkBase/Utility.hxx>
#include <SkBase/Heap.hxx>
#include <SkBase/new.hxx>
#include <SkBase/Allocator.hxx>
#include <SkBase/Assert.hxx>
#include <SkBase/SharedData.hxx>
#include <SkBase/String.hxx>
#include <SkBase/Utf8String.hxx>

#ifdef SKETCH_WIN
#include <SkBase/nt/AnsiString_nt.hxx>
#endif


namespace sketch
{

#ifdef SKETCH_WIN

inline AnsiString toSystemLocale(const String& s)
{
    return AnsiString(s);
}

inline AnsiString toSystemLocale(const wchar_t* s, size_t l = size_t(-1))
{
    return AnsiString(s, l);
}

inline AnsiString toSystemLocale(const UChar* s, size_t l = size_t(-1))
{
    return AnsiString(s, l);
}

inline String fromSystemLocale(const char* s, size_t l = size_t(-1))
{
    if (!s)
    {
        return String();
    }

    if (l == size_t(-1))
    {
        l = strlen(s);
    }

    if (!l)
    {
        return String();
    }

    ANSI_STRING as = { USHORT(l), USHORT(l), PCHAR(s) };
    UNICODE_STRING us = { 0 };

    auto status = RtlAnsiStringToUnicodeString(&us, &as, TRUE);
    if (status != STATUS_SUCCESS)
    {
        return String();
    }

    String result(&us);
    RtlFreeUnicodeString(&us);

    return result;
}

#else

inline Utf8String toSystemLocale(const String& s)
{
    return Utf8String(s);
}

inline Utf8String toSystemLocale(const wchar_t* s, size_t l = size_t(-1))
{
    return Utf8String(s, l);
}

inline Utf8String toSystemLocale(const UChar* s, size_t l = size_t(-1))
{
    return Utf8String(s, l);
}

inline String fromSystemLocale(const char* s, size_t l = size_t(-1))
{
    return String(s, l);
}

#endif

} // namespace sketch{}


#include <SkBase/Time.hxx>
#include <SkBase/Date.hxx>
#include <SkBase/Trace.hxx>
#include <SkBase/Error.hxx>

#ifdef SKETCH_CXX_EXCEPTIONS
#include <SkBase/Exception.hxx>
#endif

#include <SkBase/ErrorState.hxx>

#include <SkBase/Pair.hxx>
#include <SkBase/UniquePtr.hxx>

#ifdef SKETCH_WIN_KM
#include <SkBase/nt/Float_nt.hxx>
#endif
