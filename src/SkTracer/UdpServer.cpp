#include "stdafx.h"
#include "UdpServer.h"

#include <SkBase/Vector.hxx>

namespace tracer
{

UdpServer::~UdpServer()
{
    m_stopFlag = true;

    if (m_socket != INVALID_SOCKET)
    {
        ::closesocket(m_socket);
    }

    if (m_worker.valid())
    {
        m_worker.join();
    }
}

UdpServer::UdpServer(uint16_t port, IMessageCallback* mc)
    : m_callback(mc)
    , m_socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
    , m_bufferSize(0)
{
    if (m_socket == INVALID_SOCKET)
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
    if (::bind(m_socket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
    {
        ::closesocket(m_socket);
        throw sketch::Exception(SkWin32Error(::WSAGetLastError(), sketch::String()));
    }

    {
        DWORD v = 0;
        int l = sizeof(v);
        if (::getsockopt(m_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast<char*>(&v), &l) != 0)
        {
            throw sketch::Exception(SkWin32Error(::WSAGetLastError(), sketch::String()));
        }

        m_buffer.reset(new uint8_t[v]);
        m_bufferSize = v;
        SketchDebug("Allocated a %d bytes long UDP buffer", v);
    }

    m_worker = std::move(sketch::Thread(this, nullptr, "UdpServer_worker"));
}

void UdpServer::run(void* ctx)
{
    SK_UNUSED(ctx);

    while (!m_stopFlag)
    {
        auto pkt = reinterpret_cast<protocol::Header*>(m_buffer.get());
        {
            auto rd = ::recvfrom(m_socket, reinterpret_cast<char*>(m_buffer.get()), m_bufferSize, 0, nullptr, nullptr);
            if (rd == SOCKET_ERROR)
            {
                auto e = ::WSAGetLastError();
                SketchError("Failed to receive a message: %d", e);
                break;
            }

            if (rd == 0)
            {
                continue;
            }

            if (rd < int(sizeof(protocol::Header)))
            {
                SketchError("Received only %d bytes of header, aborting", rd);
                break;
            }
            
            if (rd < int(pkt->size + sizeof(protocol::Header)))
            {
                SketchError("Received only %d bytes of data (%d promised), aborting", rd, pkt->size);
                break;
            }
        }

        try
        {
            auto s = reinterpret_cast<char*>(m_buffer.get());
            s += sizeof(protocol::Header);

            s[pkt->size] = 0; // force null terminator

            auto r = std::move(protocol::Record::unpack(s));
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

    ::shutdown(m_socket, SD_BOTH);

    SketchInfo("Socket closed");
}


} // namespace tracer {}
