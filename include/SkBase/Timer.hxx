#pragma once


#include "../Sketch.hxx"
#include "IDelegate.hxx"
#include "TimeSpan.hxx"

#if defined SKETCH_WIN_UM
#include "win32/Timer_win32.hxx"
#elif defined SKETCH_WIN_KM
#include "nt/Timer_nt.hxx"
#endif


namespace sketch
{

class Timer
    : public NonCopyable
{
public:
    inline ~Timer() noexcept
    {
    }

    inline explicit Timer(ISimpleDelegate* delegate, void* ctx) noexcept
        : m_impl(delegate, ctx)
    {
    }

    inline bool valid() const noexcept
    {
        return m_impl.valid();
    }

    inline void cancel() noexcept
    {
        m_impl.cancel();
    }

    inline bool singleShot(TimeSpan when) noexcept
    {
        return m_impl.singleShot(when);
    }

    inline bool periodic(TimeSpan period) noexcept
    {
        return m_impl.periodic(period);
    }

private:
    internal::TimerImpl m_impl;
};


} // namespace sketch {}
