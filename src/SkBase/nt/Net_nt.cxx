#include <SkBase/Event.hxx>
#include <SkBase/Net.hxx>

#include "Net_nt.hxx"

#include <wsk.h>


namespace sketch
{

namespace net
{


WskData g_WskData = { 0 };


bool initialize() noexcept
{
    g_WskData.wskCliDisp.Version = MAKE_WSK_VERSION(1, 0);
    g_WskData.wskCliDisp.WskClientEvent = NULL;
    g_WskData.wskCliDisp.Reserved = 0;

    g_WskData.wskCliNpi.Dispatch = &g_WskData.wskCliDisp;
    g_WskData.wskCliNpi.ClientContext = NULL;

    auto e = WskRegister(&g_WskData.wskCliNpi, &g_WskData.wskReg);
    if (e < 0)
    {
        return false;
    }

    e = WskCaptureProviderNPI(&g_WskData.wskReg, WSK_INFINITE_WAIT, &g_WskData.wskProvNpi);
    if (e < 0)
    {
        WskDeregister(&g_WskData.wskReg);
        return false;
    }

    return true;
}

void finalize() noexcept
{
    WskReleaseProviderNPI(&g_WskData.wskReg);
    WskDeregister(&g_WskData.wskReg);
}

} // namespace net {}


class Request
    : public NonCopyable
{
public:
    ~Request() noexcept
    {
        if (m_irp)
        {
            IoFreeIrp(m_irp);
        }
    }

    Request() noexcept
        : m_irp(IoAllocateIrp(1, FALSE))
        , m_complete(Event::manualReset)
    {
        if (m_irp)
        {
            IoReuseIrp(m_irp, STATUS_UNSUCCESSFUL);
            IoSetCompletionRoutine(m_irp, _ioReqIrpCompletion, this, TRUE, TRUE, TRUE);
        }
    }

    bool valid() const noexcept
    {
        if (!m_irp) return false;

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

    PIRP irp() noexcept
    {
        return m_irp;
    }

private:
    static NTSTATUS _ioReqIrpCompletion(PDEVICE_OBJECT devObj, PIRP irp, PVOID ctx) noexcept
    {
        SK_UNUSED(devObj);
        SK_UNUSED(irp);

        auto _this = static_cast<Request*>(ctx);
        _this->m_iosb.Status = irp->IoStatus.Status;
        _this->m_iosb.Information = irp->IoStatus.Information;

        _this->m_complete.set();

        return STATUS_MORE_PROCESSING_REQUIRED;
    }

private:
    PIRP m_irp;
    IO_STATUS_BLOCK m_iosb;
    Event m_complete;
};



Vector<Net::Address> Net::hostNameToAddress(const wchar_t* host)
{
    Vector<Net::Address> v;

    UNICODE_STRING usHost;
    RtlInitUnicodeString(&usHost, host);

    Request r;
    if (r.valid())
    {
        ADDRINFOEXW hints = { 0 };
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        ADDRINFOEXW* result = nullptr;

        net::g_WskData.wskProvNpi.Dispatch->WskGetAddressInfo(
            net::g_WskData.wskProvNpi.Client,
            &usHost,
            nullptr,
            NS_ALL,
            nullptr,
            &hints,
            &result,
            nullptr,
            nullptr,
            r.irp()
        );

        r.wait();

        if (r.status() == STATUS_SUCCESS)
        {
            for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
            {
                switch (ptr->ai_family)
                {
                case AF_INET:
                {
                    wchar_t buffer[46];
                    auto sa = &((sockaddr_in*)(ptr->ai_addr))->sin_addr;
                    RtlIpv4AddressToStringW(sa, buffer);

                    Net::Address a;
                    a.type = Net::Address::Type::ip4;
                    a.address = String(buffer);
                    v.push_back(a);

                    break;
                }

                case AF_INET6:
                {
                    wchar_t buffer[46];
                    auto sa = &((sockaddr_in6*)(ptr->ai_addr))->sin6_addr;
                    RtlIpv6AddressToStringW(sa, buffer);

                    Net::Address a;
                    a.type = Net::Address::Type::ip6;
                    a.address = String(buffer);
                    v.push_back(a);

                    break;
                }

                }
            }

            net::g_WskData.wskProvNpi.Dispatch->WskFreeAddressInfo(
                net::g_WskData.wskProvNpi.Client,
                result
            );
        }
        else
        {
            setLastError(SkNtError(r.status(), String()));
        }
    }

    return v;
}

String Net::hostNameToAddressSimple(const wchar_t* host)
{
    auto v = sketch::move(hostNameToAddress(host));
    if (v.empty()) return String();
    for (const auto& a : v)
    {
        if (a.type == Net::Address::Type::ip4) return a.address.str();
    }

    return String();
}

String Net::addressToHostName(const wchar_t* address)
{
    struct sockaddr *skAddr = nullptr;
    uint16_t port = 0;
    int32_t nameLen = 0;

    struct sockaddr_in in4Addr = { 0 };
    NTSTATUS sta = RtlIpv4StringToAddressExW(address, TRUE, &in4Addr.sin_addr, &port);
    if (!NT_SUCCESS(sta))
    {
        struct sockaddr_in6 in6Addr = { 0 };
        ULONG scopeId;
        sta = RtlIpv6StringToAddressExW(address, &in6Addr.sin6_addr, &scopeId, &port);
        if (!NT_SUCCESS(sta))
        {
            return String();
        }
        else
        {
            in6Addr.sin6_family = AF_INET6;
            in6Addr.sin6_port = port;
            in6Addr.sin6_scope_id = scopeId;
            nameLen = sizeof(in6Addr);
            skAddr = (struct sockaddr *)&in6Addr;
        }
    }
    else
    {
        in4Addr.sin_family = AF_INET;
        in4Addr.sin_port = port;
        nameLen = sizeof(struct sockaddr_in);
        skAddr = (struct sockaddr *)&in4Addr;
    }

    UNICODE_STRING usHost;
    usHost.Length = 0;
    usHost.MaximumLength = NI_MAXHOST * sizeof(WCHAR);
    usHost.Buffer = static_cast<PWCH>(SkMalloc(usHost.MaximumLength));
    if (!usHost.Buffer)
    {
        return String();
    }

    String result;

    Request r;
    if (r.valid())
    {
        net::g_WskData.wskProvNpi.Dispatch->WskGetNameInfo(
            net::g_WskData.wskProvNpi.Client,
            static_cast<PSOCKADDR>(skAddr),
            nameLen,
            &usHost,
            nullptr,
            NI_NAMEREQD,
            nullptr,
            nullptr,
            r.irp()
        );

        r.wait();

        if (r.status() == STATUS_SUCCESS)
        {
            result = String(&usHost);
        }
    }

    SkFree(usHost.Buffer);

    return result;
}


} // namespace sketch {}
