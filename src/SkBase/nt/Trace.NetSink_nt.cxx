#include <SkBase/Event.hxx>
#include <SkBase/Net.hxx>
#include <SkBase/Time.hxx>

#include "Net_nt.hxx"
#include "../Trace_impl.hxx"
#include "../Trace.NetSink_impl.hxx"

#include <wsk.h>


#define htons(V) RtlUshortByteSwap(V)


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
        : m_proto(proto)
        , m_socket(nullptr)
        , m_connected(false)
        , m_inAddr()
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
            m_addrLen = sizeof(struct sockaddr_in);
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
        if (!sreq->valid())
        {
            return;
        }

        m_pending.push_back(req.detach());

        if (m_proto == Protocol::tcp)
        {
            auto dispatch = PWSK_PROVIDER_CONNECTION_DISPATCH(m_socket->Dispatch);
            auto sta = dispatch->WskSend(m_socket, sreq->wskBuf(), 0, sreq->irp());

            if (sta != STATUS_PENDING)
            {
                log::bailOutWrite(log::Debug, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: WskSend -> %08x", sta);
                return;
            }
        }
        else if (m_proto == Protocol::udp)
        {
            auto dispatch = PWSK_PROVIDER_DATAGRAM_DISPATCH(m_socket->Dispatch);
            auto sta = dispatch->WskSendTo(
                m_socket,
                sreq->wskBuf(),
                0,
                reinterpret_cast<PSOCKADDR>(&m_inAddr),
                0,
                nullptr,
                sreq->irp()
            );

            if (sta != STATUS_PENDING)
            {
                log::bailOutWrite(log::Debug, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: WskSendTo -> %08x", sta);
                return;
            }
        }
    }

private:
    struct Request
        : public NonCopyable
    {
        ~Request() noexcept
        {
            if (m_wskBuf.Mdl)
            {
                _freeMdlChain(m_wskBuf.Mdl);
            }

            if (m_irp)
            {
                IoFreeIrp(m_irp);
            }
        }

        Request(NetSinkImpl* _owner) noexcept
            : m_owner(_owner)
            , m_irp(IoAllocateIrp(1, FALSE))
            , m_complete(Event::manualReset)
            , m_wskBuf()
        {
            if (m_irp)
            {
                IoReuseIrp(m_irp, STATUS_UNSUCCESSFUL);
                IoSetCompletionRoutine(m_irp, _ioReqIrpCompletion, this, TRUE, TRUE, TRUE);
            }
        }

        void setData(void* data, size_t size) noexcept
        {
            if (data && size)
            {
                m_wskBuf.Length = size;
                m_wskBuf.Offset = 0;
                m_wskBuf.Mdl = IoAllocateMdl(data, ULONG(size), FALSE, FALSE, m_irp);
                if (m_wskBuf.Mdl)
                {
                    MmBuildMdlForNonPagedPool(m_wskBuf.Mdl);
                    m_wskBuf.Mdl->Next = nullptr;
                }
            }
        }

        NetSinkImpl* owner() noexcept
        {
            return m_owner;
        }

        IoRequestT* outer() noexcept
        {
            return IoRequestT::master(this);
        }

        bool valid() const noexcept
        {
            if (!m_irp) return false;
            if (m_wskBuf.Length && !m_wskBuf.Mdl) return false;

            return true;
        }

        bool wait() noexcept
        {
            return m_complete.wait();
        }

        NTSTATUS status() const noexcept
        {
            return m_iosb.Status;
        }

        uintptr_t information() const noexcept
        {
            return m_iosb.Information;
        }

        PIRP irp() noexcept
        {
            return m_irp;
        }

        PWSK_BUF wskBuf() noexcept
        {
            return &m_wskBuf;
        }

    private:
        static NTSTATUS _ioReqIrpCompletion(PDEVICE_OBJECT devObj, PIRP irp, PVOID ctx) noexcept
        {
            SK_UNUSED(devObj);
            
            auto _this = static_cast<Request*>(ctx);
            _this->m_iosb.Status = irp->IoStatus.Status;
            _this->m_iosb.Information = irp->IoStatus.Information;

            auto owner = _this->owner();
            SK_ASSERT(owner);

            auto outer = _this->outer();
            SK_ASSERT(outer);

            outer->setStatus(_this->m_iosb.Status, _this->m_iosb.Information);

            if (outer->type() == IoRequestT::write)
            {
                if (!outer->resultSize())
                {
                    log::bailOutWrite(log::Warning, log::g_ModuleName, __FILE__, __LINE__, "NetSinkImpl: Connection lost");
                    owner->m_connected = false;
                }

                // release the request
                auto o = IoRequestT::Ref(_this->outer()); // get ownership
                owner->m_pending.erase(o.get());

                // no need to set the event
                return STATUS_MORE_PROCESSING_REQUIRED;
            }

            // this is a synchronous request, the caller will release the request object
            _this->m_complete.set();

            return STATUS_MORE_PROCESSING_REQUIRED;
        }

        static void _freeMdlChain(PMDL chain)
        {
            PMDL cur = chain;

            while (cur)
            {
                PMDL next = cur->Next;
                IoFreeMdl(cur);
                cur = next;
            }
        }

    private:
        NetSinkImpl* m_owner;
        PIRP m_irp;
        IO_STATUS_BLOCK m_iosb;
        Event m_complete;
        WSK_BUF m_wskBuf;
    };

    bool _checkSocket()
    {
        if (m_socket)
        {
            return true;
        }

        // create socket
        {
            auto req = IoRequestT::make(IoRequestT::create, nullptr, nullptr, 0, this);
            if (!req) return false;
            auto sreq = req->sub();
            if (!sreq->valid()) return false;

            NTSTATUS sta = STATUS_UNSUCCESSFUL;
            if (m_proto == Protocol::tcp)
            {
                sta = net::g_WskData.wskProvNpi.Dispatch->WskSocket(
                    net::g_WskData.wskProvNpi.Client,
                    m_inAddr.ss_family,
                    SOCK_STREAM,
                    IPPROTO_TCP,
                    WSK_FLAG_CONNECTION_SOCKET,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    sreq->irp()
                );
            }
            else if (m_proto == Protocol::udp)
            {
                sta = net::g_WskData.wskProvNpi.Dispatch->WskSocket(
                    net::g_WskData.wskProvNpi.Client,
                    m_inAddr.ss_family,
                    SOCK_DGRAM,
                    IPPROTO_UDP,
                    WSK_FLAG_DATAGRAM_SOCKET,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    sreq->irp()
                );
            }
            else
            {
                SK_ASSERT(!"NetSinkImpl: Unsupported protocol");
                return false;
            }

            if (!NT_SUCCESS(sta))
            {
                SK_ASSERT(!"NetSinkImpl: WskSocket() failed");
                return false;
            }

            sreq->wait();
            if (!NT_SUCCESS(sreq->status()))
            {
                SK_ASSERT(!"NetSinkImpl: WskSocket() wait failed");
                return false;
            }

            m_socket = reinterpret_cast<PWSK_SOCKET>(sreq->information());
        }

        // bind socket
        {
            auto req = IoRequestT::make(IoRequestT::bind, nullptr, nullptr, 0, this);
            if (!req)
            {
                _closeSocket();
                return false;
            }

            auto sreq = req->sub();
            if (!sreq->valid())
            {
                _closeSocket();
                return false;
            }

            NTSTATUS sta = STATUS_UNSUCCESSFUL;

            sockaddr_storage local = { 0 };
            local.ss_family = m_inAddr.ss_family;

            if (m_proto == Protocol::tcp)
            {
                auto dispatch = PWSK_PROVIDER_CONNECTION_DISPATCH(m_socket->Dispatch);
                sta = dispatch->WskBind(m_socket, reinterpret_cast<PSOCKADDR>(&local), 0, sreq->irp());
            }
            else if (m_proto == Protocol::udp)
            {
                auto dispatch = PWSK_PROVIDER_DATAGRAM_DISPATCH(m_socket->Dispatch);
                sta = dispatch->WskBind(m_socket, reinterpret_cast<PSOCKADDR>(&local), 0, sreq->irp());
            }

            if (!NT_SUCCESS(sta))
            {
                SK_ASSERT(!"NetSinkImpl: WskBind() failed");
                _closeSocket();
                return false;
            }

            sreq->wait();
            if (!NT_SUCCESS(sreq->status()))
            {
                SK_ASSERT(!"NetSinkImpl: WskBind() wait failed");
                _closeSocket();
                return false;
            }
        }

        return true;
    }

    bool _checkConnection(bool force)
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

            auto req = IoRequestT::make(IoRequestT::connect, nullptr, nullptr, 0, this);
            if (req)
            {
                auto sreq = req->sub();
                if (sreq->valid())
                {
                    auto dispatch = PWSK_PROVIDER_CONNECTION_DISPATCH(m_socket->Dispatch);
                    if (NT_SUCCESS(dispatch->WskConnect(m_socket, reinterpret_cast<PSOCKADDR>(&m_inAddr), 0, sreq->irp())))
                    {
                        sreq->wait();
                        if (NT_SUCCESS(sreq->status()))
                        {
                            m_connected = true;
                        }
                    }
                }
            }
        }

        return m_connected;
    }

    void _closeSocket()
    {
        if (m_socket)
        {
            m_connected = false;

            if (m_connected)
            {
                SK_ASSERT(m_proto == Protocol::tcp);

                auto req = IoRequestT::make(IoRequestT::disconnect, nullptr, nullptr, 0, this);
                if (req)
                {
                    auto sreq = req->sub();
                    if (sreq->valid())
                    {
                        auto dispatch = PWSK_PROVIDER_CONNECTION_DISPATCH(m_socket->Dispatch);
                        if (NT_SUCCESS(dispatch->WskDisconnect(m_socket, nullptr, 0, sreq->irp())))
                        {
                            sreq->wait();

                            m_connected = false;
                        }
                    }
                }
            }

            auto req = IoRequestT::make(IoRequestT::close, nullptr, nullptr, 0, this);
            if (req)
            {
                auto sreq = req->sub();
                if (sreq->valid())
                {
                    NTSTATUS sta = STATUS_UNSUCCESSFUL;
                    if (m_proto == Protocol::tcp)
                    {
                        auto dispatch = PWSK_PROVIDER_CONNECTION_DISPATCH(m_socket->Dispatch);
                        sta = dispatch->Basic.WskCloseSocket(m_socket, sreq->irp());
                    }
                    else if (m_proto == Protocol::udp)
                    {
                        auto dispatch = PWSK_PROVIDER_DATAGRAM_DISPATCH(m_socket->Dispatch);
                        sta = dispatch->Basic.WskCloseSocket(m_socket, sreq->irp());
                    }

                    if (NT_SUCCESS(sta))
                    {
                        sreq->wait();

                        m_socket = nullptr;
                    }
                }
            }
        }

        // delete pending requests
        IoRequestT::Ref req(m_pending.pull_front());
        while (req)
        {
            req = sketch::move(IoRequestT::Ref(m_pending.pull_front()));
        }
    }

private:
    Protocol m_proto;
    PWSK_SOCKET m_socket;
    bool m_connected;
    sockaddr_storage m_inAddr;
    int32_t m_addrLen;
    Time m_lastReconnectAttempt;
    IntrusiveList<IoRequestT, &IoRequestT::link> m_pending;
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
