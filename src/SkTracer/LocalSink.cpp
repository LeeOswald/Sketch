#include "stdafx.h"
#include "LocalSink.h"


namespace tracer
{


LocalSink::Ref LocalSink::make(IMessageCallback* mc) noexcept
{
    return LocalSink::Ref(new LocalSink(mc));
}

void LocalSink::write(sketch::log::Record::Ref r) noexcept
{
    protocol::Record::Ref translated(new protocol::Record);

    translated->level = static_cast<protocol::Level>(r->level());
    translated->module = CStringW(r->module());
    translated->file = CStringW(r->file());
    translated->line = r->line();

    {
        auto date = r->time().unpack();
        auto f = sketch::AnsiString::format(
            "%02d:%02d:%02d.%04d",
            date.hour(),
            date.minute(),
            date.second(),
            date.millisecond()
        );

        translated->time = CStringW(f.str());
    }

    translated->pid = r->pid();
    translated->tid = r->tid();
    translated->text = CStringW(r->text().str());

    m_callback->message(std::move(translated));
}

LocalSink::~LocalSink() noexcept
{
}

LocalSink::LocalSink(IMessageCallback* mc) noexcept
    : m_callback(mc)
{
}


} // namespace tracer {}
