#include <SkTest/SkTest.hxx>
#include <SkBase/Trace.NetSink.hxx>

#include "../TestHarness.hxx"


namespace sketch
{

namespace test
{

NTSTATUS TestDriver::driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath) noexcept
{
    SK_UNUSED(driverObject);
    SK_UNUSED(registryPath);

    if (!sketch::log::initialize())
    {
        return STATUS_UNSUCCESSFUL;
    }

    sketch::log::registerSink("Debugger", sketch::log::TraceSink0::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));

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
            harness.reset();
        }
    }

    sketch::log::finalize();

    return STATUS_UNSUCCESSFUL;
}

void TestDriver::driverUnload(DRIVER_OBJECT* driverObject) noexcept
{
    SK_UNUSED(driverObject);
}


} // namespace test {}

} // namespace sketch {}

