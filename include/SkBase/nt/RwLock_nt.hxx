#pragma once


namespace sketch
{

namespace internal
{

class RwLockImpl
    : public NonCopyable
{
public:
    inline ~RwLockImpl() noexcept
    {
        ExDeleteResourceLite(&m_resource);
    }

    inline RwLockImpl() noexcept
    {
        ExInitializeResourceLite(&m_resource);
    }

    inline void acquireExclusive() noexcept
    {
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(&m_resource, TRUE);
    }

    inline void acquireShared() noexcept
    {
        KeEnterCriticalRegion();
        ExAcquireResourceSharedLite(&m_resource, TRUE);
    }

    inline void releaseExclusive() noexcept
    {
        ExReleaseResourceLite(&m_resource);
        KeLeaveCriticalRegion();
    }

    inline void releaseShared() noexcept
    {
        ExReleaseResourceLite(&m_resource);
        KeLeaveCriticalRegion();
    }

private:
    ERESOURCE m_resource;
};

} // namespace internal {}

} // namespace sketch {}
