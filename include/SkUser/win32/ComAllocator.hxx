#pragma once

#include "../SkUser.hxx"

#include <SkBase/Allocator.hxx>

#include <Objbase.h>


namespace sketch
{

namespace win32
{

// STL-style COM allocator

template <class _Ty, bool _Throwing>
class ComAllocatorBase;


template <bool _Throwing>
class ComAllocatorBase<void, _Throwing>
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<class _U> struct rebind
    {
        typedef ComAllocatorBase<_U, _Throwing> other;
    };
};


template <class _Ty, bool _Throwing>
class ComAllocatorBase
{
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef _Ty* pointer;
    typedef const _Ty* const_pointer;
    typedef _Ty& reference;
    typedef const _Ty& const_reference;
    typedef _Ty value_type;

    template<class _U>
    struct rebind
    {
        typedef ComAllocatorBase<_U, _Throwing> other;
    };

    ComAllocatorBase() noexcept
    {
    }

    ComAllocatorBase(const ComAllocatorBase &) noexcept
    {
    }

    template<class _U>
    ComAllocatorBase(const ComAllocatorBase<_U, _Throwing>&) noexcept
    {
    }

    ~ComAllocatorBase() noexcept
    {
    }

    pointer address(typename std::add_lvalue_reference<typename std::remove_const<typename std::remove_reference<reference>::type>::type>::type x) const noexcept
    {
        return &x;
    }

    const_pointer address(const_reference x) const noexcept
    {
        return &x;
    }

    __declspec(noalias) __declspec(restrict)__forceinline
    pointer allocate(size_type n, typename ComAllocatorBase<void, _Throwing>::const_pointer = 0)
    {
        auto p = reinterpret_cast<_Ty*>(::CoTaskMemAlloc(sizeof(_Ty) * n));
#ifdef SKETCH_CXX_EXCEPTIONS
        if (!p && _Throwing) throwOutOfMemoryException(__FILE__, __LINE__);
#endif

        return p;
    }

    __declspec(noalias) __forceinline
    void deallocate(pointer p, size_type n = 0) noexcept
    {
        SK_UNUSED(n);
        if (p)
        {
            ::CoTaskMemFree(const_cast<typename std::remove_const<_Ty>::type*>(p));
        }
    }

    size_type max_size() const noexcept
    {
        return size_t(-1) / sizeof(_Ty);
    }

    void destroy(const pointer p)
    {
        p->_Ty::~_Ty();
    }

    template<class _U, class... Args>
    void construct(_U* p, Args&&... args)
    {
        ::new ((void*)p) _U(std::forward<Args>(args)...);
    }
};


template <class _Ty>
using ThrowingComAllocator = ComAllocatorBase<_Ty, true>;

template <class _Ty>
struct is_nonthrowing_allocator<ThrowingComAllocator<_Ty>, _Ty>
    : public true_type
{
};


template <class _Ty>
using NonThrowingComAllocator = ComAllocatorBase<_Ty, false>;

template <class _Ty>
struct is_nonthrowing_allocator<NonThrowingComAllocator<_Ty>, _Ty>
    : public false_type
{
};


#ifdef SKETCH_CXX_EXCEPTIONS

template<class _Ty>
using ComAllocator = ThrowingComAllocator<_Ty>;

#else

template<class _Ty>
using ComAllocator = NonThrowingComAllocator<_Ty>;

#endif


template<class _Ty, class _U>
inline bool operator==(const ComAllocator<_Ty>&, const ComAllocator<_U>&) noexcept
{
    return true;
}

template<class _Ty, class _U>
inline bool operator!=(const ComAllocator<_Ty>&, const ComAllocator<_U>&) noexcept
{
    return false;
}


} // namespace win32 {}

} // namespace sketch {}


