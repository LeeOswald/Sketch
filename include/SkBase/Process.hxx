#pragma once

#include "../Sketch.hxx"


namespace sketch
{

namespace process
{

typedef uint32_t id_t;

namespace current
{

id_t id() noexcept;

} // namespace current {}

} // namespace process {}

} // namespace sketch {}


#if defined(SKETCH_WIN_KM)
#include "nt/Process_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/Process_win32.hxx"
#endif
