#pragma once

#include "../../Sketch.hxx"
#include "../RefCount.hxx"

#ifdef SKETCH_WIN_UM
#include <Unknwn.h>
#else
#include <unknown.h>
#endif



namespace sketch
{

namespace nt
{


struct __declspec(novtable) INonDelegatingUnknown
{
#ifdef SKETCH_WIN_KM
    virtual NTSTATUS __stdcall NonDelegatingQueryInterface(REFIID iid, void** pp) = 0;
#else
    virtual HRESULT __stdcall NonDelegatingQueryInterface(REFIID iid, void** pp) = 0;
#endif
    virtual ULONG __stdcall NonDelegatingAddRef() = 0;
    virtual ULONG __stdcall NonDelegatingRelease() = 0;
};



class __declspec(novtable) ComObjRoot
    : public NonCopyable
{
protected:
    virtual ULONG _AddRef() = 0;
    virtual ULONG _Release() = 0;

    virtual ~ComObjRoot() noexcept
    {
    }

    ComObjRoot() noexcept
        : m_refCount(0)
    {
    }

protected:
    mutable refcount::type m_refCount;
};



template <class _Interface>
class __declspec(novtable) ComInterfaceImpl
    : virtual public ComObjRoot
    , public _Interface
{
public:
    ULONG __stdcall AddRef() override
    {
        return _AddRef();
    }

    ULONG __stdcall Release() override
    {
        return _Release();
    }

#ifdef SKETCH_WIN_KM
    NTSTATUS __stdcall QueryInterface(REFIID iid, void** pp) = 0;
#else
    HRESULT __stdcall QueryInterface(REFIID iid, void** pp) = 0;
#endif
};


template <class _Ty>
class __declspec(novtable) ComClass
    : virtual public ComObjRoot
    , public INonDelegatingUnknown
{
protected:
    virtual ~ComClass() noexcept
    {
    }

    ComClass(IUnknown* outer = nullptr) noexcept
        : m_outer(outer)
    {
        if (!m_outer)
        {
            m_outer = reinterpret_cast<IUnknown*>(static_cast<INonDelegatingUnknown*>(this));
        }
    }

private:
    ULONG _AddRef() noexcept override
    {
        refcount::increment(&m_refCount);
        return static_cast<ULONG>(m_refCount);
    }

    ULONG _Release() noexcept override
    {
        SK_ASSERT(m_refCount > 0);

        if (!refcount::decrement(&m_refCount))
        {
            delete this;
            return 0;
        }

        return static_cast<ULONG>(m_refCount);
    }

#ifdef SKETCH_WIN_KM
    NTSTATUS __stdcall NonDelegatingQueryInterface(REFIID iid, void** pp) noexcept override
#else
    HRESULT __stdcall NonDelegatingQueryInterface(REFIID iid, void** pp) noexcept override
#endif
    {
        return m_outer->QueryInterface(iid, pp);
    }

    ULONG __stdcall NonDelegatingAddRef() override
    {
        return m_outer->AddRef();
    }

    ULONG __stdcall NonDelegatingRelease() override
    {
        return m_outer->Release();
    }

private:
    IUnknown* m_outer;
};


} // namespace nt {}

} // namespace sketch {}
