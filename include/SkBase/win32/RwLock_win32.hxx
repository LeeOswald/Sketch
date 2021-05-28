#pragma once


namespace sketch
{

namespace internal
{

class RwLockImpl
    : private NonCopyable
{
public:
    inline ~RwLockImpl() noexcept
    {
    }

    inline RwLockImpl() noexcept
    {
        InitializeSRWLock(&m_lock);
    }

    inline void acquireExclusive() noexcept
    {
        AcquireSRWLockExclusive(&m_lock);
    }

    inline void acquireShared() noexcept
    {
        AcquireSRWLockShared(&m_lock);
    }

    inline void releaseExclusive() noexcept
    {
        ReleaseSRWLockExclusive(&m_lock);
    }

    inline void releaseShared() noexcept
    {
        ReleaseSRWLockShared(&m_lock);
    }

private:
    SRWLOCK m_lock;
};

} // namespace internal {}

} // namespace sketch {}
