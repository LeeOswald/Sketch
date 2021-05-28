#include "common.h"

#include <SkBase/Timer.hxx>
#include <SkBase/Thread.hxx>


using namespace sketch;


struct TimerDelegateOne
    : public ISimpleDelegate
{
    TimerDelegateOne()
    {
        SketchInfo("TimerDelegateOne::TimerDelegateOne()");
    }

    ~TimerDelegateOne()
    {
        SketchInfo("TimerDelegateOne::~TimerDelegateOne()");
    }

    void ISimpleDelegate::run(void* ctx) override
    {
        SK_UNUSED(ctx);
        SketchInfo("TimerDelegateOne::run()");
    }
};


struct TimerDelegatePeriodic
    : public ISimpleDelegate
{
    TimerDelegatePeriodic()
    {
        SketchInfo("TimerDelegatePeriodic::TimerDelegatePeriodic()");
    }

    ~TimerDelegatePeriodic()
    {
        SketchInfo("TimerDelegatePeriodic::~TimerDelegatePeriodic()");
    }

    void ISimpleDelegate::run(void* ctx) override
    {
        SK_UNUSED(ctx);
        SketchInfo("TimerDelegatePeriodic::run()");
    }
};


SKETCH_TEST_BEGIN(Timer, "Timer::singleShot()")
    TimerDelegateOne d;
    Timer t(&d, nullptr);
    t.singleShot(TimeSpan(0, 0, 0, 500));
    thread::current::sleep(1000);
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(Timer, "Timer::singleShot()")
    TimerDelegatePeriodic d;
    Timer t(&d, nullptr);
    t.periodic(TimeSpan(0, 0, 0, 500));
    thread::current::sleep(2000);
    t.cancel();
SKETCH_TEST_END()
