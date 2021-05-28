#pragma once

#include <SkBase/Heap.hxx>


void* operator new(size_t, void* ptr) noexcept
{
    return ptr;
}

void operator delete(void*, void*) noexcept
{
}

void* operator new[](size_t, void* ptr) noexcept
{
    return ptr;
}

void operator delete[](void*, void*) noexcept
{
}

void* operator new(size_t size, const sketch::nothrow_t&) noexcept
{
    if (!size)
    {
        size = 1;
    }

    return SkMalloc(size);
}

void  operator delete(void* ptr, const sketch::nothrow_t&) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void  operator delete(void* ptr, size_t, const sketch::nothrow_t&) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void* operator new[](size_t size, const sketch::nothrow_t&) noexcept
{
    if (!size)
    {
        size = 1;
    }

    return SkMalloc(size);
}

void  operator delete[](void* ptr, const sketch::nothrow_t&) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void  operator delete[](void* ptr, size_t, const sketch::nothrow_t&) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void* __cdecl operator new(size_t size)
{
    if (!size)
    {
        size = 1;
    }

    return SkMalloc(size);
}

void __cdecl operator delete(void* ptr) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void __cdecl operator delete(void* ptr, size_t) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void* __cdecl operator new[](size_t size)
{
    if (!size)
    {
        size = 1;
    }

    return SkMalloc(size);
}

void __cdecl operator delete[](void* ptr) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}

void __cdecl operator delete[](void* ptr, size_t) noexcept
{
    if (ptr)
    {
        SkFree(ptr);
    }
}
