#include <SkBase/Event.hxx>
#include <SkBase/IntrusiveList.hxx>
#include <SkBase/Mutex.hxx>
#include <SkBase/Queue.hxx>
#include <SkBase/Thread.hxx>
#include <SkBase/UniquePtr.hxx>

#include "Trace_impl.hxx"


namespace sketch
{

namespace log
{


DefaultTraceFormatter::~DefaultTraceFormatter() noexcept
{

}

DefaultTraceFormatter::DefaultTraceFormatter() noexcept
{

}

ITraceFormatter::Ref DefaultTraceFormatter::make() noexcept
{
    return new (sketch::nothrow) DefaultTraceFormatter();
}



namespace
{

struct TraceRegistration
{
    TraceRegistration(String _name, log::ITraceSink::Ref _sink, log::ITraceFormatter::Ref _formatter, ITraceFilter::Ref _filter)
        : name(_name)
        , sink(_sink)
        , formatter(_formatter)
        , filter(_filter)
    {
    }

    ListNode<TraceRegistration> node;
    String name;
    log::ITraceSink::Ref sink;
    log::ITraceFormatter::Ref formatter;
    log::ITraceFilter::Ref filter;
};


typedef IntrusiveList<TraceRegistration, &TraceRegistration::node> TraceRegistrationList;

struct TraceSinkWriter;

struct TraceGlobalData
{
    ~TraceGlobalData()
    {
    }

    TraceGlobalData()
        : lock()
        , sinkList()
        , stopFlag(false)
        , queue(4096)
        , recordAvailableEvent(Event::autoReset)
    {
    }

    Mutex lock;
    TraceRegistrationList sinkList;

    bool stopFlag;
    Queue<Record> queue;
    Event recordAvailableEvent;
    UniquePtr<TraceSinkWriter> sinkWriter;
    UniquePtr<Thread> writerThread;
};

static sketch::refcount::type g_RefCount = 0;
static uint8_t _TraceGlobalDataPlace[sizeof(TraceGlobalData)];
static TraceGlobalData* g_TraceGlobalData = nullptr;

static void pumpEvents()
{
    auto record = g_TraceGlobalData->queue.pop();
    while (record)
    {
        Record::Ref ref;
        ref.attach(record);

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            for (auto& r : g_TraceGlobalData->sinkList)
            {
                if (r.filter->filter(ref))
                {
                    r.sink->write(r.formatter->format(ref));
                }
            }
        }

        record = g_TraceGlobalData->queue.pop();
    }
}

struct TraceSinkWriter
    : public ISimpleDelegate
{
    ~TraceSinkWriter()
    {
    }

    TraceSinkWriter()
    {
    }

    void ISimpleDelegate::run(void* ctx) override
    {
        SK_UNUSED(ctx);

        while (!g_TraceGlobalData->stopFlag)
        {
            if (g_TraceGlobalData->queue.empty())
            {
                auto result = g_TraceGlobalData->recordAvailableEvent.waitAlertable(); // underlying async IO relies on this alertability
                if (result != Event::success)
                {
                    continue;
                }
            }

            while (!g_TraceGlobalData->queue.empty())
            {
                pumpEvents();
            }
        }
    }
};

} // namespace {}



#ifdef SKETCH_DBG
Level g_Level = Debug;
#else
Level g_Level = Info;
#endif


const char* g_ModuleName = "Sketch";


bool initialize(const char* moduleName) noexcept
{
    if (refcount::increment(&g_RefCount) > 1)
    {
        return true;
    }

    if (moduleName)
    {
        g_ModuleName = moduleName;
    }

    SK_ASSERT(!g_TraceGlobalData);

    g_TraceGlobalData = new (_TraceGlobalDataPlace) TraceGlobalData;

    g_TraceGlobalData->sinkWriter.reset(new (sketch::nothrow) TraceSinkWriter());
    if (!g_TraceGlobalData->sinkWriter)
    {
        finalize();
        return false;
    }

    g_TraceGlobalData->writerThread.reset(new (sketch::nothrow) Thread(g_TraceGlobalData->sinkWriter.get(), "Trace_dispatcher"));
    if (!g_TraceGlobalData->writerThread)
    {
        finalize();
        return false;
    }

    return true;
}

void finalize() noexcept
{
    flush();

    if (refcount::decrement(&g_RefCount) != 0)
    {
        return;
    }

    if (g_TraceGlobalData)
    {
        g_TraceGlobalData->stopFlag = true;

        if (g_TraceGlobalData->writerThread)
        {
            g_TraceGlobalData->writerThread->alert();
            g_TraceGlobalData->writerThread->join();
            g_TraceGlobalData->writerThread.reset();
        }

        if (g_TraceGlobalData->sinkWriter)
        {
            g_TraceGlobalData->sinkWriter.reset();
        }

        auto record = g_TraceGlobalData->queue.pop();
        while (record)
        {
            record->release();
            record = g_TraceGlobalData->queue.pop();
        }

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            while (!g_TraceGlobalData->sinkList.empty())
            {
                auto reg = g_TraceGlobalData->sinkList.pull_back();
                delete reg;
            }
        }

        auto p = g_TraceGlobalData;
        g_TraceGlobalData = nullptr;
        p->~TraceGlobalData();
    }
}

