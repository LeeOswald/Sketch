#include <SkService/win32/ServiceApplication_win32.hxx>
#include <SkBase/init.hxx>

#include <crtdbg.h>


namespace sketch
{

namespace win32
{

ServiceApplication* ServiceApplication::s_instance = nullptr;

int ServiceApplication::StaticWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept
{
    ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

#ifdef SKETCH_DBG
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif

    int result = -1;
    if (!sketch::SkBase::initialize())
    {
        return result;
    }

    auto o = instance();
    SK_ASSERT(o);
    if (o)
    {
        result = o->winMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    sketch::SkBase::finalize();

    return result;
}

int ServiceApplication::winMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept
{
    SK_UNUSED(hInstance);
    SK_UNUSED(hPrevInstance);
    SK_UNUSED(lpCmdLine);
    SK_UNUSED(nCmdShow);

    auto name = serviceName();
    SK_ASSERT(name && *name);

    SERVICE_TABLE_ENTRYW svcTable[] =
    {
        { const_cast<WCHAR*>(name), (LPSERVICE_MAIN_FUNCTIONW)_ServiceMainStatic },
        { nullptr, nullptr }
    };

    if (!::StartServiceCtrlDispatcherW(svcTable))
    {
        auto e = ::GetLastError();

        if (e == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
        {
            setLastError(SkWin32Error(e, L"This application can be started by Service Control Manager only"));
        }
        else
        {
            setLastError(SkWin32Error(e, String()));
        }

        logError(lastError());

        return false;
    }

    return true;
}

void WINAPI ServiceApplication::_ServiceMainStatic(DWORD argc, LPWSTR *argv)
{
    auto o = instance();
    SK_ASSERT(o);
    if (o)
    {
        o->serviceMain(argc, argv);
    }
}

void ServiceApplication::serviceMain(DWORD argc, LPWSTR *argv) noexcept
{
    SK_UNUSED(argc);
    SK_UNUSED(argv);

    auto name = serviceName();
    SK_ASSERT(name && *name);

    log::g_ModuleName = moduleName();

    SK_ASSERT(!m_statusHandle);
    m_statusHandle = RegisterServiceCtrlHandlerExW(name, _ServiceCtrlHandlerExStatic, this);
    if (!m_statusHandle)
    {
        setLastError(SkWin32ErrorFromLastError(String()));
        logError(lastError());

        return;
    }

    if (!start())
    {
        _reportState(SERVICE_STOPPED);
    }
}

bool ServiceApplication::start() noexcept
{
    _reportState(SERVICE_RUNNING);

    return true;
}

DWORD WINAPI ServiceApplication::_ServiceCtrlHandlerExStatic(
    DWORD dwControl,
    DWORD dwEventType,
    LPVOID lpEventData,
    LPVOID lpContext
)
{
    DWORD result = ERROR_CALL_NOT_IMPLEMENTED;
    auto o = static_cast<ServiceApplication*>(lpContext);
    SK_ASSERT(o);

    if (o)
    {
        result = o->serviceCtrlHandlerEx(dwControl, dwEventType, lpEventData, lpContext);
    }

    return result;
}

DWORD ServiceApplication::serviceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) noexcept
{
    dwEventType;
    lpEventData;
    lpContext;

    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
        return ctrlStop();

    case SERVICE_CONTROL_INTERROGATE:
        // the handler should always return NO_ERROR
        return NO_ERROR;
    }

    return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD ServiceApplication::ctrlStop() noexcept
{
    _reportState(SERVICE_STOPPED);

    return NO_ERROR;
}

void ServiceApplication::_reportState(uint32_t state, uint32_t waitMsec, uint32_t checkpoint, int32_t errorCode) noexcept
{
    SERVICE_STATUS ss = { 0 };
    ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ss.dwCurrentState = state;

    if (state == SERVICE_START_PENDING)
    {
        ss.dwControlsAccepted = 0;
    }
    else
    {
        ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    ss.dwWin32ExitCode = (errorCode == 0) ? NO_ERROR : ERROR_SERVICE_SPECIFIC_ERROR;
    ss.dwServiceSpecificExitCode = errorCode;
    ss.dwCheckPoint = checkpoint;
    ss.dwWaitHint = waitMsec;

    if (!SetServiceStatus(m_statusHandle, &ss))
    {
        setLastError(SkWin32ErrorFromLastError(L"Failed to report service status"));
        logError(lastError());
    }
}


} // namespace win32 {}

} // namespace sketch {}



extern "C" int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return sketch::win32::ServiceApplication::StaticWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
