#include <Sketch.hxx>
#include <SkBase/Event.hxx>
#include <SkBase/Thread.hxx>


namespace sketch
{

namespace thread
{

namespace current
{

void sleep(uint32_t msec) noexcept
{
    LARGE_INTEGER to;
    to.QuadPart = -1;
    to.QuadPart *= msec * 10000;

    SK_VERIFY(NT_SUCCESS(KeDelayExecutionThread(KernelMode, FALSE, &to)));
}

bool sleepAlertable(uint32_t msec) noexcept
{
    LARGE_INTEGER to;
    to.QuadPart = -1;
    to.QuadPart *= msec * 10000;

    auto status = KeDelayExecutionThread(KernelMode, TRUE, &to);
    SK_ASSERT(NT_SUCCESS(status));

    return ((status == STATUS_ALERTED) || (status == STATUS_USER_APC));
}

void setPriority(priority p) noexcept
{
    int pr = 0;
    switch (p)
    {
    case priority::low: pr = LOW_REALTIME_PRIORITY - 2; break;
    case priority::normal: pr = LOW_REALTIME_PRIORITY; break;
    case priority::high: pr = LOW_REALTIME_PRIORITY + 2; break;
    case priority::realtime: pr = HIGH_PRIORITY; break;
    default:
        SK_ASSERT(0);
    }

    KeSetPriorityThread(PsGetCurrentThread(), pr);
}

void setAffinity(affinity_t a) noexcept
{
    KeSetSystemAffinityThread(a);
}

void setName(const char* name) noexcept
{
    SK_UNUSED(name);
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
            ZwClose(m_h);
        }

        if (m_obj)
        {
            ObDereferenceObject(m_obj);
        }
    }

    inline explicit ThreadImpl(ISimpleDelegate* task, void* ctx, const char* name = nullptr) noexcept
        : m_h(0)
        , m_obj(nullptr)
        , m_id(0)
        , m_joined(false)
        , m_threadStartEvent(Event::manualReset)
        , m_delegate(task)
        , m_ctx(ctx)
    {
        SK_ASSERT(m_delegate);

        SK_UNUSED(name);

        auto status = PsCreateSystemThread(
            &m_h,
            THREAD_ALL_ACCESS,
            nullptr,
            0,
            nullptr,
            _threadProcStatic,
            this
        );

        if (NT_SUCCESS(status))
        {
            m_threadStartEvent.wait();
        }
    }

    inline bool valid() const noexcept
    {
        return !!m_obj;
    }

    inline thread::id_t id() const noexcept
    {
        return m_id;
    }

    inline void join() noexcept
    {
        if (valid())
        {
            SK_ASSERT(thread::current::id() != id());
            if (thread::current::id() == id())
            {
                return;
            }

            m_threadStartEvent.wait();
            SK_VERIFY(KeWaitForSingleObject(m_obj, Executive, KernelMode, FALSE, nullptr) == STATUS_SUCCESS);
            m_joined = true;
        }
    }

    inline void alert() noexcept
    {
        SK_ASSERT(m_h);
        ZwAlertThread(m_h);
    }

private:
    static void __stdcall _threadProcStatic(void* arg)
    {
        auto _this = static_cast<ThreadImpl*>(arg);
        _this->_run();

        PsTerminateSystemThread(0);
    }

    void _run()
    {
        SK_VERIFY(NT_SUCCESS(ObReferenceObjectByHandle(
            m_h,
            THREAD_ALL_ACCESS,
            nullptr,
            KernelMode,
            &m_obj,
            nullptr
        )));

        m_threadStartEvent.set();
        if (!m_obj)
        {
            return; // not much we can do
        }

        m_id = thread::current::id();

        SK_ASSERT(m_delegate);
        m_delegate->run(m_ctx);
    }

private:
    HANDLE m_h;
    void* m_obj;
    thread::id_t m_id;
    bool m_joined;
    Event m_threadStartEvent;
    ISimpleDelegate* m_delegate;
    void* m_ctx;
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
    return m_impl->id();
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
