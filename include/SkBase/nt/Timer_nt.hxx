#pragma once


namespace sketch
{

namespace internal
{


class TimerImpl
    : public NonCopyable
{
public:
    ~TimerImpl() noexcept
    {
        cancel();
    }

    explicit TimerImpl(ISimpleDelegate* delegate, void* ctx) noexcept
        : m_delegate(delegate)
        , m_ctx(ctx)
    {
        KeInitializeTimer(&m_timer);
        KeInitializeDpc(&m_dpc, _DpcProc, this);
    }

    bool valid() const noexcept
    {
        return true;
    }

    void cancel() noexcept
    {
        KeCancelTimer(&m_timer);
    }

    bool singleShot(TimeSpan when) noexcept
    {
        LARGE_INTEGER li;
        li.QuadPart = -static_cast<LONGLONG>(when.to100nsecIntervals());
        KeSetTimer(&m_timer, li, &m_dpc);

        return true;
    }

    bool periodic(TimeSpan period) noexcept
    {
        LARGE_INTEGER start;
        start.QuadPart = -static_cast<LONGLONG>(period.to100nsecIntervals());
        KeSetTimerEx(&m_timer, start, static_cast<LONG>(period.toMilliseconds()), &m_dpc);

        return true;
    }

private:
    static void __stdcall _DpcProc(struct _KDPC*, void* context, void* argument1, void* argument2)
    {
        SK_UNUSED(argument1);
        SK_UNUSED(argument2);

        auto _this = static_cast<TimerImpl*>(context);
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
    KTIMER m_timer;
    ISimpleDelegate* m_delegate;
    void* m_ctx;
    KDPC m_dpc;
};


} // namespace internal {}

} // namespace sketch {}
