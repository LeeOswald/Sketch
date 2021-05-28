#pragma once


namespace sketch
{

namespace system
{

inline uint32_t processorCount() noexcept
{
    return KeNumberProcessors;
}

inline bool isDebuggerPresent() noexcept
{
    return (KdRefreshDebuggerNotPresent() == FALSE);
}


} // namespace system {}

} // namespace sketch {}
