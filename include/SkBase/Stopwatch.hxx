#pragma once

#include "../Sketch.hxx"


namespace sketch
{

struct StopwatchBase
{
    typedef uint64_t value_t;

    value_t value{ 0 };
    value_t started{ 0 };
};


} // namespace sketch {}

#if defined(SKETCH_WIN_KM)
#include "nt/Stopwatch_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/Stopwatch_win32.hxx"
#endif

namespace sketch
{

#if defined(SKETCH_WIN_KM)
typedef internal::Stopwatch Stopwatch;
#elif defined(SKETCH_WIN_UM)
typedef internal::Stopwatch Stopwatch;
#endif

} // namespace sketch {}
