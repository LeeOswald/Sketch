#include "stdafx.h"
#include "FakeClient.h"
#include "Protocol.h"

#include <jansson/sk_jansson.hxx>

#include <ws2tcpip.h>
#include <ip2string.h>

FakeClient::~FakeClient()
{
    m_stop = true;

    if (m_worker.joinable())
    {
        m_worker.join();
    }

    if (m_socket != INVALID_SOCKET)
    {
        ::closesocket(m_socket);
    }
}

FakeClient::FakeClient(Protocol proto, const wchar_t* address, uint16_t port)
    : m_protocol(proto)
{
    const struct sockaddr* skAddr = (struct sockaddr*)&m_inAddr;
    struct sockaddr_in* in4Addr = (struct sockaddr_in*)&m_inAddr;
    struct sockaddr_in6* in6Addr = (struct sockaddr_in6*)&m_inAddr;
    uint16_t dummyPort = 0;
    ULONG scopeId;

    auto status = ::RtlIpv4StringToAddressExW(address, TRUE, &in4Addr->sin_addr, &dummyPort);
    if (!NT_SUCCESS(status))
    {
        status = ::RtlIpv6StringToAddressExW(address, &in6Addr->sin6_addr, &scopeId, &dummyPort);
        if (!NT_SUCCESS(status))
        {
            setLastError(SkNtError(status, sketch::String()));
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
        setLastError(SkNtError(STATUS_INVALID_PARAMETER, sketch::String()));
        return;
    }

    if (proto == Protocol::tcp)
    {
        m_socket = ::WSASocket(
            skAddr->sa_family,
            SOCK_STREAM,
            IPPROTO_TCP,
            nullptr,
            0,
            0
        );
    }
    else if (proto == Protocol::udp)
    {
        m_socket = ::WSASocket(
            skAddr->sa_family,
            SOCK_DGRAM,
            IPPROTO_UDP,
            nullptr,
            0,
            0
        );
    }
    else
    {
        SK_ASSERT(0);
    }

    if (m_socket == INVALID_SOCKET)
    {
        setLastError(SkWin32Error(::WSAGetLastError(), sketch::String()));
        return;
    }

    if (proto == Protocol::udp)
    {
        DWORD v = 0;
        int l = sizeof(v);
        if (::getsockopt(m_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast<char*>(&v), &l) == 0)
        {
            SketchInfo("Max message size %d", v);
        }
    }

    struct sockaddr_storage local = { 0 };
    local.ss_family = (ADDRESS_FAMILY)skAddr->sa_family;
    auto e = ::bind(m_socket, (const struct sockaddr*)&local, m_addrLen);
    if (e == SOCKET_ERROR)
    {
        ::closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        setLastError(SkWin32Error(::WSAGetLastError(), sketch::String()));
        return;
    }

    m_worker = std::move(std::thread([this]() { _workerThread();  }));
}

bool FakeClient::_checkConnection()
{
    if (!m_connected)
    {
        auto e = ::connect(m_socket, (struct sockaddr*)&m_inAddr, m_addrLen);
        if (e == SOCKET_ERROR)
        {
            SketchError("Failed to connect: %d", ::WSAGetLastError());
        }
        else
        {
            m_connected = true;
        }
    }

    return m_connected;
}

sketch::Pair<size_t, sketch::UniquePtr<uint8_t[]>> FakeClient::_fakeMessage()
{
    auto level = static_cast<sketch::log::Level>(::rand() % int(sketch::log::Off));
    static const char* s_msgs[] = {
        "Fuck it!",
        "Dam it, pal.",
        "No you.",
        "LOL u nub",
        "Can I eat your ass?",
        "I just wanted to ask",
        "Write back as soon as you can",
        "Dear Chelsey Lane"
    };

    int mi = ::rand() % _countof(s_msgs);

    auto r = sketch::log::Record::make(level, "Fake", __FILE__, __LINE__, sketch::AnsiString(s_msgs[mi]));

    sketch::UniquePtr<uint8_t[]> result;
    size_t resultSize = 0;

    auto j = json_object();
    if (j)
    {
        json_object_set_new(j, "level", json_integer(r->level()));
        json_object_set_new(j, "module", json_string(r->module()));
        json_object_set_new(j, "file", json_string(r->file()));
        json_object_set_new(j, "line", json_integer(r->line()));

        {
            auto date = r->time().unpack();
            auto f = sketch::AnsiString::format(
                "%02d:%02d:%02d.%04d",
                date.hour(),
                date.minute(),
                date.second(),
                date.millisecond()
            );

            json_object_set_new(j, "time", json_string(f.str()));
        }

        json_object_set_new(j, "pid", json_integer(r->pid()));
        json_object_set_new(j, "tid", json_integer(r->tid()));

        json_object_set_new(j, "irql", json_integer(::rand() % 3));

        json_object_set_new(j, "text", json_string(r->text().str()));

        auto data = json_dumps(j, JSON_COMPACT);
        json_delete(j);

        auto l = strlen(data) + 1;
        auto required = sizeof(protocol::Header) + l;
        result.reset(new (std::nothrow) uint8_t[required]);
        if (result)
        {
            auto hdr = reinterpret_cast<protocol::Header*>(result.get());
            hdr->size = l;
            auto msg = reinterpret_cast<char*>(hdr + 1);
            memcpy(msg, data, l);
            resultSize = required;
        }

        SkFree(data);
    }

    return sketch::Pair<size_t, sketch::UniquePtr<uint8_t[]>>(resultSize, std::move(result));
}

void FakeClient::_workerThread()
{
    SketchInfo("FakeClient started");

    ::srand(::GetTickCount());

    try
    {
        while (!m_stop)
        {
            auto m = _fakeMessage();
            if (!m.first) continue;

            if (m_protocol == Protocol::tcp)
            {
                if (_checkConnection())
                {
                    auto sd = ::send(m_socket, reinterpret_cast<const char*>(m.second.get()), m.first, 0);
                    if (sd == SOCKET_ERROR)
                    {
                        if (::WSAGetLastError() == WSAEMSGSIZE)
                        {
                            SketchError("Message too large (%d bytes)", m.first);
                            continue;
                        }

                        SketchError("Failed to send the message");
                    }
                }
                else
                {
                    sketch::thread::current::sleep(1000);
                }
            }
            else if (m_protocol == Protocol::udp)
            {
                auto sd = ::sendto(m_socket, reinterpret_cast<const char*>(m.second.get()), m.first, 0, (struct sockaddr*)&m_inAddr, m_addrLen);
                if (sd == SOCKET_ERROR)
                {
                    if (::WSAGetLastError() == WSAEMSGSIZE)
                    {
                        SketchError("Message too large (%d bytes)", m.first);
                        continue;
                    }

                    SketchError("Failed to send the message");
                }
            }

            sketch::thread::current::sleep(200);
        }
    }
    catch (sketch::Exception& e)
    {
        auto inner = e.error();
        auto f = inner->format(u"Unexpected exception: {MS} {FL}");

        auto l = sketch::toSystemLocale(f);
        SketchError(l.str());
    }
    catch (std::exception& e)
    {
        SketchError(e.what());
    }

    ::shutdown(m_socket, SD_BOTH);

    SketchInfo("FakeClient stopped");
}
