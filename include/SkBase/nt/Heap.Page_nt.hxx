#pragma once


namespace sketch
{

namespace nt
{

template <bool _Executable = false>
class PageHeap
    : public NonCopyable
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

        void* base = nullptr;
        SIZE_T sz = size;
        uint32_t protect = _Executable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
        auto status = ZwAllocateVirtualMemory(NtCurrentProcess(), &base, 1, &sz, MEM_COMMIT | MEM_RESERVE, protect);
        if (!NT_SUCCESS(status))
        {
            return nullptr;
        }

        return base;
    }

    static inline void free(void* p) noexcept
    {
        SK_ASSERT(p);

        SIZE_T size = 0;
        SK_VERIFY(NT_SUCCESS(ZwFreeVirtualMemory(NtCurrentProcess(), &p, &size, MEM_RELEASE)));
    }

    static inline size_t granularity() noexcept
    {
        SYSTEM_BASIC_INFORMATION si = { 0 };
        SK_VERIFY(NT_SUCCESS(ZwQuerySystemInformation(SystemBasicInformation, &si, sizeof(si), nullptr)));
        return si.AllocationGranularity;
    }
};

} // namespace nt {}

} // namespace sketch {}
