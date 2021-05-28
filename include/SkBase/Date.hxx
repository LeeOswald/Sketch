#pragma once

#include "../Sketch.hxx"


namespace sketch
{

class Time;


class Date
{
public:
    enum Month
    {
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12,
    };

    enum DayOfWeek
    {
        Sunday = 0,
        Monday = 1,
        Tuesday = 2,
        Wednesday = 3,
        Thursday = 4,
        Friday = 5,
        Saturday = 6,
    };


public:
    inline ~Date(void) noexcept
    {
    }

    inline Date(void) noexcept
        : m_Year(0)
        , m_Month(January)
        , m_Day(0)
        , m_DayOfWeek(Sunday)
        , m_Hours(0)
        , m_Minutes(0)
        , m_Seconds(0)
        , m_Milliseconds(0)
    {
    }

    inline Date(const Date& other) noexcept
        : m_Year(other.m_Year)
        , m_Month(other.m_Month)
        , m_Day(other.m_Day)
        , m_DayOfWeek(other.m_DayOfWeek)
        , m_Hours(other.m_Hours)
        , m_Minutes(other.m_Minutes)
        , m_Seconds(other.m_Seconds)
        , m_Milliseconds(other.m_Milliseconds)
    {
    }

#ifdef SKETCH_WIN_UM

    explicit Date(const FILETIME& ft) noexcept;

    inline explicit Date(const SYSTEMTIME& st) noexcept
        : m_Year(st.wYear)
        , m_Month(Month(st.wMonth))
        , m_Day(st.wDay)
        , m_DayOfWeek(DayOfWeek(st.wDayOfWeek))
        , m_Hours(st.wHour)
        , m_Minutes(st.wMinute)
        , m_Seconds(st.wSecond)
        , m_Milliseconds(st.wMilliseconds)
    {
    }

#endif // SKETCH_WIN_UM

    inline Date(int year, Month month, int day, DayOfWeek dayOfWeek, int hour, int minute, int second, int millisecond) noexcept
        : m_Year(year)
        , m_Month(month)
        , m_Day(day)
        , m_DayOfWeek(dayOfWeek)
        , m_Hours(hour)
        , m_Minutes(minute)
        , m_Seconds(second)
        , m_Milliseconds(millisecond)
    {
        SK_ASSERT((month <= December) && (month >= January));
        SK_ASSERT(day <= daysInMonth(year, month));
        SK_ASSERT((dayOfWeek <= Saturday) && (dayOfWeek >= Sunday));
        SK_ASSERT(hour <= 23);
        SK_ASSERT(minute <= 59);
        SK_ASSERT(second <= 59);
        SK_ASSERT(millisecond <= 999);
    }

    inline Date(int year, Month month, int day, DayOfWeek dayOfWeek) noexcept
        : m_Year(year)
        , m_Month(month)
        , m_Day(day)
        , m_DayOfWeek(dayOfWeek)
        , m_Hours(0)
        , m_Minutes(0)
        , m_Seconds(0)
        , m_Milliseconds(0)
    {
        SK_ASSERT(month <= December);
        SK_ASSERT(day <= daysInMonth(year, month));
        SK_ASSERT(dayOfWeek <= Saturday);
    }

    inline Date(int hour, int minute, int second, int millisecond) noexcept
        : m_Year(0)
        , m_Month(January)
        , m_Day(0)
        , m_DayOfWeek(Sunday)
        , m_Hours(hour)
        , m_Minutes(minute)
        , m_Seconds(second)
        , m_Milliseconds(millisecond)
    {
        SK_ASSERT(hour <= 23);
        SK_ASSERT(minute <= 59);
        SK_ASSERT(second <= 59);
        SK_ASSERT(millisecond <= 999);
    }

    inline Date& operator=(const Date& other) noexcept
    {
        m_Year = other.m_Year;
        m_Month = other.m_Month;
        m_Day = other.m_Day;
        m_DayOfWeek = other.m_DayOfWeek;
        m_Hours = other.m_Hours;
        m_Minutes = other.m_Minutes;
        m_Seconds = other.m_Seconds;
        m_Milliseconds = other.m_Milliseconds;

        return *this;
    }

#ifdef SKETCH_WIN_UM

    inline Date& operator=(const FILETIME& ft) noexcept
    {
        *this = Date(ft);
        return *this;
    }

