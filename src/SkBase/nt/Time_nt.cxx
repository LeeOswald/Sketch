#include <SkBase/Date.hxx>
#include <SkBase/Time.hxx>


namespace sketch
{

Time::Time(const Date& other) noexcept
{
    TIME_FIELDS st;
    st.Year = CSHORT(other.year());
    st.Month = CSHORT(other.month());
    st.Day = CSHORT(other.day());
    st.Weekday = CSHORT(other.dayOfWeek());
    st.Hour = CSHORT(other.hour());
    st.Minute = CSHORT(other.minute());
    st.Second = CSHORT(other.second());
    st.Milliseconds = CSHORT(other.millisecond());

    LARGE_INTEGER ft = { 0 };
    RtlTimeFieldsToTime(&st, &ft);

    m_value = ft.QuadPart;
}

Time Time::utcNow() noexcept
{
    LARGE_INTEGER li = { 0 };
    KeQuerySystemTime(&li);

    return Time(li.QuadPart);
}

Time Time::now() noexcept
{
    LARGE_INTEGER st = { 0 };
    KeQuerySystemTime(&st);
    LARGE_INTEGER lo = { 0 };
    ExSystemTimeToLocalTime(&st, &lo);

    return Time(lo.QuadPart);
}

Date Time::unpack() const noexcept
{
    LARGE_INTEGER li;
    li.QuadPart = m_value;
    TIME_FIELDS tf = { 0 };
    RtlTimeToTimeFields(&li, &tf);

    Date dt(tf.Year, Date::Month(tf.Month), tf.Day, Date::DayOfWeek(tf.Weekday), tf.Hour, tf.Minute, tf.Second, tf.Milliseconds);
    return dt;
}

} // namespace sketch  {}
