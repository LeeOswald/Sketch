#include <SkBase/Date.hxx>
#include <SkBase/Time.hxx>


namespace sketch
{


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
    TIME_FIELDS tf;
    tf.Year = CSHORT(m_Year);
    tf.Month = CSHORT(m_Month);
    tf.Day = CSHORT(m_Day);
    tf.Weekday = CSHORT(m_DayOfWeek);
    tf.Hour = CSHORT(m_Hours);
    tf.Minute = CSHORT(m_Minutes);
    tf.Second = CSHORT(m_Seconds);
    tf.Milliseconds = CSHORT(m_Milliseconds);

    LARGE_INTEGER li = { 0 };
    RtlTimeFieldsToTime(&tf, &li);

    return Time(li.QuadPart);
}


} // namespace sketch  {}