bool registerSink(const char* name, ITraceSink::Ref sink, ITraceFormatter::Ref formatter, ITraceFilter::Ref filter) noexcept
{
    SK_ASSERT(name);
    if (!name)
    {
        return false;
    }

    SK_ASSERT(sink);
    if (!sink)
    {
        return false;
    }

    SK_ASSERT(formatter);
    if (!formatter)
    {
        return false;
    }

    SK_ASSERT(filter);
    if (!filter)
    {
        return false;
    }

    SK_ASSERT(g_TraceGlobalData);
    if (!g_TraceGlobalData)
    {
        return false;
    }

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        auto r = new (sketch::nothrow) TraceRegistration(name, sink, formatter, filter);
        if (!r)
        {
            return false;
        }

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            g_TraceGlobalData->sinkList.push_back(r); // noexcept, so this is safe to use raw pointer 'r'
        }

#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (Exception&)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }
#endif


    return true;
}

void unregisterSink(const char* name) noexcept
{
    SK_ASSERT(g_TraceGlobalData);
    if (!g_TraceGlobalData)
    {
        return;
    }

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        String sName(name);

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            for (auto& r : g_TraceGlobalData->sinkList)
            {
                if (r.name == sName)
                {
                    g_TraceGlobalData->sinkList.erase(&r);
                    delete (&r);
                    break;
                }
            }
        }
#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (Exception&)
    {
    }
    catch (...)
    {
    }
#endif
}

bool setFormatter(const char* sinkName, ITraceFormatter::Ref formatter) noexcept
{
    SK_ASSERT(formatter);
    if (!formatter)
    {
        return false;
    }

    SK_ASSERT(g_TraceGlobalData);
    if (!g_TraceGlobalData)
    {
        return false;
    }

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        String sName(sinkName);

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            for (auto& r : g_TraceGlobalData->sinkList)
            {
                if (r.name == sName)
                {
                    r.formatter = formatter;
                    return true;
                }
            }
        }
#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (Exception&)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }
#endif

    return false;
}

bool setFilter(const char* sinkName, ITraceFilter::Ref filter) noexcept
{
    SK_ASSERT(filter);
    if (!filter)
    {
        return false;
    }

    SK_ASSERT(g_TraceGlobalData);
    if (!g_TraceGlobalData)
    {
        return false;
    }

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif
        String sName(sinkName);

        {
            Mutex::Guard g(g_TraceGlobalData->lock);
            for (auto& r : g_TraceGlobalData->sinkList)
            {
                if (r.name == sName)
                {
                    r.filter = filter;
                    return true;
                }
            }
        }
#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (Exception&)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }
#endif

    return false;
}

void flush() noexcept
{
#ifdef SKETCH_WIN_KM
    if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
    {
        return;
    }
#endif

    SK_ASSERT(g_TraceGlobalData);
    if (!g_TraceGlobalData)
    {
        return;
    }

    if (g_TraceGlobalData->stopFlag)
    {
        return;
    }

    while (!g_TraceGlobalData->queue.empty())
    {
        thread::current::yield();
    }
}

bool writeInternalV(Level level, const char* module, const char* file, int line, const char* format, va_list args) noexcept
{
    if (!g_TraceGlobalData)
    {
        return bailOutWriteV(level, module, file, line, format, args);
    }

    if (g_TraceGlobalData->stopFlag)
    {
        return bailOutWriteV(level, module, file, line, format, args);
    }

    if (level < g_Level)
    {
        return false;
    }

    if (!format)
    {
        return false;
    }

    if (!module)
    {
        module = "Sketch";
    }

    if (g_TraceGlobalData->sinkList.empty())
    {
        return bailOutWriteV(level, module, file, line, format, args);
    }

#ifdef SKETCH_CXX_EXCEPTIONS
    try
    {
#endif

#ifdef SKETCH_WIN
        auto text = AnsiString::formatV(format, args);
#else
        auto text = Utf8String::formatV(format, args);
#endif

        auto record = Record::make(level, module, file, line, text);
        auto ptr = record.detach();
        if (!g_TraceGlobalData->queue.push(ptr))
        {
            // discard
            ptr->release();
            return false;
        }

        g_TraceGlobalData->recordAvailableEvent.set();

#ifdef SKETCH_CXX_EXCEPTIONS
    }
    catch (Exception&)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }
#endif

    return true;
}


} // namespace log {}

} // namespace sketch {}
