#pragma once

#include "../Sketch.hxx"

#if defined(SKETCH_WIN_KM)
#include "nt/Spinlock_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/Spinlock_win32.hxx"
#endif



