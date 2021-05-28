#include <SkTest/SkTest.hxx>
#include <SkBase/Trace.NetSink.hxx>

#include "../TestHarness.hxx"


namespace sketch
{

namespace test
{

int TestApplication::main(int argc, wchar_t** argv) noexcept
{
    SK_UNUSED(argc);
    SK_UNUSED(argv);

    int result = -1;

    if (!sketch::log::initialize())
    {
        return result;
    }

    sketch::log::registerSink("Debugger", sketch::log::TraceSink0::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
    sketch::log::registerSink("StdOut", sketch::log::TraceSink1::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));

#if 1
    sketch::log::registerSink("Net", sketch::log::createUdpSink(L"127.0.0.1", 667), sketch::log::NullTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
#else
    sketch::log::registerSink("Net", sketch::log::createTcpSink(L"127.0.0.1", 667), sketch::log::NullTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
#endif

    {
        sketch::UniquePtr<TestHarness> harness(new (sketch::nothrow) TestHarness());
        if (harness && harness->valid())
        {
            harness->wait();
            result = 0;
        }
    }

    sketch::log::finalize();

    return result;
}


} // namespace test {}

} // namespace sketch {}


