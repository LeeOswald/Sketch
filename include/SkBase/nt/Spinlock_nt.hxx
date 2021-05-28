#pragma once

#include "../Thread.hxx"


namespace sketch
{


class Spinlock
    : public NonCopyable
{
public:
	class Guard
        : private sketch::NonCopyable
    {
		friend class Spinlock;

    public:
        inline ~Guard() noexcept
        {
            m_.unlock(*this);
        }

        inline explicit Guard(Spinlock& s) noexcept
			: m_(s)
        {
            m_.lock(*this);
        }

    private:
		Spinlock& m_;
        KSPIN_LOCK* m_lock;
		KLOCK_QUEUE_HANDLE m_queue;
    };


    inline ~Spinlock() noexcept
    {
    }

    inline Spinlock() noexcept
    {
        KeInitializeSpinLock(&m_spinLock);
    }

    inline void lock(Guard& g) noexcept
    {
		g.m_lock = &m_spinLock;
        KeAcquireInStackQueuedSpinLock(g.m_lock, &g.m_queue);
    }

    inline void unlock(Guard& g) noexcept
    {
        KeReleaseInStackQueuedSpinLock(&g.m_queue);
    }

private:
    KSPIN_LOCK m_spinLock;

};


} // namespace sketch {}
