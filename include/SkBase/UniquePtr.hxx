#pragma once

#include "Pair.hxx"
#include "type_traits.hxx"

namespace sketch
{


template<class _Ty>
struct DefaultDeleter
{
    constexpr DefaultDeleter() noexcept = default;

    template<class _Ty2, class = typename enable_if<is_convertible<_Ty2 *, _Ty *>::value, void>::type>
    inline DefaultDeleter(const DefaultDeleter<_Ty2>&) noexcept
    {
    }

    inline void operator()(_Ty *_Ptr) const noexcept
    {
        static_assert(0 < sizeof(_Ty), "can't delete an incomplete type");
        delete _Ptr;
    }
};

template<class _Ty>
struct DefaultDeleter<_Ty[]>
{
    constexpr DefaultDeleter() noexcept = default;

    template<class _Uty, class = typename enable_if<is_convertible<_Uty(*)[], _Ty(*)[]>::value, void>::type>
    inline DefaultDeleter(const DefaultDeleter<_Uty[]>&) noexcept
    {
    }

    template<class _Uty, class = typename enable_if<is_convertible<_Uty(*)[], _Ty(*)[]>::value, void>::type>
    inline void operator()(_Uty *_Ptr) const noexcept
    {
        static_assert(0 < sizeof(_Uty), "can't delete an incomplete type");
        delete[] _Ptr;
    }
};


template <class _Ty, class _D = DefaultDeleter<_Ty>>
class UniquePtr;


template <class _Ty, class _D>
class UniquePtr
{
private:
    typedef const _Ty* UniquePtr<_Ty, DefaultDeleter<_Ty>>::* unspecified_pointer_type;

public:
    typedef _Ty* pointer;
    typedef _Ty element_type;
    typedef _D deleter_type;

    inline UniquePtr() noexcept
        : m_ptr(nullptr)
    {
        static_assert(!(sketch::is_reference<_D>::value || sketch::is_pointer<_D>::value), "_D shall not be a reference type or pointer type");
    }

    inline explicit UniquePtr(pointer p) noexcept
        : m_ptr(p)
    {
        static_assert(!(sketch::is_reference<_D>::value || sketch::is_pointer<_D>::value), "_D shall not be a reference type or pointer type");
    }

    inline UniquePtr(sketch::nullptr_t) noexcept
        : m_ptr(nullptr)
    {
    }

    inline UniquePtr(
        pointer p,
        typename sketch::conditional<
            sketch::is_reference<deleter_type>::value,
            deleter_type,
            typename sketch::add_lvalue_reference<const typename sketch::remove_reference<deleter_type>::type>::type
        >::type d
    ) noexcept
        : m_ptr(p)
        , m_deleter(d)
    {
    }

    inline UniquePtr(UniquePtr&& u) noexcept
        : m_ptr(u.get())
        , m_deleter(sketch::forward<_D>(u.m_deleter))
    {
        u.detach();
    }

    template <class _U, class _E>
    inline UniquePtr(UniquePtr<_U, _E>&& u) noexcept
        : m_ptr(u.get())
        , m_deleter(sketch::forward<_D>(u.get_deleter()))
    {
        static_assert(!sketch::is_array<_U>::value, "conversion among array and non-array forms are disallowed");
        static_assert((!sketch::is_reference<deleter_type>::value || sketch::is_same<deleter_type, _E>::value), "If _D is a reference type, then _E shall be the same type as _D");
        u.detach();
    }

    inline ~UniquePtr() noexcept
    {
        if (get())
        {
            get_deleter()(get());
        }
    }

    inline UniquePtr& operator=(UniquePtr&& u) noexcept
    {
        reset(u.detach());
        m_deleter = sketch::move(u.m_deleter);
        return *this;
    }

    template <class _U, class _E>
    inline UniquePtr& operator=(UniquePtr<_U, _E>&& u) noexcept
    {
        static_assert(!sketch::is_array<_U>::value, "conversion among array and non-array forms are disallowed");

        reset(u.detach());
        m_deleter = sketch::move(u.get_deleter());
        return *this;
    }

    inline UniquePtr& operator=(unspecified_pointer_type*)
    {
        reset();
        return *this;
    }

    inline typename sketch::add_lvalue_reference<_Ty>::type operator*() const noexcept
    {
        return *m_ptr;
    }

    __forceinline pointer operator->() const noexcept
    {
        return m_ptr;
    }

    __forceinline pointer get() const noexcept
    {
        return m_ptr;
    }

    inline typename sketch::add_lvalue_reference<deleter_type>::type get_deleter() noexcept
    {
        return m_deleter;
    }

    inline typename sketch::add_lvalue_reference<const typename sketch::remove_reference<deleter_type>::type>::type get_deleter() const noexcept
    {
        return m_deleter;
    }

