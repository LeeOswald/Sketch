#pragma once

#include <SkBase/Event.hxx>
#include <SkBase/Mutex.hxx>
#include <SkBase/Thread.hxx>

#include "Protocol.h"

#include <list>


namespace tracer
{

class TcpServer;

class TcpClient
    : public sketch::NonCopyable
    , private sketch::ISimpleDelegate
{
public:
    typedef sketch::UniquePtr<TcpClient> Ref;

    ~TcpClient() noexcept;
    TcpClient(SOCKET s, TcpServer* owner, IMessageCallback* mc);

    bool dead() const noexcept
    {
        return m_dead;
    }

private:
    void sketch::ISimpleDelegate::run(void* ctx) override;

private:
    SOCKET m_soket;
    TcpServer* m_owner;
    IMessageCallback* m_callback;
    sketch::Thread m_worker;
    bool m_stopFlag{ false };
    bool m_dead{ false };
};


class TcpServer
    : public sketch::NonCopyable
    , private sketch::ISimpleDelegate
{
public:
    typedef sketch::UniquePtr<TcpServer> Ref;

    ~TcpServer();
    TcpServer(uint16_t port, IMessageCallback* mc);

    void onDisconnect();

private:
    void sketch::ISimpleDelegate::run(void* ctx) override;
    void _worker();
    void _watchdog();

private:
    enum
    {
        WorkerThread = 1,
        WathdogThread = 2
    };

    IMessageCallback* m_callback;
    SOCKET m_soket{ INVALID_SOCKET };
    bool m_stopFlag{ false };
    sketch::Thread m_worker;
    sketch::Thread m_wathdog;
    sketch::Mutex m_lock;
    std::list<TcpClient::Ref> m_clients;
    sketch::Event m_disconnectEvent;
};

} // namespace tracer {}
