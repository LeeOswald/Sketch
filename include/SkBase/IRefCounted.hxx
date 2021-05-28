#pragma once


#include "RefCount.hxx"


namespace sketch
{


struct __declspec(novtable) IRefCounted
{
    virtual void addRef() const noexcept = 0;
    virtual bool release() const noexcept = 0;

protected:
    virtual ~IRefCounted() noexcept { }
};


template <class _Itf>
class __declspec(novtable) RefCountedBase
    : public _Itf
    , public NonCopyable
{
protected:
    virtual ~RefCountedBase() noexcept
    {
    }

    RefCountedBase() noexcept
        : m_refCount(0)
    {
    }

public:
    void addRef() const noexcept override
    {
        refcount::increment(&m_refCount);
    }

    bool release() const noexcept override
    {
        SK_ASSERT(m_refCount > 0);

        if (!refcount::decrement(&m_refCount))
        {
            delete this;
            return false;
        }

        return true;
    }

private:
    mutable refcount::type m_refCount;
};


template <class T>
class RefCountedNoReleasePtr
    : public T
{
private:
    virtual void addRef() const noexcept = 0;
    virtual bool release() const noexcept = 0;
};


template <class T>
class RefCountedPtr
{
public:
    ~RefCountedPtr() noexcept
    {
        if (m_p)
        {
            m_p->release();
        }
    }

    RefCountedPtr() noexcept
        : m_p(nullptr)
    {
    }

    RefCountedPtr(T* p) noexcept
    {
        m_p = p;
        if (p)
        {
            p->addRef();
        }
    }

    RefCountedPtr(const RefCountedPtr<T>& p) noexcept
        : RefCountedPtr<T>(p.m_p)
    {
    }

    RefCountedPtr(RefCountedPtr<T>&& p) noexcept
        : RefCountedPtr<T>()
    {
        p.swap(*this);
    }

    T* operator=(T* p) noexcept
    {
        if (m_p != p)
        {
            RefCountedPtr(p).swap(*this);
        }

        return *this;
    }

    T* operator=(const RefCountedPtr<T>& p) noexcept
    {
        if (m_p != p.m_p)
        {
            RefCountedPtr(p.m_p).swap(*this);
        }

        return m_p;
    }

    T* operator=(RefCountedPtr<T>&& p) noexcept
    {
        if (m_p != p.m_p)
        {
            RefCountedPtr(static_cast<RefCountedPtr&&>(p)).swap(*this);
        }

        return m_p;
    }

    void swap(RefCountedPtr& other) noexcept
    {
        T* pTemp = m_p;
        m_p = other.m_p;
        other.m_p = pTemp;
    }

    RefCountedNoReleasePtr<T>* get() const noexcept
    {
        return m_p;
    }

    T** writeablePtr() noexcept
    {
        SK_ASSERT(!m_p);
        return &m_p;
    }

    RefCountedNoReleasePtr<T>* operator->() const noexcept
    {
        SK_ASSERT(m_p);

        return static_cast<RefCountedNoReleasePtr<T>*>(m_p);
    }

    operator bool() const noexcept
    {
        return (m_p != nullptr);
    }

    bool operator!() const noexcept
    {
        return (!m_p);
    }

    bool operator<(T* p) const noexcept
    {
        return m_p < p;
    }

    bool operator!=(T* p) const noexcept
    {
        return !operator==(p);
    }

    bool operator==(T* p) const noexcept
    {
        return m_p == p;
    }

    void release() noexcept
    {
        T* pTemp = m_p;
        if (pTemp)
        {
            m_p = nullptr;
            pTemp->release();
        }
    }

    void attach(T* p) noexcept
    {
        if (m_p)
        {
            m_p->release();
        }

        m_p = p;
    }

    T* detach() noexcept
    {
        T* p = m_p;
        m_p = nullptr;
        return p;
    }

protected:
    T* m_p;
};


} // namespace sketch {}
