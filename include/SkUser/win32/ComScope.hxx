#pragma once

#include "../SkUser.hxx"

#include <Objbase.h>


namespace sketch
{

namespace win32
{


// CoInitailize/CoUninitialize wrapper

class ComScope
    : public NonCopyable
    , public ErrorState
{
public:
    ~ComScope() noexcept
    {
        if (!anyErrors())
        {
            ::CoUninitialize();
        }
    }

    ComScope(uint32_t flags = COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY)
    {
        auto hr = ::CoInitializeEx(nullptr, flags);
        if (FAILED(hr))
        {
            SK_ASSERT(0);
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
        }
    }
};



} // namespace win32 {}

} // namespace sketch {}
