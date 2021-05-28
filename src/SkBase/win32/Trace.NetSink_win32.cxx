#include <SkBase/Event.hxx>
#include <SkBase/Net.hxx>
#include <SkBase/Time.hxx>

#include <ip2string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#include "../Trace_impl.hxx"
#include "../Trace.NetSink_impl.hxx"


namespace sketch
{

namespace log
{

namespace internal
{

class NetSinkImpl
    : public RefCountedBase<ITraceSink>
{
private:
    struct Request;
    using IoRequestT = IoRequest<Request>;

public:
    enum class Protocol
    {
        udp,
        tcp
    };

    ~NetSinkImpl() noexcept
    {
        _closeSocket();
    }

    NetSinkImpl(Protocol proto, const wchar_t* address, uint16_t port) noexcept
        : m_ConnectEx(nullptr)
        , m_proto(proto)
        , m_socket(INVALID_SOCKET)
        , m_connected(false)
        , m_terminating(false)
        , m_terminated(Event::manualReset)
        , m_addrLen(0)
    {
        auto skAddr = reinterpret_cast<sockaddr*>(&m_inAddr);
        auto in4Addr = reinterpret_cast<sockaddr_in*>(&m_inAddr);
        auto in6Addr = reinterpret_cast<sockaddr_in6*>(&m_inAddr);
        uint16_t dummyPort = 0;
        ULONG scopeId;

        auto status = RtlIpv4StringToAddressExW(address, TRUE, &in4Addr->sin_addr, &dummyPort);
        if (!NT_SUCCESS(status))
        {
            status = RtlIpv6StringToAddressExW(address, &in6Addr->sin6_addr, &scopeId, &dummyPort);
            if (!NT_SUCCESS(status))
            {
                log::bailOutWrite(log::Error, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Invalid IP address");
                SK_ASSERT(!"NetSinkImpl: Invalid IP address");
                return;
            }
            else
            {
                in6Addr->sin6_family = AF_INET6;
                in6Addr->sin6_scope_id = scopeId;
            }
        }
        else
        {
            in4Addr->sin_family = AF_INET;
        }

        if (skAddr->sa_family == AF_INET)
        {
            in4Addr->sin_port = htons(port);
            m_addrLen = sizeof(sockaddr_in);
        }
        else if (skAddr->sa_family == AF_INET6)
        {
            in6Addr->sin6_port = htons(port);
            m_addrLen = sizeof(*in6Addr);
        }
        else
        {
            log::bailOutWrite(log::Error, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Invalid IP address family");
            SK_ASSERT(!"NetSinkImpl: Invalid IP address family");
            return;
        }
    }

    void ITraceSink::write(Record::Ref r) noexcept override
    {
        if (!_checkSocket())
        {
            return;
        }

        if (m_proto == Protocol::tcp)
        {
            if (!_checkConnection(r->level() >= log::Warning))
            {
                return;
            }
        }

        auto req = packRecord<Request>(r, this);
        if (!req)
        {
            return;
        }

        auto sreq = req->sub();
        sreq->setData(req->data(), req->size());
        
        m_pending.push_back(req.detach());

        DWORD wr = 0;
        int se = 0;
        if (m_proto == Protocol::tcp)
        {
             se = WSASend(
                m_socket,
                &sreq->wsaBuf,
                1,
                &wr,
                0,
                &sreq->overlapped,
                _completionProc
            );
        }
        else if (m_proto == Protocol::udp)
        {
            se = WSASendTo(
                m_socket,
                &sreq->wsaBuf,
                1,
                &wr,
                0,
                reinterpret_cast<sockaddr*>(&m_inAddr),
                m_addrLen,
                &sreq->overlapped,
                _completionProc
            );
        }

        if (se == SOCKET_ERROR)
        {
            auto e = WSAGetLastError();
            if (e != WSA_IO_PENDING)
            {
                req->setStatus(e, 0);

                if (m_proto == Protocol::tcp)
                {
                    if (
                        (e == WSAECONNABORTED) ||
                        (e == WSAECONNRESET)
                        )
                    {
                        log::bailOutWrite(log::Warning, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Connection lost");

                        _closeSocket();
                    }
                }

                return;
            }
        }
    }

private:
    struct Request
        : public NonCopyable
    {
        NetSinkImpl* owner;
        WSABUF wsaBuf;
        WSAOVERLAPPED overlapped;

        ~Request() noexcept
        {
        }

        Request(NetSinkImpl* _owner) noexcept
            : owner(_owner)
            , wsaBuf()
            , overlapped()
        {
            overlapped.hEvent = reinterpret_cast<HANDLE>(this);
        }

        IoRequestT* outer() noexcept
        {
            return IoRequestT::master(this);
        }

        void setData(void* data, size_t size) noexcept
        {
            wsaBuf = { static_cast<ULONG>(size), static_cast<char*>(data) };
        }
    };

    static void CALLBACK _completionProc(
        DWORD dwError,
        DWORD cbTransferred,
        LPWSAOVERLAPPED lpOverlapped,
        DWORD dwFlags
    ) noexcept
    {
        SK_UNUSED(dwFlags);

        auto r = reinterpret_cast<Request*>(lpOverlapped->hEvent);
        SK_ASSERT(r);

        auto outer = IoRequestT::Ref(r->outer()); // get ownership
        SK_ASSERT(outer);
        outer->setStatus(dwError, cbTransferred);

        auto _this = r->owner;
        _this->m_pending.erase(outer.get());
        if (_this->m_pending.empty())
        {
            if (_this->m_terminating)
            {
                _this->m_terminated.set();
            }
        }
    }

    static LPFN_CONNECTEX _getConnectExAddr(SOCKET socket) noexcept
    {
        LPFN_CONNECTEX ret = nullptr;
        GUID guid = WSAID_CONNECTEX;
        DWORD retBytes;

        auto e = WSAIoctl(
            socket,
            SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guid,
            sizeof(guid),
            &ret,
            sizeof(ret),
            &retBytes,
            nullptr,
            nullptr
        );

        if (e == SOCKET_ERROR)
        {
            return nullptr;
        }

        return ret;
    }

    void _closeSocket() noexcept
    {
        if (m_socket != INVALID_SOCKET)
        {
            shutdown(m_socket, SD_BOTH);

            m_connected = false;
            m_terminating = true;

            if (CancelIoEx((HANDLE)m_socket, nullptr))
            {
                m_terminated.wait();
            }

            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

        // delete pending requests
        IoRequestT::Ref req(m_pending.pull_front());
        while (req)
        {
            req = sketch::move(IoRequestT::Ref(m_pending.pull_front()));
        }
    }

    bool _checkSocket() noexcept
    {
        if (m_socket != INVALID_SOCKET)
        {
            return true;
        }

        m_terminating = false;
        m_terminated.clear();

        if (m_proto == Protocol::udp)
        {
            m_socket = WSASocket(
                m_inAddr.ss_family,
                SOCK_DGRAM,
                IPPROTO_UDP,
                nullptr,
                0,
                WSA_FLAG_OVERLAPPED
            );
        }
        else if (m_proto == Protocol::tcp)
        {
            m_socket = WSASocket(
                m_inAddr.ss_family,
                SOCK_STREAM,
                IPPROTO_TCP,
                nullptr,
                0,
                WSA_FLAG_OVERLAPPED
            );
        }
        else
        {
            SK_ASSERT(!"NetSinkImpl: Unsupported protocol");
            return false;
        }

        if (m_socket == INVALID_SOCKET)
        {
            SK_ASSERT(!"NetSinkImpl: WSASocket() failed");
            return false;
        }

        sockaddr_storage local = { 0 };
        local.ss_family = m_inAddr.ss_family;
        auto e = bind(m_socket, reinterpret_cast<const sockaddr*>(&local), m_addrLen);
        if (e == SOCKET_ERROR)
        {
            SK_ASSERT(!"NetSinkImpl: bind() failed");
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            return false;
        }

        if (m_proto == Protocol::tcp)
        {
            m_ConnectEx = _getConnectExAddr(m_socket);
            if (!m_ConnectEx)
            {
                SK_ASSERT(!"NetSinkImpl: _getConnectExAddr() failed");
                closesocket(m_socket);
                m_socket = INVALID_SOCKET;
                return false;
            }
        }

        return true;
    }

    bool _checkConnection(bool force) noexcept
    {
        if (!m_connected)
        {
            auto now = Time::now();

            if (m_lastReconnectAttempt.value())
            {
                if (!force)
                {
                    if (now.toMilliseconds() - m_lastReconnectAttempt.toMilliseconds() < 5000)
                    {
                        return false;
                    }
                }
            }

            m_lastReconnectAttempt = now;

            log::bailOutWrite(log::Info, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Trying to reconnect...");
            
            WSAOVERLAPPED ol;
            ol.hEvent = WSACreateEvent();
            if (ol.hEvent == WSA_INVALID_EVENT)
            {
                SK_ASSERT(0);
                return false;
            }

            SK_ASSERT(m_ConnectEx);
            if (!m_ConnectEx(
                m_socket,
                (const sockaddr*)&m_inAddr,
                m_addrLen,
                nullptr,
                0,
                nullptr,
                &ol
            ))
            {
                auto e = WSAGetLastError();
                if (e == WSA_IO_PENDING)
                {
                    DWORD bytesNum;
                    DWORD flags;

                    e = WSAWaitForMultipleEvents(1, &ol.hEvent, TRUE, INFINITE, TRUE);
                    while ((e == WSA_WAIT_IO_COMPLETION) && !m_terminating)
                    {
                        e = WSAWaitForMultipleEvents(1, &ol.hEvent, TRUE, INFINITE, TRUE);
                    }

                    if (e == WSA_WAIT_EVENT_0)
                    {
                        if (WSAGetOverlappedResult(m_socket, &ol, &bytesNum, FALSE, &flags))
                        {
                            m_connected = true;
                        }
                        else
                        {
                            e = WSAGetLastError();
                        }
                    }
                }

                if (!m_connected)
                {
                    log::bailOutWrite(log::Error, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Failed to connect: %d", e);
                }
            }

            WSACloseEvent(ol.hEvent);
        }

        return m_connected;
    }

private:
    LPFN_CONNECTEX m_ConnectEx;
    Protocol m_proto;
    SOCKET m_socket;
    bool m_connected;
    bool m_terminating;
    Event m_terminated;
    IntrusiveList<IoRequestT, &IoRequestT::link> m_pending;
    sockaddr_storage m_inAddr;
    int32_t m_addrLen;
    Time m_lastReconnectAttempt;
};

} // namespace internal {}



ITraceSink::Ref createUdpSink(const wchar_t* address, uint16_t port)
{
    return ITraceSink::Ref(new (sketch::nothrow) internal::NetSinkImpl(internal::NetSinkImpl::Protocol::udp, address, port));
}

ITraceSink::Ref createTcpSink(const wchar_t* address, uint16_t port)
{
    return ITraceSink::Ref(new (sketch::nothrow) internal::NetSinkImpl(internal::NetSinkImpl::Protocol::tcp, address, port));
}

} // namespace log {}

} // namespace sketch {}
