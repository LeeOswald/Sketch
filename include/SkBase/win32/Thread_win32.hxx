#pragma once


namespace sketch
{

namespace thread
{

namespace current
{

inline id_t id() noexcept
{
    return ::GetCurrentThreadId();
}

inline void yield() noexcept
{
    YieldProcessor();
}

inline uint32_t currentProcessor() noexcept
{
    return ::GetCurrentProcessorNumber();
}


} // namespace current {}

} // namespace thread {}

} // namespace sketch {}
