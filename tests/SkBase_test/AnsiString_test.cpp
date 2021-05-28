#include "common.h"

using namespace sketch;

SKETCH_TEST_BEGIN(AnsiString, "AnsiString::AnsiString()")
    AnsiString s;
    SKETCH_TEST_ASSERT(s.length() == 0);
    SKETCH_TEST_ASSERT(s.empty());
    SKETCH_TEST_ASSERT(*s.str() == 0);
SKETCH_TEST_END()
