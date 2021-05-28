#pragma once

#include <SkBase/UniquePtr.hxx>

#include <atlbase.h>


// must match SkBase/Trace.NetSink.hxx

namespace protocol
{

// record header as sent over the wire
struct Header
{
    uint32_t size; // record size in bytes, w/out this header

    // uint8_t data[size];
};


enum Level
{
    Invalid = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Highest = 4,
    Max // should go last
};

struct Record
{
    bool selected;
    Level level;
    CStringW module;
    CStringW file;
    int line;
    CStringW time;
    int pid;
    int tid;
    int irql;
    CStringW text;

    typedef sketch::UniquePtr<Record> Ref;

    Record()
        : selected(false)
        , level(Invalid)
        , line(-1)
        , pid(-1)
        , tid(-1)
        , irql(-1)
    {
    }

    static Ref unpack(const char* jsonRaw);
};

} // namespace protocol {}

namespace tracer
{

struct IMessageCallback
{
    virtual void message(protocol::Record::Ref r) = 0;

protected:
    ~IMessageCallback() noexcept { }
};


} // namespace tracer {}
