#pragma once

#include "../type_traits.hxx"


namespace sketch
{

namespace nt
{

template <class _Ty, POOL_TYPE _PoolType>
class Allocator;

template <POOL_TYPE _PoolType>
class Allocator<void, _PoolType>
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<class _U> struct rebind
    {
        typedef Allocator<_U, _PoolType> other;
    };
};

template <class _Ty, POOL_TYPE _PoolType>
class Allocator
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
        typedef Allocator<_U, _PoolType> other;
    };

    Allocator() noexcept
    {
    }

    Allocator(const Allocator &) noexcept
    {
    }

    template<class _U>
    Allocator(const Allocator<_U, _PoolType>&) noexcept
    {
    }

    ~Allocator() noexcept
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
    pointer allocate(size_type n, typename Allocator<void, _PoolType>::const_pointer = 0) noexcept
    {
        // TODO: replace with custom memory manager
        return static_cast<pointer>(ExAllocatePoolWithTag(_PoolType, sizeof(_Ty) * n, 'tekS'));
    }

    __declspec(noalias) __forceinline
    void deallocate(pointer p, size_type n = 0) noexcept
    {
        SK_UNUSED(n);

        if (p)
        {
            ExFreePool(const_cast<typename sketch::remove_const<_Ty>::type*>(p));
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


template<class _Ty, class _U, POOL_TYPE _PoolType>
inline bool operator==(const Allocator<_Ty, _PoolType>&, const Allocator<_U, _PoolType>&) noexcept
{
    return true;
}

template<class _Ty, class _U, POOL_TYPE _PoolType>
inline bool operator!=(const Allocator<_Ty, _PoolType>&, const Allocator<_U, _PoolType>&) noexcept
{
    return false;
}


template <class _Ty>
using paged_allocator = Allocator<_Ty, PagedPool>;

template <class _T>
struct is_nonthrowing_allocator<paged_allocator<_T>, _T>
    : public true_type
{
};


template <class _Ty>
using nonpaged_allocator = Allocator<_Ty, NonPagedPool>;

template <class _T>
struct is_nonthrowing_allocator<nonpaged_allocator<_T>, _T>
    : public true_type
{
};



} // namespace nt {}

} // namespace sketch {}
