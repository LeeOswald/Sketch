#pragma once

#include <Sketch.hxx>

namespace sketch
{

namespace win32
{


class Dll
    : private NonCopyable
    , public ErrorState
{
public:
    static BOOL StaticDllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) noexcept;

    ~Dll() noexcept
    {
        SK_ASSERT(s_instance == this);
        s_instance = nullptr;
    }

    static Dll* instance() noexcept
    {
        return s_instance;
    }

    HMODULE base() const noexcept
    {
        return m_base;
    }

    virtual const char* moduleName() const noexcept = 0;
    virtual BOOL dllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) noexcept = 0;

protected:
    Dll() noexcept
        : ErrorState(ErrorState::Mode::neverThrow)
    {
        SK_ASSERT(!s_instance);
        s_instance = this;
    }

private:
    BOOL _dllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) noexcept;

private:
    static Dll* s_instance;

    HMODULE m_base;
};

} // namespace win32 {}

} // namespace sketch {}
