#pragma once

#include "../Atomic.hxx"
#include "../Thread.hxx"


namespace sketch
{


class Spinlock
    : private NonCopyable
{
public:
    class Guard
        : public NonCopyable
    {
    public:
        inline ~Guard() noexcept
        {
            m_.unlock();
        }

        inline explicit Guard(Spinlock& s) noexcept
            : m_(s)
        {
            m_.lock();
        }

    private:
        Spinlock& m_;
    };
	
public:
    inline ~Spinlock() noexcept
    {
    }

    inline Spinlock() noexcept
        : m_lock(0)
#ifdef SKETCH_DBG
        , m_threadId(0)
#endif
    {
    }

    inline void lock() noexcept
    {
#ifdef SKETCH_DBG
        SK_ASSERT(m_threadId != thread::current::id());
#endif
        if (atomic::compare_exchange(&m_lock, 1, 0) != 0)
        {
            do
            {
                while (m_lock != 0)
                {
                    thread::current::yield();
                }
            } while (atomic::compare_exchange(&m_lock, 1, 0) != 0);
        }

#ifdef SKETCH_DBG
        m_threadId = thread::current::id();
#endif
    }

    inline void unlock() noexcept
    {
#ifdef SKETCH_DBG
        SK_ASSERT(m_threadId == thread::current::id());
        m_threadId = 0;
#endif

        atomic::release::store(&m_lock, 0);
    }

private:
    atomic_t m_lock;
#ifdef SKETCH_DBG
    thread::id_t m_threadId;
#endif
};


} // namespace sketch {}
