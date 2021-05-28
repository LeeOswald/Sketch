#pragma once


#include "../Sketch.hxx"


namespace sketch
{


class TimeSpan
{
public:
    inline ~TimeSpan() noexcept
    {
    }

    inline explicit TimeSpan(int64_t intervals = 0) noexcept
        : m_span(intervals)
    {
    }

    inline TimeSpan(unsigned hours, unsigned minutes, unsigned seconds, unsigned milliseconds)  noexcept
        : m_span(((((((hours * 60LL) + minutes) * 60LL) + seconds) * 1000LL) + milliseconds) * 10000LL)
    {
    }

    inline TimeSpan::TimeSpan(const TimeSpan& other) noexcept
        : m_span(other.m_span)
    {
    }

    inline TimeSpan& operator=(const TimeSpan& other) noexcept
    {
        m_span = other.m_span;
        return *this;
    }

    inline int64_t toDays() const noexcept
    {
        return m_span / (10000000LL * 3600 * 24);
    }

    inline int64_t toHours() const noexcept
    {
        return m_span / (10000000LL * 3600);
    }

    inline int64_t toMinutes() const noexcept
    {
        return m_span / (10000000LL * 60);
    }

    inline int64_t toSeconds() const noexcept
    {
        return m_span / 10000000LL;
    }

    inline int64_t toMilliseconds() const noexcept
    {
        return m_span / 10000LL;
    }

    inline int64_t to100nsecIntervals() const noexcept
    {
        return m_span;
    }

    inline TimeSpan operator+(const TimeSpan& ts) const noexcept
    {
        return TimeSpan(m_span + ts.m_span);
    }

    inline TimeSpan operator-(const TimeSpan& ts) const noexcept
    {
        return TimeSpan(m_span - ts.m_span);
    }

    inline TimeSpan operator*(unsigned Count) const noexcept
    {
        return TimeSpan(m_span * Count);
    }

    inline TimeSpan operator/(unsigned Count) const noexcept
    {
        return TimeSpan(m_span / Count);
    }

    inline TimeSpan& operator+=(const TimeSpan& ts) noexcept
    {
        m_span += ts.m_span;
        return *this;
    }

    inline TimeSpan& operator-=(const TimeSpan& ts) noexcept
    {
        m_span -= ts.m_span;
        return *this;
    }

    inline bool operator==(const TimeSpan& other) const noexcept
    {
        return (m_span == other.m_span);
    }

    inline bool operator!=(const TimeSpan& other) const noexcept
    {
        return (m_span != other.m_span);
    }

    inline bool operator<(const TimeSpan& other) const noexcept
    {
        return (m_span < other.m_span);
    }

    inline bool operator<=(const TimeSpan& other) const noexcept
    {
        return (m_span <= other.m_span);
    }

    inline bool operator>(const TimeSpan& other) const noexcept
    {
        return (m_span > other.m_span);
    }

    inline bool operator>=(const TimeSpan& other) const noexcept
    {
        return (m_span >= other.m_span);
    }

protected:
    int64_t m_span; // 100-nsec intervals
};


} // namespace sketch {}
