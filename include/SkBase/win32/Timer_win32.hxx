#pragma once


namespace sketch
{

namespace internal
{


class TimerImpl
    : private NonCopyable
{
public:
    ~TimerImpl() noexcept
    {
        cancel();

        if (m_queue)
        {
            SK_VERIFY(::DeleteTimerQueueEx(m_queue, INVALID_HANDLE_VALUE)); // wait for the callback
        }
    }

    explicit TimerImpl(ISimpleDelegate* delegate, void* ctx) noexcept
        : m_queue(::CreateTimerQueue())
        , m_h(0)
        , m_delegate(delegate)
        , m_ctx(ctx)
    {
        SK_ASSERT(m_queue);
    }

    bool valid() const noexcept
    {
        return (m_queue != 0);
    }

    void cancel() noexcept
    {
        if (m_h)
        {
            ::DeleteTimerQueueTimer(m_queue, m_h, INVALID_HANDLE_VALUE); // wait for the callback
            m_h = 0;
        }
    }

    bool singleShot(TimeSpan when) noexcept
    {
        SK_ASSERT(m_queue);

        cancel(); // just in case

        return !!::CreateTimerQueueTimer(
            &m_h,
            m_queue,
            _TimerCallback,
            this,
            static_cast<DWORD>(when.toMilliseconds()),
            0,
            WT_EXECUTEDEFAULT
        );
    }

    bool periodic(TimeSpan period) noexcept
    {
        SK_ASSERT(m_queue);

        cancel(); // just in case

        auto v = static_cast<DWORD>(period.toMilliseconds());

        return !!::CreateTimerQueueTimer(
            &m_h,
            m_queue,
            _TimerCallback,
            this,
            v,
            v,
            WT_EXECUTEDEFAULT
        );
    }

private:
    static void CALLBACK _TimerCallback(
        PVOID lpParameter,
        BOOLEAN TimerOrWaitFired
    )
    {
        SK_UNUSED(TimerOrWaitFired);

        auto _this = static_cast<TimerImpl*>(lpParameter);
        SK_ASSERT(_this);
        _this->_run();
    }

    void _run()
    {
        if (m_delegate)
        {
            m_delegate->run(m_ctx);
        }
    }

private:
    HANDLE m_queue;
    HANDLE m_h;
    ISimpleDelegate* m_delegate;
    void* m_ctx;
};


} // namespace internal {}

} // namespace sketch {}
