#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(KERNEL_MODE) && !defined(USER_MODE)
#error Either KERNEL_MODE or USER_MODE must be defined
#endif

#ifndef _MSC_VER
#error Unsupported compiler
#endif

#if defined(_WIN32) && defined(KERNEL_MODE)
#define SKETCH_C_WIN
#define SKETCH_C_WIN_KM
#define SKETCH_C_KM
#elif defined(_WIN32) && defined(USER_MODE)
#define SKETCH_C_WIN
#define SKETCH_C_WIN_UM
#define SKETCH_C_UM
#else
#error Unsupported platform
#endif

#if defined(SKETCH_C_WIN)
#if defined(_M_IX86)
#define SKETCH_C_32
#elif defined(_M_X64)
#define SKETCH_C_64
#endif
#endif

#ifdef SKETCH_C_WIN
#pragma warning (disable:4996) // function or variable may be unsafe
#endif


// platform headers
#if defined(SKETCH_C_WIN_KM)
#include <ntifs.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#elif defined(SKETCH_C_WIN_UM)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef HAVE_SKETCH_HXX

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef long                int32_t;
typedef unsigned long       uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;


#ifdef SKETCH_C_64
typedef long long ssize_t;
#else
typedef __w64 int ssize_t;
#endif

#endif // HAVE_SKETCH_HXX

#ifndef offsetof
#ifdef  SKETCH_C_64
#define offsetof(s,m)   (size_t)( (ptrdiff_t)&(((s *)0)->m) )
#else
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif
#endif


#ifdef SKETCH_C_WIN_KM
#define SKETCH_C_ASSERT(x)    NT_ASSERT(x)
#else
#include <cassert>
#define SKETCH_C_ASSERT(x)    assert(x)
#endif


#define SK_C_UNUSED(x) (x)


void* __cdecl sketch_c_calloc(size_t _Count, size_t _Size);
void __cdecl sketch_c_free(void* _Block);
void* __cdecl sketch_c_malloc(size_t _Size);


#ifdef SKETCH_C_WIN

inline int sketch_c_snprintfV(char* buffer, size_t size, const char* format, va_list args)
{
    return _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
}

inline int sketch_c_snprintf(char* buffer, size_t size, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int s = sketch_c_snprintfV(buffer, size, format, args);
    va_end(args);
    return s;
}

#endif


void sketch_c_rand(void* buffer, size_t size);


#ifdef __cplusplus
} // extern "C" {}
#endif
