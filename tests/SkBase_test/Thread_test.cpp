#include "common.h"

#include <SkBase/Thread.hxx>
#include <SkBase/System.hxx>


using namespace sketch;


SKETCH_TEST_BEGIN(Thread, "thread")
    {
        auto ov = system::getOsVersion();
        SketchInfo("Detected OS version %d.%d.%d SP %d.%d", ov.versionMajor, ov.versionMinor, ov.build, ov.spMajor, ov.spMinor);
    }

    SketchInfo("Page size: %d", system::pageSize());
    SketchInfo("Debugger present: %s", system::isDebuggerPresent() ? "YES" : "NO");
    SketchInfo("Processor count: %d", system::processorCount());
    SketchInfo("Current thread Id: %d", thread::current::id());
    SketchInfo("Current processor: %d", thread::current::currentProcessor());
SKETCH_TEST_END()


struct Delegate
    : public ISimpleDelegate
{
    Delegate()
    {
        SketchInfo("Delegate::Delegate()");
    }

    ~Delegate()
    {
        SketchInfo("Delegate::~Delegate()");
    }

    void ISimpleDelegate::run(void* ctx) override
    {
        SK_UNUSED(ctx);

        SketchInfo("Delegate::run() started on thread %d", thread::current::id());

        for (int i = 0; i < 5; i++)
        {
            SketchInfo("thinking...");
            thread::current::sleep(500);
        }

        SketchInfo("Delegate::run() finished");
    }
};

SKETCH_TEST_BEGIN(Thread, "Thread::Thread(ISimpleDelegate*, const char*)")
    Delegate d;
    Thread t(&d, "Test Thread 0");
    SketchInfo("Started thread %d", t.id());
    SKETCH_TEST_ASSERT(t.valid());

    t.join();

    SketchInfo("Stopped thread %d", t.id());

SKETCH_TEST_END()


