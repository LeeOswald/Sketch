#include <Sketch.hxx>
#include <SkBase/Thread.hxx>

#include <process.h>


namespace sketch
{

namespace thread
{

namespace current
{

void sleep(uint32_t msec) noexcept
{
    ::Sleep(msec);
}

bool sleepAlertable(uint32_t msec) noexcept
{
    return (::SleepEx(msec, TRUE) == WAIT_IO_COMPLETION);
}

void setPriority(priority p) noexcept
{
    int pr = 0;
    switch (p)
    {
    case priority::low: pr = THREAD_PRIORITY_BELOW_NORMAL; break;
    case priority::normal: pr = THREAD_PRIORITY_NORMAL; break;
    case priority::high: pr = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case priority::realtime: pr = THREAD_PRIORITY_TIME_CRITICAL; break;
    default:
        SK_ASSERT(0);
    }

    SK_VERIFY(::SetThreadPriority(::GetCurrentThread(), pr));
}

void setAffinity(affinity_t a) noexcept
{
    ::SetThreadAffinityMask(::GetCurrentThread(), a);
}

const DWORD VCThreadNameMagic = 0x406D1388;
#pragma pack(push,8)
typedef struct
{
    DWORD dwType;      // Must be 0x1000.
    LPCSTR szName;     // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void setNameInternal(DWORD tid, const char* name)
{
    if (::IsDebuggerPresent())
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = tid;
        info.dwFlags = 0;

        __try
        {
            ::RaiseException(VCThreadNameMagic, 0, sizeof(info) / sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(&info));
        }
        __except (EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }
}

void setName(const char* name) noexcept
{
    if (name)
    {
        setNameInternal((DWORD)-1, name);
    }
}

} // namespace current {}

} // namespace thread {}

namespace internal
{

class ThreadImpl
    : public NonCopyable
{
public:
    inline ~ThreadImpl() noexcept
    {
        join();

        if (m_h)
        {
            ::CloseHandle(m_h);
        }
    }

    inline explicit ThreadImpl(ISimpleDelegate* task, void* ctx, const char* name = nullptr) noexcept
        : m_h(0)
        , m_id(0)
        , m_delegate(task)
        , m_ctx(ctx)
    {
        SK_ASSERT(m_delegate);

        if (name)
        {
            ::strncpy_s(m_name, _countof(m_name), name, _TRUNCATE);
            m_name[_countof(m_name) - 1] = 0;
        }
        else
        {
            m_name[0] = 0;
        }

        m_h = (HANDLE)::_beginthreadex(nullptr, 0, _threadProcStatic, this, 0, &m_id);
    }

    inline bool valid() const noexcept
    {
        return !!m_h;
    }

    inline thread::id_t id() const noexcept
    {
        return m_id;
    }

    inline void join() noexcept
    {
        if (m_h)
        {
            SK_ASSERT(thread::current::id() != id());
            if (thread::current::id() == id())
            {
                return;
            }

            SK_VERIFY(::WaitForSingleObject(m_h, INFINITE) == WAIT_OBJECT_0);
        }
    }

    inline void alert() noexcept
    {
        SK_ASSERT(m_h);
        ::QueueUserAPC(_APCProc, m_h, 0);
    }

private:
    static void CALLBACK _APCProc(ULONG_PTR dwParam)
    {
        SK_UNUSED(dwParam);
    }

    static unsigned __stdcall _threadProcStatic(void* arg)
    {
        auto _this = static_cast<ThreadImpl*>(arg);
        _this->_run();
        return 0;
    }

    void _run()
    {
        if (m_name[0])
        {
            thread::current::setName(m_name);
        }

        SK_ASSERT(m_delegate);
        m_delegate->run(m_ctx);
    }

private:
    HANDLE m_h;
    thread::id_t m_id;
    ISimpleDelegate* m_delegate;
    void* m_ctx;
    char m_name[32];
};

} // namespace internal {}


Thread::~Thread() noexcept
{
    if (valid())
    {
        join();
    }
}

Thread::Thread() noexcept
{
}

Thread::Thread(ISimpleDelegate* task, void* ctx, const char* name) noexcept
    : m_impl(new (sketch::nothrow) internal::ThreadImpl(task, ctx, name))
{
}

Thread::Thread(Thread&& other) noexcept
    : m_impl(sketch::move(other.m_impl))
{
}

Thread& Thread::operator=(Thread&& other) noexcept
{
    m_impl = sketch::move(other.m_impl);
    return *this;
}

thread::id_t Thread::id() const noexcept
{
    SK_ASSERT(valid());
    return m_impl ? m_impl->id() : 0;
}

bool Thread::valid() const noexcept
{
    return m_impl ? m_impl->valid() : false;
}

void Thread::join() noexcept
{
    SK_ASSERT(valid());
    m_impl->join();
}

void Thread::alert() noexcept
{
    SK_ASSERT(valid());
    m_impl->alert();
}


} // namespace sketch {}
