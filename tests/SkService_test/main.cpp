#include <Sketch.hxx>
#include <SkBase/Thread.hxx>
#include <SkUser/SkUser.hxx>

#include <SkService/win32/ServiceApplication_win32.hxx>


class TestService
    : public sketch::win32::ServiceApplication
    , private sketch::ISimpleDelegate
{
    typedef sketch::win32::ServiceApplication base;

public:
    ~TestService()
    {
    }

    TestService()
        : m_stopFlag(false)
    {
    }

    const char* moduleName() const noexcept override
    {
        return "SkService_test";
    }

    const wchar_t* serviceName() const noexcept override
    {
        return L"SkService_test";
    }

    int winMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept override
    {
        int result = -1;

        if (!sketch::log::initialize())
        {
            return result;
        }

#if 0
        sketch::log::registerSink("Debugger", sketch::log::TraceSink0::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
#else
        sketch::log::registerSink("Debugger", sketch::log::TraceSink2::make(), sketch::log::DefaultTraceFormatter::make(), sketch::log::DefaultTraceFilter::make(sketch::log::Debug));
#endif

        result = base::winMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

        sketch::log::finalize();

        return result;
    }

    bool start() noexcept override
    {
        _reportState(SERVICE_START_PENDING, 3000);

        m_stopFlag = false;
        m_thread = sketch::move(sketch::Thread(this, "ServiceThread"));
        if (!m_thread.valid())
        {
            _reportState(SERVICE_STOPPED);
            return false;
        }

        _reportState(SERVICE_RUNNING);

        SketchDebug("Service [%s] started", sketch::toSystemLocale(serviceName()).str());

        return true;
    }

    DWORD ctrlStop() noexcept override
    {
        _reportState(SERVICE_STOP_PENDING, 3000);

        m_stopFlag = true;
        m_thread.join();

        _reportState(SERVICE_STOPPED);

        SketchDebug("Service [%s] stopped", sketch::toSystemLocale(serviceName()).str());

        return NO_ERROR;
    }

    void sketch::ISimpleDelegate::run(void* ctx) override
    {
        SK_UNUSED(ctx);

        SketchDebug("Service thread started");

        while (!m_stopFlag)
        {
            SketchDebug("Service running");
            sketch::thread::current::sleep(1000);
        }

        SketchDebug("Service thread exited");
    }

private:
    bool m_stopFlag;
    sketch::Thread m_thread;
};

static TestService g_Service;


