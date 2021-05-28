#include <SkBase/Mutex.hxx>
#include <SkBase/Thread.hxx>

namespace sketch
{

namespace internal
{


class MutexImpl
    : private NonCopyable
{
public:
    inline ~MutexImpl() noexcept
    {
        DeleteCriticalSection(&m_lock);
    }

    inline MutexImpl() noexcept
#ifdef SKETCH_DBG
        : m_threadId(0)
#endif
    {
        if (!InitializeCriticalSectionAndSpinCount(&m_lock, 4000))
        {
            ::abort(); // TODO: not much we can do in this case
        }
    }

    inline void lock() noexcept
    {
#ifdef SKETCH_DBG
        SK_ASSERT(m_threadId != thread::current::id());
#endif

        EnterCriticalSection(&m_lock);

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

        LeaveCriticalSection(&m_lock);
    }

private:
    CRITICAL_SECTION m_lock;
#ifdef SKETCH_DBG
    thread::id_t m_threadId;
#endif
};


} // namespace internal {}


Mutex::~Mutex() noexcept
{
}

Mutex::Mutex() noexcept
    : m_impl(new (sketch::nothrow) internal::MutexImpl())
{
}

Mutex::Mutex(Mutex&& other) noexcept
    : m_impl(sketch::move(other.m_impl))
{
}

Mutex& Mutex::operator=(Mutex&& other) noexcept
{
    m_impl = sketch::move(other.m_impl);
    return *this;
}

bool Mutex::valid() const noexcept
{
    return !!m_impl;
}

void Mutex::lock() noexcept
{
    SK_ASSERT(valid());
    m_impl->lock();
}

void Mutex::unlock() noexcept
{
    SK_ASSERT(valid());
    m_impl->unlock();
}

} // namespace sketch {}
