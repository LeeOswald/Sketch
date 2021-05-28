#pragma once

#include "../../Sketch.hxx"


namespace sketch
{

namespace nt
{

// CriticalRegion temporarily disables the execution of normal kernel APCs,
// but does not prevent special kernel APCs from running

class CriticalRegion
    : public NonCopyable
{
public:
    ~CriticalRegion() noexcept
    {
        KeLeaveCriticalRegion();
    }

    CriticalRegion() noexcept
    {
        KeEnterCriticalRegion();
    }
};

} // namespace nt {}

} // namespace sketch {}
