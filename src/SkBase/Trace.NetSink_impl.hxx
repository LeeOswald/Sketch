#pragma once

#include <SkBase/IoRequest.hxx>
#include <SkBase/Trace.NetSink.hxx>
#include <SkBase/UniquePtr.hxx>

namespace sketch
{

namespace log
{

namespace internal
{


struct Header
{
    uint32_t size; // record size in bytes, w/out this header

                   // uint8_t data[size];
};

char* _packRecord(Record::Ref r) noexcept;

template <class _R, class... Args>
typename IoRequest<_R>::Ref packRecord(Record::Ref r, Args&&... args) noexcept
{
    struct Deleter
    {
        constexpr Deleter() noexcept = default;
        inline Deleter(const Deleter&) noexcept { }

        inline void operator()(char* p) const noexcept
        {
            SkFree(p);
        }
    };

    using IoRequestT = IoRequest<_R>;
    IoRequestT::Ref result;

    UniquePtr<char, Deleter> d(_packRecord(r));
    if (d)
    {
        auto l = strlen(d.get()) + 1;
        auto required = sizeof(Header) + l;

        result = sketch::move(IoRequestT::make(IoRequestT::write, nullptr, nullptr, required, sketch::forward<Args>(args)...));
        if (result)
        {
            auto hdr = reinterpret_cast<Header*>(result->data());
            hdr->size = uint32_t(l);
            auto msg = reinterpret_cast<char*>(hdr + 1);
            memcpy(msg, d.get(), l);
        }
    }

    return result;
}


} // namespace internal {}

} // namespace log {}

} // namespace sketch {}
