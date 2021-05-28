#pragma once

#include "../Sketch.hxx"


#ifdef SKETCH_WIN_KM

#define SK_ASSERT(x)    NT_ASSERT(x)

#else

#include <cassert>

#define SK_ASSERT(x)    assert(x)

#endif


#ifdef SKETCH_DBG
#define SK_VERIFY(x)    SK_ASSERT(x)
#else
#define SK_VERIFY(x)    (x)
#endif
