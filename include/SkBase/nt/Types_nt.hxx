#pragma once

#if defined(SKETCH_WIN_KM)

// WDK lacks <cstdint>, so we need to craft our own types

namespace sketch
{

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef long                int32_t;
typedef unsigned long       uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

using ::intptr_t;
using ::uintptr_t;

using ::size_t;
using ::ptrdiff_t;

#ifdef SKETCH_64

typedef long long ssize_t;

#else

typedef __w64 int ssize_t;

#endif


typedef decltype(nullptr) nullptr_t;

} // namespace sketch {}


using sketch::int8_t;
using sketch::uint8_t;
using sketch::int16_t;
using sketch::uint16_t;
using sketch::int32_t;
using sketch::uint32_t;
using sketch::int64_t;
using sketch::uint64_t;


using ::intptr_t;
using ::uintptr_t;

using ::ptrdiff_t;
using ::size_t;
using sketch::ssize_t;

using sketch::nullptr_t;


#elif defined(SKETCH_WIN_UM)

#include <cstdint>

namespace sketch
{

using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

using std::intptr_t;
using std::uintptr_t;


using ::size_t;

#ifdef SKETCH_64

typedef long long ptrdiff_t;
typedef long long ssize_t;

#else

typedef __w64 int ptrdiff_t;
typedef __w64 int ssize_t;

#endif

using std::nullptr_t;

} // namespace sketch {}


using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

using std::intptr_t;
using std::uintptr_t;

using ::size_t;
using sketch::ptrdiff_t;
using sketch::ssize_t;

#endif




