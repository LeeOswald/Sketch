#pragma once

#include "../SkUser.hxx"


#include <SkBase/Pair.hxx>
#include <SkBase/UniquePtr.hxx>

#include <combaseapi.h>


namespace sketch
{

namespace win32
{



struct PropVariant
    : public PROPVARIANT
    , public ErrorState
{
    ~PropVariant() noexcept
    {
        clear();
    }

    PropVariant() noexcept
    {
        ::PropVariantInit(this);
    }

    PropVariant(const PROPVARIANT& other)
    {
        auto hr = ::PropVariantCopy(this, &other);
        if (FAILED(hr))
        {
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
        }
    }

    PropVariant(const PropVariant& other)
        : PropVariant(static_cast<const PROPVARIANT&>(other))
    {
    }

    PropVariant& operator=(const PROPVARIANT& other)
    {
        if (this != &other)
        {
            clear();

            auto hr = ::PropVariantCopy(this, &other);
            if (FAILED(hr))
            {
                setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
            }
        }

        return *this;
    }

    PropVariant& operator=(const PropVariant& other)
    {
        return operator=(static_cast<const PROPVARIANT&>(other));
    }

    bool empty() const noexcept
    {
        return (vt == VT_EMPTY);
    }

    void clear() noexcept
    {
        ::PropVariantClear(this);
    }

    template <class _Ty>
    _Ty get() const;

    template <>
    String get() const
    {
        SK_ASSERT(vt == VT_LPWSTR);
        return String(pwszVal);
    }

    template <>
    Utf8String get() const
    {
        SK_ASSERT(vt == VT_LPSTR);
        return Utf8String(pszVal);
    }

    template <>
    Pair<size_t, UniquePtr<uint8_t[]>> get() const // size:data
    {
        SK_ASSERT(vt == VT_BLOB);
        UniquePtr<uint8_t[]> v;
        v.reset(new (sketch::nothrow) uint8_t[blob.cbSize]);

        if (!v)
        {
            setLastError(win32::Error::make(E_OUTOFMEMORY, String(), __FILE__, __LINE__));
            return Pair<size_t, UniquePtr<uint8_t[]>>();
        }

        ::memcpy(v.get(), blob.pBlobData, blob.cbSize);
        return sketch::make_pair(size_t(blob.cbSize), sketch::move(v));
    }
};



} // namespace win32 {}

} // namespace sketch {}
