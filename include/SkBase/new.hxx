#pragma once

#include "../Sketch.hxx"

#if defined(SKETCH_STL_AVAILABLE)

#include <new>

namespace sketch
{

using std::nothrow_t;
using std::nothrow;

} // namespace sketch {}


#elif defined(SKETCH_WIN_KM)

#include "nt/new_nt.hxx"

#endif

