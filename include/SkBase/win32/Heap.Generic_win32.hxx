#pragma once


namespace sketch
{

namespace win32
{


class GenericHeap
    : private NonCopyable
{
public:
    inline ~GenericHeap() noexcept
    {
    }

    inline GenericHeap() noexcept
        : m_heap(::GetProcessHeap())
    {
        SK_ASSERT(m_heap);
    }

    inline void* allocate(size_t size) noexcept
    {
        SK_ASSERT(size);
        return ::HeapAlloc(m_heap, 0, size);
    }

    inline void free(void* p) noexcept
    {
        SK_ASSERT(p);
        SK_VERIFY(::HeapFree(m_heap, 0, p));
    }

private:
    HANDLE m_heap;
};


} // namespace win32 {}

} // namespace sketch {}
