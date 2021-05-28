#pragma once

#include "../Sketch.hxx"


namespace sketch
{

class Date;


class Time
{
public:
    inline ~Time() noexcept
    {
    }

    inline explicit Time(uint64_t val = 0) noexcept
        : m_value(val)
    {
    }

    inline Time(const Time& other) noexcept
        : m_value(other.m_value)
    {
    }

    explicit Time(const Date& other) noexcept;

    inline Time& operator=(const Time& other) noexcept
    {
        m_value = other.m_value;
        return *this;
    }

    Time& operator=(const Date& other) noexcept;

#ifdef SKETCH_WIN_UM

    inline explicit Time(const FILETIME& ft) noexcept
        : m_value(uint64_t(ft.dwLowDateTime) | (uint64_t(ft.dwHighDateTime) << 32))
    {
    }

    explicit Time(const SYSTEMTIME& st) noexcept;

    inline Time& operator=(const FILETIME& ft) noexcept
    {
        m_value = uint64_t(ft.dwLowDateTime) | (uint64_t(ft.dwHighDateTime) << 32);
        return *this;
    }

    Time& operator=(const SYSTEMTIME& st) noexcept;

#endif // SKETCH_WIN_UM

    static Time now() noexcept;
    static Time utcNow() noexcept;

    Date unpack() const noexcept;

#ifdef SKETCH_WIN_UM

    inline FILETIME toFileTime() const noexcept
    {
        FILETIME ft;
        ft.dwLowDateTime = uint32_t(m_value);
        ft.dwHighDateTime = uint32_t(m_value >> 32);
        return ft;
    }

    SYSTEMTIME toSystemTime() const noexcept;

#endif // SKETCH_WIN_UM

    inline int64_t value() const noexcept
    {
        return m_value;
    }

    inline int toMilliseconds() const noexcept
    {
        return int(m_value / 10000);
    }

    inline void addMilliseconds(int64_t nMilliseconds) noexcept
    {
        m_value += (nMilliseconds * 10000);
    }

private:
    uint64_t m_value; //number of 100-nanosecond intervals since January 1, 1601 (UTC).
};


inline bool operator==(const Time& a, const Time& b) noexcept
{
    return (a.value() == b.value());
}

inline bool operator!=(const Time& a, const Time& b) noexcept
{
    return (a.value() != b.value());
}

inline bool operator>(const Time& a, const Time& b) noexcept
{
    return (a.value() > b.value());
}

inline bool operator<(const Time& a, const Time& b) noexcept
{
    return (a.value() < b.value());
}

inline bool operator>=(const Time& a, const Time& b) noexcept
{
    return (a.value() >= b.value());
}

inline bool operator<=(const Time& a, const Time& b) noexcept
{
    return (a.value() <= b.value());
}



} // namespace sketch {}
