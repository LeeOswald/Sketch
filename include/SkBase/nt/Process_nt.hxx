#pragma once

namespace sketch
{

namespace process
{

namespace current
{

inline id_t id() noexcept
{
    return static_cast<id_t>(reinterpret_cast<uintptr_t>(PsGetCurrentProcessId()));
}

} // namespace current {}

} // namespace process {}

} // namespace sketch {}
