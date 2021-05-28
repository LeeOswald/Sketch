#include <SkBase/Net.hxx>


#include <ip2string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


namespace sketch
{    

namespace net
{

bool initialize() noexcept
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }

    return true;
}

void finalize() noexcept
{
    WSACleanup();
}

} // namespace net {}
    

Vector<Net::Address> Net::hostNameToAddress(const wchar_t* host)
{
    Vector<Net::Address> v;

    ADDRINFOW hints = { 0 };
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    ADDRINFOW* result = nullptr;
    auto e = GetAddrInfoW(host, nullptr, &hints, &result);
    if (e)
    {
        setLastError(SkWin32Error(e, String()));
    }

    for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        switch (ptr->ai_family)
        {
        case AF_INET:
        case AF_INET6:
        {
            wchar_t buffer[46];
            DWORD l = _countof(buffer);
            e = WSAAddressToStringW(LPSOCKADDR(ptr->ai_addr), DWORD(ptr->ai_addrlen), nullptr, buffer, &l);
            if (e)
            {
                setLastError(SkWin32Error(e, String()));
            }
            else
            {
                Net::Address a;
                a.type = (ptr->ai_family == AF_INET) ? Net::Address::Type::ip4 : Net::Address::Type::ip6;
                a.address = String(buffer);
                v.push_back(a);
            }

            break;
        }
        }
    }

    FreeAddrInfoW(result);
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
    const struct sockaddr *skAddr = nullptr;
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

    wchar_t host[NI_MAXHOST];
    auto e = GetNameInfoW(skAddr, nameLen, host, _countof(host), nullptr, 0, NI_NAMEREQD);
    if (e != 0)
    {
        return String();
    }

    return String(host);
}


} // namespace sketch {}
