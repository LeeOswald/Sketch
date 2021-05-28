#pragma once

#include "../Sketch.hxx"


#if defined (SKETCH_WIN_UM)
#include "win32/Heap.Generic_win32.hxx"
#elif defined (SKETCH_WIN_KM)
#include "nt/Heap.Generic_nt.hxx"
#endif


namespace sketch
{

#if defined (SKETCH_WIN_UM)
typedef win32::GenericHeap GenericHeap;
#elif defined (SKETCH_WIN_KM)
typedef nt::GenericHeap<NonPagedPoolNx> GenericHeap;
#endif

} // namespace sketch {}
