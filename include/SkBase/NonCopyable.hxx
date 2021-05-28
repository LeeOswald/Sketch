#pragma once

#include "../Sketch.hxx"

namespace sketch
{

class NonCopyable
{
public:
    inline NonCopyable() noexcept
    {
    }

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};


} // namespace sketch {}


