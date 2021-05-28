#pragma once

#include "../Sketch.hxx"


namespace sketch
{

template <class _A, class _Ty>
struct is_nonthrowing_allocator
    : public false_type
{
};

} // namespace sketch {}


#if defined(SKETCH_WIN_KM)
#include "nt/Allocator_nt.hxx"
#endif


namespace sketch
{

#if defined(SKETCH_UM)

template <class _Ty, bool _Throwing>
class AllocatorBase;


template <bool _Throwing>
class AllocatorBase<void, _Throwing>
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<class _U> struct rebind
    {
        typedef AllocatorBase<_U, _Throwing> other;
    };
};


template <class _Ty, bool _Throwing>
class AllocatorBase
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
        typedef AllocatorBase<_U, _Throwing> other;
    };

    AllocatorBase() noexcept
    {
    }

    AllocatorBase(const AllocatorBase &) noexcept
    {
    }

    template<class _U>
    AllocatorBase(const AllocatorBase<_U, _Throwing>&) noexcept
    {
    }

    ~AllocatorBase() noexcept
    {
    }

    pointer address(typename sketch::add_lvalue_reference<typename sketch::remove_const<typename sketch::remove_reference<reference>::type>::type>::type x) const noexcept
    {
        return &x;
    }

    const_pointer address(const_reference x) const noexcept
    {
        return &x;
    }

    __declspec(noalias) __declspec(restrict) __forceinline
    pointer allocate(size_type n, typename AllocatorBase<void, _Throwing>::const_pointer = 0)
    {
        auto p = static_cast<pointer>(SkMalloc(sizeof(_Ty) * n));
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
            SkFree(p);
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
        ::new ((void*)p) _U(sketch::forward<Args>(args)...);
    }
};


template <class _Ty>
using ThrowingAllocator = AllocatorBase<_Ty, true>;

template <class _Ty>
struct is_nonthrowing_allocator<ThrowingAllocator<_Ty>, _Ty>
    : public true_type
{
};


template <class _Ty>
using NonThrowingAllocator = AllocatorBase<_Ty, false>;

template <class _Ty>
struct is_nonthrowing_allocator<NonThrowingAllocator<_Ty>, _Ty>
    : public false_type
{
};


#ifdef SKETCH_CXX_EXCEPTIONS

template<class _Ty>
using Allocator = ThrowingAllocator<_Ty>;

#else

template<class _Ty>
using Allocator = NonThrowingAllocator<_Ty>;

#endif


#elif defined(SKETCH_WIN_KM)

template<class _Ty>
using Allocator = sketch::nt::nonpaged_allocator<_Ty>;

#endif


template<class _Ty, class _U>
inline bool operator==(const Allocator<_Ty>&, const Allocator<_U>&) noexcept
{
    return true;
}

template<class _Ty, class _U>
inline bool operator!=(const Allocator<_Ty>&, const Allocator<_U>&) noexcept
{
    return false;
}


} // namespace sketch {}
