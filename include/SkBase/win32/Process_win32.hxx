#pragma once

namespace sketch
{

namespace process
{

namespace current
{

inline id_t id() noexcept
{
    return ::GetCurrentProcessId();
}

} // namespace current {}

} // namespace process {}

} // namespace sketch {}
