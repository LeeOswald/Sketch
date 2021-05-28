#pragma once

#include <SkBase/Thread.hxx>

#include "Protocol.h"


namespace tracer
{

class UdpServer
    : public sketch::NonCopyable
    , private sketch::ISimpleDelegate
{
public:
    typedef sketch::UniquePtr<UdpServer> Ref;

    ~UdpServer();
    UdpServer(uint16_t port, IMessageCallback* mc);

private:
    void sketch::ISimpleDelegate::run(void* ctx) override;
    void _worker();

private:
    IMessageCallback* m_callback;
    SOCKET m_socket{ INVALID_SOCKET };
    bool m_stopFlag{ false };
    sketch::Thread m_worker;
    sketch::UniquePtr<uint8_t[]> m_buffer;
    size_t m_bufferSize;
};


} // namespace tracer {}
