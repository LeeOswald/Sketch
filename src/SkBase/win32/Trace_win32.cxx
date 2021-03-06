#include <SkBase/Date.hxx>
#include <SkBase/Process.hxx>
#include <SkBase/Thread.hxx>

#include "../Trace_impl.hxx"

#include <strsafe.h>

namespace sketch
{

namespace log
{


Record::Ref DefaultTraceFormatter::format(Record::Ref r) noexcept
{
    Record::Ref result;

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        char l = '?';
        switch (r->level())
        {
        case Highest:
            l = '!';
            break;
        case Error:
            l = 'E';
            break;
        case Warning:
            l = 'W';
            break;
        case Info:
            l = 'I';
            break;
        case Debug:
            l = 'D';
            break;
        }

        auto date = r->time().unpack();

        auto f = AnsiString::format(
            "[%c %02d:%02d:%02d.%04d %s @%d:%d] %s",
            l,
            date.hour(),
            date.minute(),
            date.second(),
            date.millisecond(),
            r->module(),
            r->pid(),
            r->tid(),
            r->text().str()
        );

        result = Record::make(
            r->level(),
            r->module(),
            r->file(),
            r->line(),
            r->time(),
            r->pid(),
            r->tid(),
            f
        );

#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (sketch::Exception&)
    {
    }
#endif

    return result;
}


namespace
{

#define DbgPrintExBufferSize 512

static void _WriteToDebugger(const AnsiString& s) noexcept
{
    if (s.length() < DbgPrintExBufferSize)
    {
        DbgPrintEx(77, 0, "%s", s.str());
    }
    else
    {
        char buffer[DbgPrintExBufferSize];
        auto text = s.str();
        while (*text)
        {
            size_t n = 0;
            while (*text && (n < DbgPrintExBufferSize - 1))
            {
                buffer[n++] = *text++;
            }

            if (!n)
            {
                break;
            }

            buffer[n] = '\0';

            DbgPrintEx(77, 0, "%s", buffer);
        }
    }
}

} // namespace {}


TraceSink0::~TraceSink0() noexcept
{

}

TraceSink0::TraceSink0() noexcept
{

}

TraceSink0::Ref TraceSink0::make() noexcept
{
    return TraceSink0::Ref(new (sketch::nothrow) TraceSink0());
}

void TraceSink0::write(Record::Ref r) noexcept
{
#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        if (r)
        {
            auto s = r->text();
            if (s.append("\n", 1))
            {
                _WriteToDebugger(s);
            }
        }
#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (sketch::Exception&)
    {
    }
#endif
}


bool bailOutWriteV(Level level, const char* module, const char* file, int line, const char* format, va_list args) noexcept
{
    SK_UNUSED(file);
    SK_UNUSED(line);

    if (level < g_Level)
    {
        return false;
    }

    char l = '?';
    switch (level)
    {
    case Highest:
        l = '!';
        break;
    case Error:
        l = 'E';
        break;
    case Warning:
        l = 'W';
        break;
    case Info:
        l = 'I';
        break;
    case Debug:
        l = 'D';
        break;
    }

    char buf[512];
    StringCchVPrintfA(buf, _countof(buf), format, args);

    auto pid = process::current::id();
    auto tid = thread::current::id();

    DbgPrintEx(77, 0, "[%c %s %d:%d] %s\n", l, module, pid, tid, buf);

    return true;
}

} // namespace log {}

} // namespace sketch {}


