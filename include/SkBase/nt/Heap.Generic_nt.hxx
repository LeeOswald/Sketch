#pragma once


namespace sketch
{

namespace nt
{


template <POOL_TYPE _PoolType>
class GenericHeap
    : public NonCopyable
{
public:
    __forceinline ~GenericHeap() noexcept
    {
    }

    __forceinline GenericHeap() noexcept
    {
    }

    inline static void* allocate(size_t size) noexcept
    {
        SK_ASSERT(size);
        return ExAllocatePoolWithTag(_PoolType, size, 'hGkS');
    }

    inline static void free(void* p) noexcept
    {
        SK_ASSERT(p);
        ExFreePoolWithTag(p, 'hGkS');
    }
};


} // namespace nt {}

} // namespace sketch {}
