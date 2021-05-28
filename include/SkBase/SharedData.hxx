#pragma once

#include "RefCount.hxx"


namespace sketch
{

namespace internal
{

constexpr inline uint32_t nextPowerOfTwo(uint32_t v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

constexpr inline uint64_t nextPowerOfTwo(uint64_t v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    ++v;
    return v;
}

constexpr inline uint32_t roundUpPage(uint32_t x)
{
    return (x + 4096 - 1) & (~(4096 - 1));
}

constexpr inline uint64_t roundUpPage(uint64_t x)
{
    return (x + 4096 - 1) & (~(4096 - 1));
}


template <typename _Ty, class _Allocator>
struct SharedData
{
    typedef _Allocator allocator_type;

    refcount::type ref;
    size_t length; // for strings, this includes '\0'
    size_t capacity;
    size_t offset;
    
    inline constexpr _Ty* data() noexcept
    {
        return offset_pointer<_Ty>(this, offset);
    }

    inline constexpr const _Ty* data() const noexcept
    {
        return offset_pointer<_Ty>(this, offset);
    }

    inline constexpr bool writeable() const noexcept
    {
        return (capacity != 0);
    }

    static SharedData* allocate(size_t capacity, size_t alignment)
    {
        if (alignment < alignof(SharedData)) alignment = alignof(SharedData);
        SK_ASSERT(!(alignment & (alignment - 1)));

        size_t headerSize = sizeof(SharedData) + (alignment - alignof(SharedData));

        auto required = (capacity * sizeof(_Ty)) + headerSize;

        allocator_type::rebind<uint8_t>::other a;
        auto p = static_cast<SharedData*>(static_cast<void*>(a.allocate(required)));
        if (!p)
        {
            return nullptr;
        }

        p->ref = 1;
        p->length = 0;
        p->capacity = capacity;

        uintptr_t data = (uintptr_t(p) + sizeof(SharedData) + alignment - 1) & ~(alignment - 1);

        p->offset = data - uintptr_t(p);

        return p;
    }

    static void deallocate(SharedData* p) noexcept
    {
        allocator_type::rebind<uint8_t>::other a;
        a.deallocate(static_cast<uint8_t*>(static_cast<void*>(p)));
    }

    static size_t grow(size_t current, size_t alignment)
    {
        if (alignment < alignof(SharedData)) alignment = alignof(SharedData);
        SK_ASSERT(!(alignment & (alignment - 1)));

        size_t headerSize = sizeof(SharedData) + (alignment - alignof(SharedData));

        auto existing = (current * sizeof(_Ty)) + headerSize;
        size_t required = 0;

        if (existing < 4084)
        {
            required = nextPowerOfTwo(existing);
            if (required - 12 <= existing) required = nextPowerOfTwo(required); 
            required -= 12; // some allocators perform worst when requested exact powers of two
            SK_ASSERT(required > existing);
        }
        else
        {
            required = roundUpPage(existing) + 4096;
        }

        size_t capacity = (required - headerSize) / sizeof(_Ty);
        return capacity;
    }

    static size_t grow(size_t current, size_t capacity, size_t alignment)
    {
        if (alignment < alignof(SharedData)) alignment = alignof(SharedData);
        SK_ASSERT(!(alignment & (alignment - 1)));

        SK_ASSERT(capacity > current);

        size_t headerSize = sizeof(SharedData) + (alignment - alignof(SharedData));

        auto required = (capacity * sizeof(_Ty)) + headerSize;
        auto existing = (current * sizeof(_Ty)) + headerSize;

        if (existing < 4084)
        {
            auto x = nextPowerOfTwo(required);
            if (x - 12 < required) x = nextPowerOfTwo(x); 
            required = x - 12; // some allocators perform worst when requested exact powers of two
            SK_ASSERT(required > existing);
        }
        else
        {
            required = sketch::max(roundUpPage(existing + 4096), roundUpPage(required));
        }
                
        capacity = (required - headerSize) / sizeof(_Ty);
        return capacity;
    }
};


template <typename _Ty, size_t _N, class _Allocator>
struct StaticSharedData
{
    SharedData<_Ty, _Allocator> header;
    _Ty data[_N];
};


} // namespace internal {}

} // namespace sketch {}
