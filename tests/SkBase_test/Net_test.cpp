#include "common.h"

#include <SkBase/Net.hxx>
#include <SkBase/System.hxx>


using namespace sketch;


SKETCH_TEST_BEGIN(Net, "Net::hostNameToAddress()")

    Net n;

    {
        static const wchar_t* const hosts[] =
        {
            L"google.com",
            L"yandex.ru",
            L"microsoft.com"
        };

        for (auto h : hosts)
        {
            auto v = n.hostNameToAddress(h);
            SKETCH_TEST_ASSERT(!v.empty());

            for (const auto& a : v)
            {
                SKETCH_TEST_ASSERT(!a.address.empty());
                SKETCH_TEST_ASSERT(a.type != Net::Address::Type::unknown);

                SketchInfo("[%ws] %ws -> %ws", (a.type == Net::Address::Type::ip4 ? L"IPv4" : L"IPv6"), h, a.address.utf16());
            }
        }
    }

    {
        auto my = system::machineName();
        auto v = n.hostNameToAddress(my.utf16());
        SKETCH_TEST_ASSERT(!v.empty());

        for (const auto& a : v)
        {
            SKETCH_TEST_ASSERT(!a.address.empty());
            SKETCH_TEST_ASSERT(a.type != Net::Address::Type::unknown);

            SketchInfo("[%ws] %ws -> %ws", (a.type == Net::Address::Type::ip4 ? L"IPv4" : L"IPv6"), my.utf16(), a.address.utf16());
        }
    }

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Net, "Net::hostNameToAddressSimple()")

    Net n;

    {
        static const wchar_t* const hosts[] =
        {
            L"google.com",
            L"yandex.ru",
            L"microsoft.com"
        };

        for (auto h : hosts)
        {
            auto v = n.hostNameToAddressSimple(h);
            SKETCH_TEST_ASSERT(!v.empty());

            SketchInfo("[simple] %ws -> %ws", h, v.utf16());
        }
    }

    {
        auto my = system::machineName();
        auto v = n.hostNameToAddressSimple(my.utf16());
        SKETCH_TEST_ASSERT(!v.empty());

        SketchInfo("[simple] %ws -> %ws", my.utf16(), v.utf16());
    }

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Net, "Net::addressToHostName()")

    Net n;

    auto my = system::machineName();
    auto v = n.hostNameToAddressSimple(my.utf16());
    SKETCH_TEST_ASSERT(!v.empty());
    SketchInfo("%ws -> %ws", my.utf16(), v.utf16());

    auto a = n.addressToHostName(v.utf16());
    SKETCH_TEST_ASSERT(!a.empty());
    SketchInfo("%ws -> %ws", v.utf16(), a.utf16());

SKETCH_TEST_END()
