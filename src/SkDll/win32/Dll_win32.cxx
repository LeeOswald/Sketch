#include <SkDll/win32/Dll_win32.hxx>
#include <SkBase/init.hxx>

namespace sketch
{

namespace win32
{

Dll* Dll::s_instance = nullptr;

BOOL Dll::StaticDllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved) noexcept
{
    if (dwReasonForCall == DLL_PROCESS_ATTACH)
    {
        if (!sketch::SkBase::initialize())
        {
            return FALSE;
        }
    }

    auto _instance = instance();
    SK_ASSERT(_instance);
    if (!_instance)
    {
        return FALSE;
    }

    auto result = _instance->_dllMain(hModule, dwReasonForCall, lpReserved);

    if (dwReasonForCall == DLL_PROCESS_DETACH)
    {
        sketch::SkBase::finalize();
    }

    return result;
}

BOOL Dll::_dllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved) noexcept
{
    if (dwReasonForCall == DLL_PROCESS_ATTACH)
    {
        m_base = hModule;

        if (lpReserved) // only set log::g_ModuleName during static DLL load, so that the process can override it later
        {
            log::g_ModuleName = moduleName();
        }
    }

    return dllMain(hModule, dwReasonForCall, lpReserved);
}


} // namespace win32 {}

} // namespace sketch {}


extern "C" BOOL DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
    return sketch::win32::Dll::StaticDllMain(hModule, dwReasonForCall, lpReserved);
}
