#pragma once

namespace sketch
{

namespace internal
{

struct Stopwatch
    : public StopwatchBase
{
    __inline void start() noexcept
    {
        SK_ASSERT(started == 0);
        LARGE_INTEGER li = KeQueryPerformanceCounter(nullptr);
        started = li.QuadPart;
    }

    __inline void stop() noexcept
    {
        SK_ASSERT(started > 0);
        LARGE_INTEGER li = KeQueryPerformanceCounter(nullptr);
        value += (li.QuadPart - started);
        started = 0;
    }

    __inline void reset() noexcept
    {
        value = 0;
    }

    __inline value_t seconds() noexcept
    {
        return value / frequency();
    }

    __inline value_t milliseconds() noexcept
    {
        return (value * 1000) / frequency();
    }

    __inline value_t microseconds() noexcept
    {
        return (value * 1000 * 1000) / frequency();
    }

    static value_t frequency() noexcept
    {
        static value_t _f = 0;
        if (_f == 0)
        {
            LARGE_INTEGER li = { 0 };
            KeQueryPerformanceCounter(&li);
            _f = li.QuadPart;
        }

        return _f;
    }
};


} // namespace internal {}

} // namespace sketch {}
