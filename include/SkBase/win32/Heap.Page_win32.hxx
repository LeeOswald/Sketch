#pragma once


namespace sketch
{

namespace win32
{

template <bool _Executable = false>
class PageHeap
    : private NonCopyable
{
public:
    inline ~PageHeap() noexcept
    {
    }

    inline PageHeap() noexcept
    {
    }

    static inline void* allocate(size_t size) noexcept
    {
        SK_ASSERT(size);
        uint32_t protect = _Executable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
        return ::VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, protect);
    }

    static inline void free(void* p) noexcept
    {
        SK_ASSERT(p);
        SK_VERIFY(::VirtualFree(p, 0, MEM_RELEASE));
    }

    static inline size_t granularity() noexcept
    {
        SYSTEM_INFO si = { 0 };
        ::GetSystemInfo(&si);
        return si.dwAllocationGranularity;
    }
};

} // namespace win32 {}

} // namespace sketch {}
