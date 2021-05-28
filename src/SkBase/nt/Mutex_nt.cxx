#include <SkBase/Mutex.hxx>
#include <SkBase/Thread.hxx>

namespace sketch
{

namespace internal
{


class MutexImpl
    : public NonCopyable
{
public:
    inline ~MutexImpl() noexcept
    {
    }

    inline MutexImpl() noexcept
#ifdef SKETCH_DBG
        : m_threadId(0)
#endif    
    {
        ExInitializeFastMutex(&m_lock);
    }

    inline void lock() noexcept
    {
#ifdef SKETCH_DBG
        SK_ASSERT(m_threadId != thread::current::id());
#endif
        
        ExAcquireFastMutex(&m_lock);
        
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
        ExReleaseFastMutex(&m_lock);
    }

private:
    FAST_MUTEX m_lock;
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

