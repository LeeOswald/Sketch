#pragma once


#include "../Sketch.hxx"


#if defined(SKETCH_WIN_KM)
#include "nt/RwLock_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/RwLock_win32.hxx"
#endif



namespace sketch
{

class RwLock
    : public NonCopyable
{
public:
    template <bool _Exclusive>
    class Guard
        : public NonCopyable
    {
    public:
        inline ~Guard() noexcept
        {
            if (_Exclusive)
                m_.releaseExclusive();
            else
                m_.releaseShared();
        }

        inline explicit Guard(RwLock& s) noexcept
            : m_(s)
        {
            if (_Exclusive)
                m_.acquireExclusive();
            else
                m_.acquireShared();
        }

    private:
        RwLock& m_;
    };

    typedef Guard<true> ExclusiveGuard;
    typedef Guard<false> SharedGuard;

public:
    inline ~RwLock() noexcept
    {
    }

    inline RwLock() noexcept
    {
    }

    inline void acquireExclusive() noexcept
    {
        m_impl.acquireExclusive();
    }

    inline void acquireShared() noexcept
    {
        m_impl.acquireShared();
    }

    inline void releaseExclusive() noexcept
    {
        m_impl.releaseExclusive();
    }

    inline void releaseShared() noexcept
    {
        m_impl.releaseShared();
    }

private:
    internal::RwLockImpl m_impl;
};


} // namespace sketch {}
