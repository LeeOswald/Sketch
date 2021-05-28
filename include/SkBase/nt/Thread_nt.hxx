#pragma once


namespace sketch
{

namespace thread
{

namespace current
{


inline id_t id() noexcept
{
    return (id_t)(uintptr_t)PsGetCurrentThreadId();
}

inline void yield() noexcept
{
    ZwYieldExecution();
}

inline uint32_t currentProcessor() noexcept
{
    return KeGetCurrentProcessorNumber();
}


} // namespace current {}

} // namespace thread {}

} // namespace sketch {}
