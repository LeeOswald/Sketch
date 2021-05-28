#pragma once

#include <Sketch.hxx>


namespace sketch
{

namespace win32
{


class ServiceApplication
    : public NonCopyable
    , public ErrorState
{
public:
    ~ServiceApplication() noexcept
    {
        SK_ASSERT(s_instance == this);
        s_instance = nullptr;
    }

    static ServiceApplication* instance() noexcept
    {
        return s_instance;
    }

    static int StaticWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept;

    virtual const char* moduleName() const noexcept = 0;
    virtual const wchar_t* serviceName() const noexcept = 0;
    virtual int winMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept;
    virtual void serviceMain(DWORD argc, LPWSTR *argv) noexcept;
    virtual DWORD serviceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) noexcept;

    virtual bool start() noexcept;
    virtual DWORD ctrlStop() noexcept;

protected:
    ServiceApplication() noexcept
        : ErrorState(ErrorState::Mode::neverThrow)
        , m_statusHandle(0)
    {
        SK_ASSERT(!s_instance);
        s_instance = this;
    }

    void _reportState(uint32_t state, uint32_t waitMsec = 0, uint32_t checkpoint = 0, int32_t errorCode = 0) noexcept;

private:
    static void WINAPI _ServiceMainStatic(DWORD argc, LPWSTR *argv);
    static DWORD WINAPI _ServiceCtrlHandlerExStatic(
        DWORD dwControl,
        DWORD dwEventType,
        LPVOID lpEventData,
        LPVOID lpContext
    );

private:
    SERVICE_STATUS_HANDLE m_statusHandle;

    static ServiceApplication* s_instance;
};


} // namespace win32 {}

} // namespace sketch {}
