#pragma once

#include "../../Sketch.hxx"

#ifdef SKETCH_WIN_UM
#include <Unknwn.h>
#else
#include <unknown.h>
#endif



namespace sketch
{

namespace nt
{


template <class T>
class ComNoReleasePtr : public T
{
private:
    virtual ULONG __stdcall AddRef() noexcept = 0;
    virtual ULONG __stdcall Release() noexcept = 0;
};



#pragma region ComPtrBase


template <class T>
class ComPtrBase
{
protected:
    __forceinline ComPtrBase() noexcept
        : m_p(nullptr)
    {
    }

    __forceinline ComPtrBase(T* p) noexcept
    {
        m_p = p;
        if (p)
        {
            p->AddRef();
        }
    }

    static IUnknown* _ptrQiAssign(IUnknown** pp, IUnknown* lp, REFIID riid) noexcept
    {
        if (!pp)
        {
            return nullptr;
        }

        IUnknown* pTemp = *pp;
#ifdef SKETCH_WIN_KM
        if (!lp || !NT_SUCCESS(lp->QueryInterface(riid, reinterpret_cast<void**>(pp))))
#else
        if (!lp || FAILED(lp->QueryInterface(riid, reinterpret_cast<void**>(pp))))
#endif
        {
            *pp = nullptr;
        }

        if (pTemp)
        {
            pTemp->Release();
        }

        return *pp;
    }

public:
    typedef T _PtrClass;

    void swap(ComPtrBase& other) noexcept
    {
        T* pTemp = m_p;
        m_p = other.m_p;
        other.m_p = pTemp;
    }

    __forceinline ~ComPtrBase() noexcept
    {
        if (m_p)
        {
            m_p->Release();
        }
    }

    __forceinline ComNoReleasePtr<T>* get() const noexcept
    {
        return (ComNoReleasePtr<T>*)m_p;
    }

    T** writeablePtr() noexcept
    {
        SK_ASSERT(!m_p);
        return &m_p;
    }

    __forceinline ComNoReleasePtr<T>* operator->() const noexcept
    {
        SK_ASSERT(m_p);

        return (ComNoReleasePtr<T>*)m_p;
    }

    __forceinline operator bool() const noexcept
    {
        return (m_p != nullptr);
    }

    __forceinline bool operator!() const noexcept
    {
        return (!m_p);
    }

    __forceinline bool operator<(T* p) const noexcept
    {
        return m_p < p;
    }

    __forceinline bool operator!=(T* p) const noexcept
    {
        return !operator==(p);
    }

    __forceinline bool operator==(T* p) const noexcept
    {
        return m_p == p;
    }

    __forceinline void release() noexcept
    {
        T* pTemp = m_p;
        if (pTemp)
        {
            m_p = nullptr;
            pTemp->Release();
        }
    }

    bool isEqualObject(IUnknown* pOther) noexcept
    {
        if (!m_p && !pOther)
        {
            return true;
        }

        if (!m_p || !pOther)
        {
            return false;
        }

        ComPtr<IUnknown> punk1;
        ComPtr<IUnknown> punk2;

        m_p->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&punk1));
        pOther->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&punk2));

        return punk1 == punk2;
    }

    void attach(T* p) noexcept
    {
        if (m_p)
        {
            m_p->Release();
        }

        m_p = p;
    }

    T* detach() noexcept
    {
        T* p = m_p;
        m_p = nullptr;
        return p;
    }

    template <class Q>
#ifdef SKETCH_WIN_KM
    NTSTATUS queryInterface(Q** pp) const noexcept
#else
    HRESULT queryInterface(Q** pp) const noexcept
#endif
    {
        SK_ASSERT(pp);

        return m_p->QueryInterface(__uuidof(Q), reinterpret_cast<void**>(pp));
    }

    template <class Q>
#ifdef SKETCH_WIN_KM
    NTSTATUS queryInterface(REFIID iid, Q** pp) const noexcept
#else
    HRESULT queryInterface(REFIID iid, Q** pp) const noexcept
#endif
    {
        SK_ASSERT(pp);

        return m_p->QueryInterface(iid, reinterpret_cast<void**>(pp));
    }

