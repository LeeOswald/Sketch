#include "common.h"

#include <SkBase/Stopwatch.hxx>
#include <SkBase/Thread.hxx>

using namespace sketch;


SKETCH_TEST_BEGIN(Stopwatch, "Stopwatch::Stopwatch()")
    Stopwatch sw;

    sw.start();
    thread::current::sleep(1000);
    sw.stop();

    sw.start();
    thread::current::sleep(500);
    sw.stop();

    SketchDebug("Sleeped 1.5 sec; measured %I64d seconds / %I64d milliseconds / %I64d microseconds",
        sw.seconds(),
        sw.milliseconds(),
        sw.microseconds()
        );
SKETCH_TEST_END()

