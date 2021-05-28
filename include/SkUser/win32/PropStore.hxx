#pragma once

#include <SkBase/nt/ComPtr.hxx>
#include <SkBase/Vector.hxx>

#include "PropVariant.hxx"

#include <propsys.h>


namespace sketch
{

namespace win32
{



class PropertyStore
    : public ErrorState
{
public:
    ~PropertyStore() noexcept
    {
    }

    explicit PropertyStore(nt::ComPtr<IPropertyStore>& p) noexcept
        : m_p(p)
    {
    }

    Vector<PROPERTYKEY> keys()
    {
        DWORD n;
        auto hr = m_p->GetCount(&n);
        if (FAILED(hr))
        {
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
            return Vector<PROPERTYKEY>();
        }

        Vector<PROPERTYKEY> v;
        if (!v.reserve(n))
        {
            setLastError(win32::Error::make(E_OUTOFMEMORY, String(), __FILE__, __LINE__));
            return Vector<PROPERTYKEY>();
        }

        for (DWORD i = 0; i < n; i++)
        {
            PROPERTYKEY key;
            hr = m_p->GetAt(i, &key);

            if (FAILED(hr))
            {
                setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
                continue;
            }

            v.push_back(key);
        }

        return v;
    }

    bool commit()
    {
        auto hr = m_p->Commit();
        if (FAILED(hr))
        {
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
            return false;
        }

        return true;
    }

    PropVariant get(const PROPERTYKEY& key)
    {
        PropVariant v;
        auto hr = m_p->GetValue(key, &v);
        if (FAILED(hr))
        {
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
        }

        return v;
    }

    bool set(const PROPERTYKEY& key, const PropVariant& v)
    {
        auto hr = m_p->SetValue(key, v);

        if (FAILED(hr))
        {
            setLastError(win32::Error::make(hr, String(), __FILE__, __LINE__));
            return false;
        }

        return true;
    }

private:
    nt::ComPtr<IPropertyStore> m_p;
};



} // namespace win32 {}

} // namespace sketch {}
