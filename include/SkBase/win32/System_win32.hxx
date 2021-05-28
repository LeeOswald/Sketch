#pragma once


namespace sketch
{

namespace system
{


inline bool isDebuggerPresent() noexcept
{
    return !!::IsDebuggerPresent();
}


} // namespace system {}

} // namespace sketch {}
