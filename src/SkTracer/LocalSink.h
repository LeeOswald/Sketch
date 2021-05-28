#pragma once

#include <Sketch.hxx>

#include "Protocol.h"

namespace tracer
{

class LocalSink
    : public sketch::RefCountedBase<sketch::log::ITraceSink>
{
public:
    static Ref make(IMessageCallback* mc) noexcept;

    void sketch::log::ITraceSink::write(sketch::log::Record::Ref r) noexcept override;

private:
    ~LocalSink() noexcept;
    LocalSink(IMessageCallback* mc) noexcept;

private:
    IMessageCallback* m_callback;
};

} // namespace tracer {}
