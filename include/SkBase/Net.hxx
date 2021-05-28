#pragma once

#include "../Sketch.hxx"
#include "Vector.hxx"

namespace sketch
{

class Net
    : public NonCopyable
    , public ErrorState
{
public:
    struct Address
    {
        enum class Type
        {
            ip4,
            ip6,
            unknown
        };

        Type type;
        String address;
    };

    Vector<Address> hostNameToAddress(const wchar_t* host);
    String hostNameToAddressSimple(const wchar_t* host); // pick up the first IPv4 address

    String addressToHostName(const wchar_t* address);
};


} // namespace sketch {}
