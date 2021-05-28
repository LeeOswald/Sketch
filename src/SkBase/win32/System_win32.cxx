#include <SkBase/System.hxx>


namespace sketch
{

namespace system
{


uint32_t processorCount() noexcept
{
    static uint32_t _pc = uint32_t(-1);
    if (_pc == uint32_t(-1))
    {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);

        _pc = si.dwNumberOfProcessors;
    }
    
    return _pc;
}

OsVersion getOsVersion() noexcept
{
    OsVersion vi;
    RTL_OSVERSIONINFOEXW ow = { 0 };
    ow.dwOSVersionInfoSize = sizeof(ow);
    SK_VERIFY(NT_SUCCESS(RtlGetVersion((PRTL_OSVERSIONINFOW)&ow)));

    vi.versionMajor = ow.dwMajorVersion;
    vi.versionMinor = ow.dwMinorVersion;
    vi.build = ow.dwBuildNumber;
    vi.spMajor = ow.wServicePackMajor;
    vi.spMinor = ow.wServicePackMinor;

    return vi;
}

uint32_t pageSize() noexcept
{
    static uint32_t _ps = uint32_t(-1);
    if (_ps == uint32_t(-1))
    {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);

        _ps = si.dwPageSize;
    }
    
    return _ps;
}


} // namespace system {}

} // namespace sketch {}
