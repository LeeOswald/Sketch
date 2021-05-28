#pragma once

#include <Sketch.hxx>

#include <string>
#include <thread>

#include <winsock2.h>


class FakeClient
    : public sketch::ErrorState
{
public:
    enum class Protocol
    {
        udp,
        tcp
    };

    ~FakeClient();
    FakeClient(Protocol proto, const wchar_t* address, uint16_t port);

private:
    bool _checkConnection();
    void _workerThread();
    sketch::Pair<size_t, sketch::UniquePtr<uint8_t[]>> _fakeMessage();

private:
    SOCKET m_socket{ INVALID_SOCKET };
    std::thread m_worker;
    bool m_stop{ false };
    bool m_connected{ false };
    Protocol m_protocol;
    struct sockaddr_storage m_inAddr;
    int32_t m_addrLen{ 0 };
};
