#include <SkBase/Date.hxx>
#include <SkBase/Time.hxx>


namespace sketch
{

Time& Time::operator=(const Date& other) noexcept
{
    *this = Time(other);

    return *this;
}


} // namespace sketch {}