    inline Date& operator=(const SYSTEMTIME& st) noexcept
    {
        m_Year = st.wYear;
        m_Month = Month(st.wMonth);
        m_Day = st.wDay;
        m_DayOfWeek = DayOfWeek(st.wDayOfWeek);
        m_Hours = st.wHour;
        m_Minutes = st.wMinute;
        m_Seconds = st.wSecond;
        m_Milliseconds = st.wMilliseconds;

        return *this;
    }

#endif // SKETCH_WIN_UM

    inline int year(void) const noexcept
    {
        return m_Year;
    }

    inline Month month(void) const noexcept
    {
        return m_Month;
    }

    inline int day(void) const noexcept
    {
        return m_Day;
    }

    inline DayOfWeek dayOfWeek(void) const noexcept
    {
        return m_DayOfWeek;
    }

    inline int hour(void) const noexcept
    {
        return m_Hours;
    }

    inline int minute(void) const noexcept
    {
        return m_Minutes;
    }

    inline int second(void) const noexcept
    {
        return m_Seconds;
    }

    inline int millisecond(void) const noexcept
    {
        return m_Milliseconds;
    }

#ifdef SKETCH_WIN_UM

    FILETIME toFileTime() const noexcept;

    inline SYSTEMTIME toSystemTime() const noexcept
    {
        SYSTEMTIME st;
        st.wYear = WORD(m_Year);
        st.wMonth = WORD(m_Month);
        st.wDay = WORD(m_Day);
        st.wDayOfWeek = WORD(m_DayOfWeek);
        st.wHour = WORD(m_Hours);
        st.wMinute = WORD(m_Minutes);
        st.wSecond = WORD(m_Seconds);
        st.wMilliseconds = WORD(m_Milliseconds);
        return st;
    }

#endif // SKETCH_WIN_UM

    Time pack() const noexcept;

    static Date now(void) noexcept;
    static Date utcNow(void) noexcept;

    inline static bool isLeapYear(int year) noexcept
    {
        if ((year % 400) == 0) return true;
        if ((year % 100) == 0) return false;
        if ((year % 4) == 0) return true;
        return false;
    }

    inline static int daysInMonth(int year, Month month) noexcept
    {
        switch (month)
        {
        case January: return 31;
        case February: return (isLeapYear(year) ? 29 : 28);
        case March: return 31;
        case April: return 30;
        case May: return 31;
        case June: return 30;
        case July: return 31;
        case August: return 31;
        case September: return 30;
        case October: return 31;
        case November: return 30;
        case December: return 31;
        default:
            SK_ASSERT(0);
            return 0;
        }
    }

private:
    int m_Year;
    Month m_Month;
    int m_Day;
    DayOfWeek m_DayOfWeek;
    int m_Hours;
    int m_Minutes;
    int m_Seconds;
    int m_Milliseconds;
};


inline bool operator==(const Date& a, const Date& b) noexcept
{
    if (a.year() != b.year()) return false;
    if (a.month() != b.month()) return false;
    if (a.day() != b.day()) return false;
    if (a.hour() != b.hour()) return false;
    if (a.minute() != b.minute()) return false;
    if (a.second() != b.second()) return false;
    if (a.millisecond() != b.millisecond()) return false;

    return true;
}

inline bool operator!=(const Date& a, const Date& b) noexcept
{
    return !(a == b);
}

inline bool operator<(const Date& a, const Date& b) noexcept
{
    if (a.year() < b.year()) return true;
    else if (a.year() == b.year())
    {
        if (a.month() < b.month()) return true;
        else if (a.month() == b.month())
        {
            if (a.day() < b.day()) return true;
            else if (a.day() == b.day())
            {
                if (a.hour() < b.hour()) return true;
                else if (a.hour() == b.hour())
                {
                    if (a.minute() < b.minute()) return true;
                    else if (a.minute() == b.minute())
                    {
                        if (a.second() < b.second()) return true;
                        else if (a.minute() == b.minute())
                        {
                            if (a.millisecond() < b.millisecond()) return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

inline bool operator>(const Date& a, const Date& b) noexcept
{
    if (!(a < b)) return false;
    return (a != b);
}

inline bool operator>=(const Date& a, const Date& b) noexcept
{
    if (a < b) return false;
    return true;
}

inline bool operator<=(const Date& a, const Date& b) noexcept
{
    if (a < b) return true;
    return (a == b);
}


} // namespace sketch {}
