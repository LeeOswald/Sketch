#include <SkBase/Date.hxx>
#include <SkBase/Time.hxx>


namespace sketch
{

Time::Time(const SYSTEMTIME& st) noexcept
{
    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);

    uint64_t value = ft.dwHighDateTime;
    value <<= 32;
    value |= ft.dwLowDateTime;

    m_value = value;
}

Time::Time(const Date& other) noexcept
{
    SYSTEMTIME st;
    st.wYear = WORD(other.year());
    st.wMonth = WORD(other.month());
    st.wDay = WORD(other.day());
    st.wDayOfWeek = WORD(other.dayOfWeek());
    st.wHour = WORD(other.hour());
    st.wMinute = WORD(other.minute());
    st.wSecond = WORD(other.second());
    st.wMilliseconds = WORD(other.millisecond());

    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);

    uint64_t value = ft.dwHighDateTime;
    value <<= 32;
    value |= ft.dwLowDateTime;

    m_value = value;
}

Time Time::utcNow() noexcept
{
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    return Time(ft);
}

Time Time::now() noexcept
{
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);

    return Time(ft);
}

Time& Time::operator=(const SYSTEMTIME& st) noexcept
{
    FILETIME ft;
    ::SystemTimeToFileTime(&st, &ft);

    uint64_t value = ft.dwHighDateTime;
    value <<= 32;
    value |= ft.dwLowDateTime;

    m_value = value;

    return *this;
}

SYSTEMTIME Time::toSystemTime() const noexcept
{
    auto ft = toFileTime();
    SYSTEMTIME st;
    ::FileTimeToSystemTime(&ft, &st);
    return st;
}

Date Time::unpack() const noexcept
{
    FILETIME ft;
    ft.dwLowDateTime = m_value & 0xffffffff;
    ft.dwHighDateTime = (uint32_t)((m_value & uint64_t(0xffffffff00000000)) >> 32);

    SYSTEMTIME st;
    ::FileTimeToSystemTime(&ft, &st);

    Date dt(st.wYear, Date::Month(st.wMonth), st.wDay, Date::DayOfWeek(st.wDayOfWeek), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return dt;
}


} // namespace sketch {}
