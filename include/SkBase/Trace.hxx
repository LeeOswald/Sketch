#pragma once

#include "../Sketch.hxx"

#include "IRefCounted.hxx"
#include "Process.hxx"
#include "Thread.hxx"


namespace sketch
{

namespace log
{


typedef enum
{
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Highest = 4,
    Off // should go last
} Level;

extern Level g_Level;
extern const char* g_ModuleName;


class Record
    : public RefCountedBase<IRefCounted>
{
public:
    typedef RefCountedPtr<Record> Ref;

    static inline Ref make(
        Level level,
        const char* module,
        const char* file,
        int line,
#ifdef SKETCH_WIN
        const AnsiString& text
#else
        const Utf8String& text
#endif
    )
    {
        return Ref(new(sketch::nothrow) Record(
            level,
            module,
            file,
            line,
            Time::now(),
            process::current::id(),
            thread::current::id(),
#ifdef SKETCH_WIN_KM
            KeGetCurrentIrql(),
#endif
            text
        ));
    }

    static inline Ref make(
        Level level,
        const char* module,
        const char* file,
        int line,
        Time time,
        uint32_t pid,
        uint32_t tid,
#ifdef SKETCH_WIN_KM
        KIRQL irql,
#endif
#ifdef SKETCH_WIN
        const AnsiString& text
#else
        const Utf8String& text
#endif
    )
    {
        return Ref(new(sketch::nothrow) Record(
            level,
            module,
            file,
            line,
            time,
            pid,
            tid,
#ifdef SKETCH_WIN_KM
            irql,
#endif
            text
        ));
    }

    inline Level level() const noexcept
    {
        return m_level;
    }

    inline const char* module() const noexcept
    {
        return m_module;
    }

    inline const char* file() const noexcept
    {
        return m_file;
    }

    inline int line() const noexcept
    {
        return m_line;
    }

    inline Time time() const noexcept
    {
        return m_time;
    }

    inline uint32_t pid() const noexcept
    {
        return m_pid;
    }

    inline uint32_t tid() const noexcept
    {
        return m_tid;
    }

#ifdef SKETCH_WIN_KM
    inline KIRQL irql() const noexcept
    {
        return m_irql;
    }
#endif

#ifdef SKETCH_WIN
    inline AnsiString text() const noexcept
#else
    inline Utf8String text() const noexcept
#endif
    {
        return m_text;
    }

protected:
    virtual ~Record() noexcept
    {
    }

    Record(
        Level level,
        const char* module,
        const char* file,
        int line,
        Time time,
        uint32_t pid,
        uint32_t tid,
#ifdef SKETCH_WIN_KM
        KIRQL irql,
#endif

#ifdef SKETCH_WIN
        const AnsiString& text
#else
        const Utf8String& text
#endif
        )
        : m_level(level)
        , m_module(module)
        , m_file(file)
        , m_line(line)
        , m_time(time)
        , m_pid(pid)
        , m_tid(tid)
#ifdef SKETCH_WIN_KM
        , m_irql(irql)
#endif
        , m_text(text)
    {
    }

private:
    Level m_level;
    const char* m_module;
    const char* m_file;
    int m_line;
    Time m_time;
    uint32_t m_pid;
    uint32_t m_tid;
#ifdef SKETCH_WIN_KM
    KIRQL m_irql;
#endif

#ifdef SKETCH_WIN
    AnsiString m_text;
#else
    Utf8String m_text;
#endif
};


struct __declspec(novtable) ITraceSink
    : public IRefCounted
{
    typedef RefCountedPtr<ITraceSink> Ref;

    virtual void write(Record::Ref r) noexcept = 0;
};


struct __declspec(novtable) ITraceFormatter
    : public IRefCounted
{
    typedef RefCountedPtr<ITraceFormatter> Ref;

    virtual Record::Ref format(Record::Ref r) noexcept = 0;
};


struct __declspec(novtable) ITraceFilter
    : public IRefCounted
{
    typedef RefCountedPtr<ITraceFilter> Ref;

    virtual bool filter(Record::Ref r) noexcept = 0;
};

class TraceSink0
    : public RefCountedBase<ITraceSink>
{
public:
    static Ref make() noexcept;

    void write(Record::Ref r) noexcept override;

private:
    ~TraceSink0() noexcept;
    TraceSink0() noexcept;
};


class DefaultTraceFormatter
    : public RefCountedBase<ITraceFormatter>
{
public:
    static ITraceFormatter::Ref make() noexcept;

    Record::Ref format(Record::Ref r) noexcept override;

private:
    ~DefaultTraceFormatter() noexcept;
    DefaultTraceFormatter() noexcept;
};


class NullTraceFormatter
    : public RefCountedBase<ITraceFormatter>
{
public:
    static ITraceFormatter::Ref make() noexcept
    {
        return ITraceFormatter::Ref(new (sketch::nothrow) NullTraceFormatter());
    }

    Record::Ref format(Record::Ref r) noexcept override
    {
        return r;
    }

private:
    ~NullTraceFormatter() noexcept
    {
    }

    NullTraceFormatter() noexcept
    {
    }
};


class DefaultTraceFilter
    : public RefCountedBase<ITraceFilter>
{
public:
    static ITraceFilter::Ref make(Level minLevel, Level maxLevel = Highest)
    {
        return ITraceFilter::Ref(new (sketch::nothrow) DefaultTraceFilter(minLevel, maxLevel));
    }

    bool filter(Record::Ref r) noexcept override
    {
        auto l = r->level();
        return ((l >= m_minLevel) && (l <= m_maxLevel));
    }

private:
    ~DefaultTraceFilter() noexcept
    {
    }

    DefaultTraceFilter(Level minLevel, Level maxLevel) noexcept
        : m_minLevel(minLevel)
        , m_maxLevel(maxLevel)
    {
    }

private:
    Level m_minLevel;
    Level m_maxLevel;
};


bool initialize(const char* moduleName = nullptr) noexcept;
void finalize() noexcept;

bool registerSink(const char* name, ITraceSink::Ref sink, ITraceFormatter::Ref formatter, ITraceFilter::Ref filter) noexcept;
void unregisterSink(const char* name) noexcept;
bool setFormatter(const char* sinkName, ITraceFormatter::Ref formatter) noexcept;
bool setFilter(const char* sinkName, ITraceFilter::Ref filter) noexcept;

bool writeInternalV(Level level, const char* module, const char* file, int line, const char* format, va_list args) noexcept;
void flush() noexcept;

inline bool writeInternal(Level level, const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(level, module, file, line, format, args);

    va_end(args);

    return true;
}

inline bool writeDebug(const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(Debug, module, file, line, format, args);

    va_end(args);

    return true;
}

inline bool writeInfo(const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(Info, module, file, line, format, args);

    va_end(args);

    return true;
}

inline bool writeWarning(const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(Warning, module, file, line, format, args);

    va_end(args);

    return true;
}

inline bool writeError(const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(Error, module, file, line, format, args);

    va_end(args);

    return true;
}

inline bool writeHighest(const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    writeInternalV(Highest, module, file, line, format, args);

    va_end(args);

    return true;
}


#define SketchDebug(format, ...) \
  (sketch::log::g_Level > sketch::log::Debug) || sketch::log::writeDebug(sketch::log::g_ModuleName, __FILE__, __LINE__, format, __VA_ARGS__)

#define SketchInfo(format, ...) \
  (sketch::log::g_Level > sketch::log::Info) || sketch::log::writeInfo(sketch::log::g_ModuleName, __FILE__, __LINE__, format, __VA_ARGS__)

#define SketchWarning(format, ...) \
  (sketch::log::g_Level > sketch::log::Warning) || sketch::log::writeWarning(sketch::log::g_ModuleName, __FILE__, __LINE__, format, __VA_ARGS__)

#define SketchError(format, ...) \
  (sketch::log::g_Level > sketch::log::Error) || sketch::log::writeError(sketch::log::g_ModuleName, __FILE__, __LINE__, format, __VA_ARGS__)

#define SketchHighest(format, ...) \
  (sketch::log::g_Level > sketch::log::Highest) || sketch::log::writeHighest(sketch::log::g_ModuleName, __FILE__, __LINE__, format, __VA_ARGS__)


#define SketchFlush() \
  sketch::log::flush()

} // namespace log {}

} // namespace sketch {}


