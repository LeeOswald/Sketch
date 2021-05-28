#pragma once


#include "Iterator.hxx"


namespace sketch
{

namespace internal
{


namespace _CircularBuffer_details
{

template<class _ForwardIterator, class _Diff, class _Ty, class _Allocator>
void uninitialized_fill_n_with_alloc(_ForwardIterator first, _Diff n, const _Ty& item, _Allocator& alloc);

template<class _ValueType, class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_copy(_InputIterator first, _InputIterator last, _ForwardIterator dest);

template<class _ValueType, class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move_if_noexcept(_InputIterator first, _InputIterator last, _ForwardIterator dest);


template<class _ValueType, class _InputIterator, class _ForwardIterator>
inline _ForwardIterator uninitialized_copy(_InputIterator first, _InputIterator last, _ForwardIterator dest)
{
    typedef _ValueType value_type;

    _ForwardIterator next = dest;

    for (; first != last; ++first, ++dest)
    {
        ::new (dest)value_type(*first);
    }

    return dest;
}

template<class _ValueType, class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move_if_noexcept_impl(_InputIterator first, _InputIterator last, _ForwardIterator dest, sketch::true_type)
{
    for (; first != last; ++first, ++dest)
    {
        ::new (dest)_ValueType(sketch::move(*first));
    }

    return dest;
}

template<class _ValueType, class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move_if_noexcept_impl(_InputIterator first, _InputIterator last, _ForwardIterator dest, sketch::false_type)
{
    return uninitialized_copy<_ValueType>(first, last, dest);
}

template<class _ValueType, class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move_if_noexcept(_InputIterator first, _InputIterator last, _ForwardIterator dest)
{
    typedef typename sketch::is_nothrow_move_constructible<_ValueType>::type tag_t;
    return uninitialized_move_if_noexcept_impl<_ValueType>(first, last, dest, tag_t());
}

template<class _ForwardIterator, class _Diff, class _Ty, class _Allocator>
inline void uninitialized_fill_n_with_alloc(_ForwardIterator first, _Diff n, const _Ty& item, _Allocator& alloc)
{
    _ForwardIterator next = first;

    for (; n > 0; ++first, --n)
    {
        alloc.construct(first, item);
    }
}


template <class _Value, class _Allocator>
struct assign_n
{
    typedef typename _Allocator::size_type size_type;

    size_type m_n;
    _Value m_item;
    _Allocator& m_alloc;

    assign_n(size_type n, _Value item, _Allocator& alloc)
        : m_n(n)
        , m_item(item)
        , m_alloc(alloc)
    {
    }

    template <class _Pointer>
    void operator()(_Pointer p) const
    {
        uninitialized_fill_n_with_alloc(p, m_n, m_item, m_alloc);
    }

private:
    assign_n<_Value, _Allocator>& operator=(const assign_n<_Value, _Allocator>&) = delete;
};


template <class _ValueType, class _Iterator>
struct assign_range
{
    _Iterator m_first;
    _Iterator m_last;

    assign_range(const _Iterator& first, const _Iterator& last) throw()
        : m_first(first)
        , m_last(last)
    {
    }

    template <class _Pointer>
    void operator()(_Pointer p) const
    {
        uninitialized_copy<_ValueType>(m_first, m_last, p);
    }
};

template <class _ValueType, class _Iterator>
inline assign_range<_ValueType, _Iterator> make_assign_range(const _Iterator& first, const _Iterator& last)
{
    return assign_range<_ValueType, _Iterator>(first, last);
}


template <class _Traits>
struct nonconst_traits;


template <class _Traits>
struct const_traits
{
    typedef typename _Traits::value_type value_type;
    typedef typename _Traits::const_pointer pointer;
    typedef typename _Traits::const_reference reference;
    typedef typename _Traits::size_type size_type;
    typedef typename _Traits::difference_type difference_type;

    typedef nonconst_traits<_Traits> nonconst_self;
};

template <class _Traits>
struct nonconst_traits
{
    typedef typename _Traits::value_type value_type;
    typedef typename _Traits::pointer pointer;
    typedef typename _Traits::reference reference;
    typedef typename _Traits::size_type size_type;
    typedef typename _Traits::difference_type difference_type;

    typedef nonconst_traits<_Traits> nonconst_self;
};


template <class _Iterator>
struct iterator_wrapper
{
    mutable _Iterator m_iterator;

    explicit iterator_wrapper(_Iterator it)
        : m_iterator(it)
    {
    }

    _Iterator operator()() const
    {
        return m_iterator++;
    }

private:
    iterator_wrapper<_Iterator>& operator=(const iterator_wrapper<_Iterator>&) = delete;
};


template <class _Pointer, class _Value>
struct item_wrapper
{
    _Value m_item;

    explicit item_wrapper(_Value item)
        : m_item(item)
    {
    }