    inline explicit operator bool() const noexcept
    {
        return (m_ptr != nullptr);
    }

    inline pointer detach() const
    {
        pointer tmp = nullptr;
        sketch::swap(m_ptr, tmp);
        return tmp;
    }

    inline void reset(pointer p = nullptr) noexcept
    {
        if (get() && get() != p)
        {
            get_deleter()(get());
        }

        set(p);
    }

    inline void swap(UniquePtr& u) noexcept
    {
        using sketch::swap;
        swap(m_ptr, u.m_ptr);
        swap(m_deleter, u.m_deleter);
    }

private:
    mutable pointer m_ptr;

    typename sketch::conditional<sketch::is_const<deleter_type>::value, deleter_type, mutable deleter_type>::type m_deleter;

    inline void set(_Ty* p)
    {
        m_ptr = p;
    }

    template<class _U>
    void reset(_U) = delete;
};


template <class _Ty, class _D>
class UniquePtr<_Ty[], _D>
{
private:
    typedef const _Ty* UniquePtr<_Ty, DefaultDeleter<_Ty>>::* unspecified_pointer_type;

public:
    typedef _Ty* pointer;
    typedef _Ty element_type;
    typedef _D deleter_type;

    inline UniquePtr() noexcept
        : m_ptr(nullptr)
    {
        static_assert(!(sketch::is_reference<_D>::value || sketch::is_pointer<_D>::value), "_D shall not be a reference type or pointer type");
    }

    inline explicit UniquePtr(pointer p) noexcept
        : m_ptr(p)
    {
        static_assert(!(sketch::is_reference<_D>::value || sketch::is_pointer<_D>::value), "_D shall not be a reference type or pointer type");
    }

    inline UniquePtr(sketch::nullptr_t) noexcept
        : m_ptr(nullptr)
    {
    }

    inline UniquePtr(
        pointer p,
        typename sketch::conditional<sketch::is_reference<deleter_type>::value, deleter_type, typename sketch::add_lvalue_reference<const typename sketch::remove_reference<deleter_type>::type>::type>::type d
    ) noexcept
        : m_ptr(p)
        , m_deleter(d)
    {
    }

    inline UniquePtr(UniquePtr&& u) noexcept
        : m_ptr(u.get())
        , m_deleter(sketch::forward<_D>(u.m_deleter))
    {
        u.detach();
    }

    template <class U, class E>
    inline UniquePtr(UniquePtr<U, E>&& u) noexcept
        : m_ptr(u.get())
        , m_deleter(sketch::forward<_D>(u.get_deleter()))
    {
        static_assert(sketch::is_array<U>::value == true, "conversion among array and non-array forms are disallowed");
        static_assert((!sketch::is_reference<deleter_type>::value || sketch::is_same<deleter_type, E>::value), "If _D is a reference type, then E shall be the same type as _D");
        u.detach();
    }

    inline ~UniquePtr() noexcept
    {
        if (get())
        {
            get_deleter()(get());
        }
    }

    inline UniquePtr& operator=(UniquePtr&& u) noexcept
    {
        reset(u.detach());
        m_deleter = sketch::move(u.m_deleter);
        return *this;
    }

    inline UniquePtr& operator=(unspecified_pointer_type *)
    {
        reset();
        return *this;
    }

    inline pointer operator[](size_t i) const noexcept
    {
        return &m_ptr[i];
    }

    inline pointer get() const noexcept
    {
        return m_ptr;
    }

    inline typename sketch::add_lvalue_reference<deleter_type>::type get_deleter() noexcept
    {
        return m_deleter;
    }

    inline typename sketch::add_lvalue_reference<const typename sketch::remove_reference<deleter_type>::type>::type get_deleter() const noexcept
    {
        return m_deleter;
    }

    inline explicit operator bool() const noexcept
    {
        return (m_ptr != nullptr);
    }

    inline pointer detach() const
    {
        pointer tmp = nullptr;
        sketch::swap(tmp, m_ptr);
        return tmp;
    }

    inline void reset(pointer p = 0) const  noexcept
    {
        if (get() && get() != p)
        {
            get_deleter()(get());
        }

        set(p);
    }

    inline void swap(UniquePtr& u) const  noexcept
    {
        using sketch::swap;
        swap(m_ptr, u.m_ptr);
        swap(m_deleter, u.deleter);
    }

private:
    mutable pointer m_ptr;
    typename sketch::conditional<sketch::is_const<deleter_type>::value, deleter_type, mutable deleter_type>::type m_deleter;

    inline void set(_Ty* p) const
    {
        m_ptr = p;
    }

    template<class Other>
    UniquePtr(Other*) = delete;

    template<class Other>
    void reset(Other*) const = delete;
};



} // namespace sketch {}
