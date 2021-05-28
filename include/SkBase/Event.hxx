#pragma once

#include "../Sketch.hxx"
#include "UniquePtr.hxx"


namespace sketch
{

namespace internal
{

class EventImpl;

} // namespace internal {}


class Event
    : public NonCopyable
{
public:
    enum Type
    {
        manualReset,
        autoReset
    };

    enum WaitResult
    {
        success,
        timeout,
        alerted
    };

    static const uint32_t infinite = (uint32_t)-1;

public:
    ~Event() noexcept;
    Event() noexcept;
    explicit Event(Type type) noexcept;
    Event(Event&& other) noexcept;
    Event& operator=(Event&& other) noexcept;

    bool valid() const noexcept;
    void set() noexcept;
    void clear() noexcept;

    bool wait(uint32_t msec = infinite) noexcept;
    WaitResult waitAlertable(uint32_t msec = infinite) noexcept;

private:
    UniquePtr<internal::EventImpl> m_impl;
};


} // namespace sketch {}
