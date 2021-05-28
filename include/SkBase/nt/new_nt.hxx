#pragma once




namespace sketch
{

struct  nothrow_t {};

extern __declspec(selectany) const nothrow_t nothrow = {};

} // namespace sketch {}


void* operator new(size_t, void* ptr) noexcept;
void operator delete(void*, void*) noexcept;

void* operator new[](size_t, void* ptr) noexcept;
void operator delete[](void*, void*) noexcept;

void* operator new(size_t size, const sketch::nothrow_t&) noexcept;
void  operator delete(void* ptr, const sketch::nothrow_t&) noexcept;
void  operator delete(void* ptr, size_t, const sketch::nothrow_t&) noexcept;

void* operator new[](size_t size, const sketch::nothrow_t&) noexcept;
void  operator delete[](void* ptr, const sketch::nothrow_t&) noexcept;
void  operator delete[](void* ptr, size_t, const sketch::nothrow_t&) noexcept;

void* __cdecl operator new(size_t size);
void __cdecl operator delete(void* ptr) noexcept;
void __cdecl operator delete(void* ptr, size_t) noexcept;

void* __cdecl operator new[](size_t size);
void __cdecl operator delete[](void* ptr) noexcept;
void __cdecl operator delete[](void* ptr, size_t) noexcept;
