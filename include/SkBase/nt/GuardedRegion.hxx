#pragma once

#include "../../Sketch.hxx"


namespace sketch
{

namespace nt
{

// GuardedRegion routine enters a guarded region, which disables
// all kernel-mode APC delivery to the current thread

class GuardedRegion
    : public NonCopyable
{
public:
    ~GuardedRegion() noexcept
    {
        KeLeaveGuardedRegion();
    }

    GuardedRegion() noexcept
    {
        KeEnterGuardedRegion();
    }
};

} // namespace nt {}

} // namespace sketch {}
