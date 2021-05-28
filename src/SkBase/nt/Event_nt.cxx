#include <SkBase/Event.hxx>

namespace sketch
{

namespace internal
{


class EventImpl
    : public NonCopyable
{
public:
    inline ~EventImpl() noexcept
    {
    }

    inline EventImpl(Event::Type type) noexcept
    {
        KeInitializeEvent(&m_event, type == Event::manualReset ? NotificationEvent : SynchronizationEvent, FALSE);
    }

    inline bool valid() const noexcept
    {
        return true;
    }

    inline void set() noexcept
    {
        KeSetEvent(&m_event, 0, FALSE);
    }

    inline void clear() noexcept
    {
        KeClearEvent(&m_event);
    }

    inline bool wait(uint32_t msec = Event::infinite) noexcept
    {
        LARGE_INTEGER to;
        if (msec != Event::infinite)
        {
            to.QuadPart = (-1) * msec * 10000;
        }

        auto status = KeWaitForSingleObject(
            &m_event,
            Executive,
            KernelMode,
            FALSE,
            (msec == Event::infinite) ? NULL : &to
        );

        return (status == STATUS_SUCCESS);
    }

    inline Event::WaitResult waitAlertable(uint32_t msec = Event::infinite) noexcept
    {
        LARGE_INTEGER to;
        if (msec != Event::infinite)
        {
            to.QuadPart = (-1) * msec * 10000;
        }

        auto status = KeWaitForSingleObject(
            &m_event,
            Executive,
            KernelMode,
            TRUE,
            (msec == Event::infinite) ? NULL : &to
        );
        SK_ASSERT(NT_SUCCESS(status));

        if (status == STATUS_SUCCESS)
        {
            return Event::success;
        }
        else if (status == STATUS_ALERTED)
        {
            return Event::alerted;
        }

        return Event::timeout;
    }

private:
    KEVENT m_event;
};


} // namespace internal {}


Event::~Event() noexcept
{
}

Event::Event() noexcept
{
}

Event::Event(Type type) noexcept
    : m_impl(new (sketch::nothrow) internal::EventImpl(type))
{
}

Event::Event(Event&& other) noexcept
    : m_impl(sketch::move(other.m_impl))
{
}

Event& Event::operator=(Event&& other) noexcept
{
    m_impl = sketch::move(other.m_impl);
    return *this;
}

bool Event::valid() const noexcept
{
    return m_impl ? m_impl->valid() : false;
}

void Event::set() noexcept
{
    SK_ASSERT(valid());
    m_impl->set();
}

void Event::clear() noexcept
{
    SK_ASSERT(valid());
    m_impl->clear();
}

bool Event::wait(uint32_t msec) noexcept
{
    SK_ASSERT(valid());
    return m_impl->wait(msec);
}

Event::WaitResult Event::waitAlertable(uint32_t msec) noexcept
{
    SK_ASSERT(valid());
    return m_impl->waitAlertable(msec);
}

} // namespace sketch {}

