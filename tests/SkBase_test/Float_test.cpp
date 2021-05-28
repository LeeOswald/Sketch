#include "common.h"

#include <math.h>

using namespace sketch;

#ifdef SKETCH_WIN_KM

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

SKETCH_TEST_BEGIN(Float_sincos, "Float_sincos")

    nt::FloatingPointSaveScope fp;

    auto f = sin(M_PI);
    auto i = static_cast<int>(f);
    SketchInfo("sin(M_PI) = %d", i);

    f = cos(M_PI);
    i = static_cast<int>(f);
    SketchInfo("cos(M_PI) = %d", i);

SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Float_lrint, "Float_lrint")

    nt::FloatingPointSaveScope fp;

    auto l = lrint(-2.3);
    SketchInfo("lrint(-2.3) = %d", l);

    l = lrint(2.3);
    SketchInfo("lrint(2.3) = %d", l);

    l = lrint(-2.9);
    SketchInfo("lrint(-2.9) = %d", l);

    l = lrint(2.9);
    SketchInfo("lrint(2.9) = %d", l);

SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Float_lrintf, "Float_lrintf")

    nt::FloatingPointSaveScope fp;

    auto l = lrintf(-2.3f);
    SketchInfo("lrintf(-2.3f) = %d", l);

    l = lrint(2.3f);
    SketchInfo("lrintf(2.3f) = %d", l);

    l = lrint(-2.9f);
    SketchInfo("lrintf(-2.9f) = %d", l);

    l = lrint(2.9f);
    SketchInfo("lrint(2.9f) = %d", l);

SKETCH_TEST_END()


#endif // SKETCH_WIN_KM
