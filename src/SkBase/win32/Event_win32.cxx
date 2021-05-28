#include <SkBase/Event.hxx>

namespace sketch
{

namespace internal
{


class EventImpl
    : private NonCopyable
{
public:
    inline ~EventImpl() noexcept
    {
        if (m_event)
        {
            ::CloseHandle(m_event);
        }
    }

    inline EventImpl(Event::Type type) noexcept
        : m_event(::CreateEventW(nullptr, (type == Event::manualReset), FALSE, nullptr))
    {
        SK_ASSERT(m_event);
    }

    inline bool valid() const noexcept
    {
        return !!m_event;
    }

    inline void set() noexcept
    {
        ::SetEvent(m_event);
    }

    inline void clear() noexcept
    {
        ::ResetEvent(m_event);
    }

    inline bool wait(uint32_t msec = Event::infinite) noexcept
    {
        auto result = ::WaitForSingleObjectEx(m_event, msec, FALSE);

        return (result == WAIT_OBJECT_0);
    }

    inline Event::WaitResult waitAlertable(uint32_t msec = Event::infinite) noexcept
    {
        auto result = ::WaitForSingleObjectEx(m_event, msec, TRUE);
        if (result == WAIT_OBJECT_0)
        {
            return Event::success;
        }
        else if (result == WAIT_IO_COMPLETION)
        {
            return Event::alerted;
        }

        SK_ASSERT(result == WAIT_TIMEOUT);
        return Event::timeout;
    }

private:
    HANDLE m_event;
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
