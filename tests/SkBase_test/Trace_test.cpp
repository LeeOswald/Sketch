#include "common.h"


using namespace sketch;


SKETCH_TEST_BEGIN(Trace, "Trace")
    SketchDebug("Some Debug message");
    SketchInfo("Some Info message");
    SketchWarning("Some Warning message");
    SketchError("Some Error message");

    SketchInfo("Some message with [%s]", "UTF-8 string");
    SketchInfo("Some message with [%S]", L"UTF-16 string");
SKETCH_TEST_END()
