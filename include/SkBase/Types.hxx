#pragma once

#include "../Sketch.hxx"

#ifdef SKETCH_WIN
#include "nt/Types_nt.hxx"
#endif


sk_static_assert(sizeof(sketch::int8_t) == 1);
sk_static_assert(sizeof(sketch::uint8_t) == 1);
sk_static_assert(sizeof(sketch::int16_t) == 2);
sk_static_assert(sizeof(sketch::uint16_t) == 2);
sk_static_assert(sizeof(sketch::int32_t) == 4);
sk_static_assert(sizeof(sketch::uint32_t) == 4);
sk_static_assert(sizeof(sketch::int64_t) == 8);
sk_static_assert(sizeof(sketch::uint64_t) == 8);

sk_static_assert(sizeof(sketch::intptr_t) == sizeof(void*));
sk_static_assert(sizeof(sketch::uintptr_t) == sizeof(void*));

sk_static_assert(sizeof(sketch::ptrdiff_t) == sizeof(void*));
sk_static_assert(sizeof(sketch::ssize_t) == sizeof(void*));
