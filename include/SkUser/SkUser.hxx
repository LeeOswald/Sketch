#pragma once

#include <Sketch.hxx>


#ifndef SKETCH_UM
#error Only user mode configurations supported
#endif



namespace sketch
{

namespace log
{


#ifdef SKETCH_WIN

class TraceSink2
    : public sketch::RefCountedBase<sketch::log::ITraceSink>
{
public:
    static Ref make() noexcept
    {
        return Ref(new (sketch::nothrow) TraceSink2());
    }

    void write(sketch::log::Record::Ref r) noexcept override
    {
        if (r)
        {
            auto s = r->text();
            if (s.append("\n", 1))
            {
                ::OutputDebugStringA(s.str());
            }
        }
    }

private:
    ~TraceSink2() noexcept
    {
    }

    TraceSink2() noexcept
    {
    }
};

#endif // SKETCH_WIN


} // namespace log {}

} // namespace sketch {}
