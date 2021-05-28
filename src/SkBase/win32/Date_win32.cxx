#include <SkBase/Date.hxx>
#include <SkBase/Time.hxx>


namespace sketch
{

Date::Date(const FILETIME& ft) noexcept
{
    SYSTEMTIME st;
    ::FileTimeToSystemTime(&ft, &st);

    m_Year = st.wYear;
    m_Month = Month(st.wMonth);
    m_Day = st.wDay;
    m_DayOfWeek = DayOfWeek(st.wDayOfWeek);
    m_Hours = st.wHour;
    m_Minutes = st.wMinute;
    m_Seconds = st.wSecond;
    m_Milliseconds = st.wMilliseconds;
}

FILETIME Date::toFileTime() const noexcept
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

    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);
    return ft;
}

Date Date::now() noexcept
{
    return Time::now().unpack();
}

Date Date::utcNow() noexcept
{
    return Time::utcNow().unpack();
}

Time Date::pack() const noexcept
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

    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);
    return Time(ft);
}

} // namespace sketch {}