#ifdef SKETCH_WIN_UM
    HRESULT copyTo(T** pp) noexcept
    {
        SK_ASSERT(pp);

        if (!pp)
        {
            return E_POINTER;
        }

        *pp = m_p;
        if (m_p)
        {
            m_p->AddRef();
        }

        return S_OK;
    }

    HRESULT createInstance(REFCLSID rclsid, REFCLSID riid = __uuidof(T), IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL) noexcept
    {
        SK_ASSERT(!m_p);

        return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, reinterpret_cast<void**>(&m_p));
    }

    HRESULT createInstance(const OLECHAR* szProgID, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL) noexcept
    {
        CLSID clsid;
        HRESULT hr = ::CLSIDFromProgID(szProgID, &clsid);

        SK_ASSERT(!m_p);

        if (SUCCEEDED(hr))
        {
            hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), reinterpret_cast<void**>(&m_p));
        }

        return hr;
    }
#endif // SKETCH_WIN_UM

protected:
    T* m_p;
};


#pragma endregion


#pragma region ComPtr


template <class T>
class ComPtr : public ComPtrBase<T>
{
public:
    ComPtr()
    {
    }

    ComPtr(T* p)
    : ComPtrBase<T>(p)
    {
    }

    ComPtr(const ComPtr<T>& p)
    : ComPtrBase<T>(p.m_p)
    {
    }

    T* operator=(T* p)
    {
        if (m_p != p)
        {
            ComPtr(p).swap(*this);
        }

        return m_p;
    }

    template <typename Q>
    T* operator=(const ComPtr<Q>& p)
    {
        if (!isEqualObject(p))
        {
            return static_cast<T*>(_ptrQiAssign((IUnknown**)&m_p, p, __uuidof(T)));
        }

        return *this;
    }

    T* operator=(const ComPtr<T>& p)
    {
        if (m_p != p.m_p)
        {
            ComPtr(p.m_p).swap(*this);
        }

        return m_p;
    }

    ComPtr(ComPtr<T>&& p)
        : ComPtrBase<T>()
    {
        p.swap(*this);
    }

    T* operator=(ComPtr<T>&& p)
    {
        if (m_p != p.m_p)
        {
            ComPtr(sketch::move(p)).swap(*this);
        }

        return m_p;
    }
};

#pragma endregion


#pragma region ComQiPtr


template <class T, const IID* piid = &__uuidof(T)>
class ComQiPtr : public ComPtr<T>
{
public:
    ComQiPtr()
    {
    }

    ComQiPtr(decltype(nullptr))
    {
    }

    ComQiPtr(T* p)
    : ComPtr<T>(p)
    {
    }

    ComQiPtr(const ComQiPtr<T, piid>& p)
    : ComPtr<T>(p.m_p)
    {
    }

    ComQiPtr(IUnknown* p)
    {
        if (p)
        {
#ifdef SKETCH_WIN_KM
            if (!NT_SUCCESS(p->QueryInterface(*piid, reinterpret_cast<void**>(&m_p))))
#else
            if (FAILED(p->QueryInterface(*piid, reinterpret_cast<void**>(&m_p))))
#endif
            {
                p = nullptr;
            }
        }
    }

    T* operator=(decltype(nullptr))
    {
        ComQiPtr(nullptr).swap(*this);
        return nullptr;
    }

    T* operator=(T* p)
    {
        if (m_p != p)
        {
            ComQiPtr(p).swap(*this);
        }

        return *this;
    }

    T* operator=(const ComQiPtr<T, piid>& p)
    {
        if (m_p != p.m_p)
        {
            ComQiPtr(p).swap(*this);
        }

        return *this;
    }

    T* operator=(IUnknown* p)
    {
        if (m_p != p)
        {
            return static_cast<T*>(_ptrQiAssign((IUnknown**)&m_p, p, *piid));
        }

        return *this;
    }
};


template<>
class ComQiPtr<IUnknown, &IID_IUnknown> : public ComPtr<IUnknown>
{
public:
    ComQiPtr()
    {
    }

    ComQiPtr(IUnknown* p)
    {
        if (p)
        {
#ifdef SKETCH_WIN_KM
            if (!NT_SUCCESS(p->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&m_p))))
#else
            if (FAILED(p->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&m_p))))
#endif
            {
                m_p = nullptr;
            }
        }
    }

    ComQiPtr(const ComQiPtr<IUnknown, &IID_IUnknown>& p)
    : ComPtr<IUnknown>(p.m_p)
    {
    }

    IUnknown* operator=(IUnknown* p)
    {
        if (m_p != p)
        {
            return _ptrQiAssign(&m_p, p, IID_IUnknown);
        }

        return m_p;
    }

    IUnknown* operator=(const ComQiPtr<IUnknown, &IID_IUnknown>& p)
    {
        if (m_p != p.m_p)
        {
            ComQiPtr(p).swap(*this);
        }

        return m_p;
    }
};


#pragma endregion


} // namespace nt {}

} // namespace sketch {}
