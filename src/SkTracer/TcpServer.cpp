#include "stdafx.h"
#include "TcpServer.h"

#include <SkBase/Vector.hxx>

namespace tracer
{

#pragma region TcpClient

TcpClient::~TcpClient() noexcept
{
    m_stopFlag = true;

    if (m_soket != INVALID_SOCKET)
    {
        ::closesocket(m_soket);
    }

    if (m_worker.valid())
    {
        m_worker.join();
    }
}

TcpClient::TcpClient(SOCKET s, TcpServer* owner, IMessageCallback* mc)
    : m_soket(s)
    , m_owner(owner)
    , m_callback(mc)
    , m_worker(sketch::Thread(this, nullptr, "TcpClient"))
{
}

void TcpClient::run(void* ctx)
{
    SK_UNUSED(ctx);

    while (!m_stopFlag)
    {
        protocol::Header pkt;
        {
            auto rd = ::recv(m_soket, reinterpret_cast<char*>(&pkt), sizeof(pkt), 0);

            if ((rd == SOCKET_ERROR) || (rd == 0))
            {
                break;
            }

            if (rd < int(sizeof(pkt)))
            {
                SketchError("Received only %d bytes of header, aborting", rd);
                break;
            }
        }

        sketch::Vector<uint8_t> w(pkt.size + 1);
        {
            auto rd = ::recv(m_soket, reinterpret_cast<char*>(&w[0]), pkt.size, 0);
            if ((rd == SOCKET_ERROR) || (rd == 0))
            {
                break;
            }

            if (rd < int(pkt.size))
            {
                SketchError("Received only %d bytes of data (%d promised), aborting", rd, pkt.size);
                break;
            }

            w[pkt.size] = 0; // force null terminator
        }

        try
        {
            auto r = std::move(protocol::Record::unpack(reinterpret_cast<const char*>(&w[0])));
            if (r->level != protocol::Invalid)
            {
                m_callback->message(std::move(r));
            }
        }
        catch (sketch::Exception& e)
        {
            auto inner = e.error();
            auto f = inner->format(u"Failed to parse a record: {MS} {FL}");

            auto l = sketch::toSystemLocale(f);
            SketchError(l.str());
        }
        catch (std::exception& e)
        {
            SketchError(e.what());
        }
    }

    ::shutdown(m_soket, SD_BOTH);

    m_dead = true;
    m_owner->onDisconnect();

    SketchInfo("Connection closed");
}

#pragma endregion

#pragma region TcpServer

TcpServer::~TcpServer()
{
    m_stopFlag = true;

    if (m_soket != INVALID_SOCKET)
    {
        ::closesocket(m_soket);
    }

    if (m_worker.valid())
    {
        m_worker.join();
    }

    if (m_wathdog.valid())
    {
        m_wathdog.alert();
        m_wathdog.join();
    }
}

TcpServer::TcpServer(uint16_t port, IMessageCallback* mc)
    : m_callback(mc)
    , m_soket(::socket(AF_INET, SOCK_STREAM, 0))
    , m_disconnectEvent(sketch::Event::autoReset)
{
    if (m_soket == INVALID_SOCKET)
    {
        throw sketch::Exception(SkWin32Error(::WSAGetLastError(), sketch::String()));
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = ::htons(port);
#if 1
    address.sin_addr.s_addr = ::inet_addr("127.0.0.1");
#else
    address.sin_addr.S_un.S_addr = INADDR_ANY;
#endif
    if (::bind(m_soket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
    {
        ::closesocket(m_soket);
        throw sketch::Exception(SkWin32Error(::WSAGetLastError(), sketch::String()));
    }

    if (::listen(m_soket, SOMAXCONN) != 0)
    {
        ::closesocket(m_soket);
        throw sketch::Exception(SkWin32Error(::WSAGetLastError(), sketch::String()));
    }

    m_worker = std::move(sketch::Thread(this, reinterpret_cast<void*>(WorkerThread), "TcpServer_worker"));
    m_wathdog = std::move(sketch::Thread(this, reinterpret_cast<void*>(WathdogThread), "TcpServer_watchdog"));
}

void TcpServer::run(void* ctx)
{
    auto who = reinterpret_cast<intptr_t>(ctx);
    if (who == WorkerThread)
    {
        _worker();
    }
    else if (who == WathdogThread)
    {
        _watchdog();
    }
    else
    {
        SK_ASSERT(0);
    }
}

void TcpServer::onDisconnect()
{
    m_disconnectEvent.set();
}

void TcpServer::_worker()
{
    while (!m_stopFlag)
    {
        auto s = ::accept(m_soket, nullptr, nullptr);
        if (s == INVALID_SOCKET)
        {
            SketchError("Failed to accept a connection, aborting");
            break;
        }

        try
        {
            TcpClient::Ref r(new TcpClient(s, this, m_callback));

            {
                sketch::Mutex::Guard lk(m_lock);
                m_clients.push_back(std::move(r));
            }

            SketchInfo("New connection accepted");
        }
        catch (sketch::Exception& e)
        {
            auto inner = e.error();
            auto f = inner->format(u"Failed to start a client thread: {MS} {FL}");

            auto l = sketch::toSystemLocale(f);
            SketchError(l.str());
        }
        catch (std::exception& e)
        {
            SketchError(e.what());
        }
    }
}

void TcpServer::_watchdog()
{
    while (!m_stopFlag)
    {
        auto wr = m_disconnectEvent.waitAlertable();
        if (wr == sketch::Event::success)
        {
            sketch::Mutex::Guard lk(m_lock);

            auto it = m_clients.begin();
            while (it != m_clients.end())
            {
                if (it->get()->dead())
                {
                    auto next = it;
                    next++;
                    m_clients.erase(it);
                    it = next;

                    SketchDebug("Client removed");
                }
                else
                {
                    it++;
                }
            }
        }
    }
}

#pragma endregion

} // namespace tracer {}
