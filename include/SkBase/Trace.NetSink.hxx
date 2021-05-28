#pragma once

#include "../Sketch.hxx"


namespace sketch
{

namespace log
{

ITraceSink::Ref createUdpSink(const wchar_t* address, uint16_t port);
ITraceSink::Ref createTcpSink(const wchar_t* address, uint16_t port);


} // namespace log {}

} // namespace sketch {}