    _Pointer operator()() const
    {
        return &m_item;
    }

private:
    item_wrapper<_Pointer, _Value>& operator=(const item_wrapper<_Pointer, _Value>&) = delete;
};


template <class _Buffer, class _Traits>
struct iterator :
    public sketch::Iterator<
    sketch::random_access_iterator_tag,
    typename _Traits::value_type,
    typename _Traits::difference_type,
    typename _Traits::pointer,
    typename _Traits::reference
    >
{
    typedef sketch::Iterator<
        sketch::random_access_iterator_tag,
        typename _Traits::value_type,
        typename _Traits::difference_type,
        typename _Traits::pointer,
        typename _Traits::reference
    > base_iterator;

    typedef iterator<_Buffer, typename _Traits::nonconst_self> nonconst_self;

    typedef typename base_iterator::value_type value_type;
    typedef typename base_iterator::pointer pointer;
    typedef typename base_iterator::reference reference;
    typedef typename _Traits::size_type size_type;
    typedef typename base_iterator::difference_type difference_type;

    iterator()
        : m_buffer(nullptr)
        , m_iterator(nullptr)
    {
    }

    iterator(const nonconst_self& it)
        : m_buffer(it.m_buffer)
        , m_iterator(it.m_iterator)
    {
    }

    iterator(const _Buffer* cb, const pointer p)
        : m_buffer(cb)
        , m_iterator(p)
    {
    }

    iterator& operator=(const iterator& it)
    {
        if (this != &it)
        {
            m_buffer = it.m_buffer;
            m_iterator = it.m_iterator;
        }

        return *this;
    }

    reference operator*() const
    {
        SK_ASSERT(m_iterator);
        return *m_iterator;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    template <class _Traits0>
    difference_type operator-(const iterator<_Buffer, _Traits0>& it) const
    {
        return linearize_pointer(*this) - linearize_pointer(it);
    }

    iterator& operator++()
    {
        SK_ASSERT(m_iterator);

        m_buffer->increment(m_iterator);

        if (m_iterator == m_buffer->m_last)
        {
            m_iterator = nullptr;
        }

        return *this;
    }

    iterator operator++(int)
    {
        iterator<_Buffer, _Traits> tmp = *this;
        ++*this;
        return tmp;
    }

    iterator& operator--()
    {
        SK_ASSERT(m_iterator != m_buffer->m_first); // check for iterator pointing to begin()
        if (m_iterator == 0)
        {
            m_iterator = m_buffer->m_last;
        }
        m_buffer->decrement(m_iterator);
        return *this;
    }

    iterator operator--(int)
    {
        iterator<_Buffer, _Traits> tmp = *this;
        --*this;
        return tmp;
    }

    iterator& operator+=(difference_type n)
    {
        if (n > 0)
        {
            SK_ASSERT(m_buffer->end() - *this >= n); // check for too large n
            m_iterator = m_buffer->add(m_iterator, n);
            if (m_iterator == m_buffer->m_last)
            {
                m_iterator = 0;
            }
        }
        else if (n < 0)
        {
            *this -= -n;
        }

        return *this;
    }

    iterator operator+(difference_type n) const
    {
        return (iterator<_Buffer, _Traits>(*this) += n);
    }

    iterator& operator-=(difference_type n)
    {
        if (n > 0)
        {
            SK_ASSERT(*this - m_buffer->begin() >= n); // check for too large n
            m_iterator = m_buffer->sub(m_iterator == 0 ? m_buffer->m_last : m_iterator, n);
        }
        else if (n < 0)
        {
            *this += -n;
        }

        return *this;
    }

    iterator operator-(difference_type n) const
    {
        return (iterator<_Buffer, _Traits>(*this) -= n);
    }

    reference operator[](difference_type n) const
    {
        return *(*this + n);
    }

    template <class _Traits0>
    bool operator==(const iterator<_Buffer, _Traits0>& it) const
    {
        return m_iterator == it.m_iterator;
    }

    template <class _Traits0>
    bool operator!=(const iterator<_Buffer, _Traits0>& it) const
    {
        return m_iterator != it.m_iterator;
    }

    template <class _Traits0>
    bool operator<(const iterator<_Buffer, _Traits0>& it) const
    {
        return linearize_pointer(*this) < linearize_pointer(it);
    }

    template <class _Traits0>
    bool operator>(const iterator<_Buffer, _Traits0>& it) const
    {
        return it < *this;
    }

    //! Less or equal.
    template <class _Traits0>
    bool operator<=(const iterator<_Buffer, _Traits0>& it) const
    {
        return !(it < *this);
    }

    //! Greater or equal.
    template <class _Traits0>
    bool operator>=(const iterator<_Buffer, _Traits0>& it) const
    {
        return !(*this < it);
    }

    template <class _Traits0>
    typename _Traits0::pointer linearize_pointer(const iterator<_Buffer, _Traits0>& it) const
    {
        return (it.m_iterator == 0) ? (m_buffer->m_buffer + m_buffer->size()) :
            (it.m_iterator < m_buffer->m_first ? it.m_iterator + (m_buffer->m_end - m_buffer->m_first) : m_buffer->m_buffer + (it.m_iterator - m_buffer->m_first));
    }

    const _Buffer* m_buffer;
    pointer m_iterator;
};


template <class _Buffer, class _Traits>
inline iterator<_Buffer, _Traits>
operator+(typename _Traits::difference_type n, const iterator<_Buffer, _Traits>& it)
{
    return it + n;
}


} // namespace _CircularBuffer_details {}


} // namespace internal {}

} // namespace sketch {}
