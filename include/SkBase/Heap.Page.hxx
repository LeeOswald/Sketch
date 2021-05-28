#pragma once

#include "../Sketch.hxx"


#if defined (SKETCH_WIN_UM)
#include "win32/Heap.Page_win32.hxx"
#elif defined (SKETCH_WIN_KM)
#include "nt/Heap.Page_nt.hxx"
#endif


namespace sketch
{

#if defined (SKETCH_WIN_UM)

template <bool _Executable = false>
using PageHeap = win32::PageHeap<_Executable>;

#elif defined (SKETCH_WIN_KM)

template <bool _Executable = false>
using PageHeap = nt::PageHeap<_Executable>;

#endif

} // namespace sketch {}
