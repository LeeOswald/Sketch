#pragma once

#include "../Sketch.hxx"

#ifdef SKETCH_STL_AVAILABLE
#include <iterator>
#include <utility>
#endif


namespace sketch
{

#ifdef SKETCH_STL_AVAILABLE // try to be compatible with STL

    using std::input_iterator_tag;
    using std::output_iterator_tag;
    using std::forward_iterator_tag;
    using std::bidirectional_iterator_tag;
    using std::random_access_iterator_tag;

    using std::iterator_traits;

#else

    struct input_iterator_tag
    {
    };

    struct output_iterator_tag
    {
    };

    struct forward_iterator_tag
        : public input_iterator_tag
    {
    };

    struct bidirectional_iterator_tag
        : public forward_iterator_tag
    {
    };

    struct random_access_iterator_tag
        : public bidirectional_iterator_tag
    {
    };

    template <class _Iterator>
    struct iterator_traits
    {
        typedef typename _Iterator::difference_type difference_type;
        typedef typename _Iterator::value_type value_type;
        typedef typename _Iterator::pointer pointer;
        typedef typename _Iterator::reference reference;
        typedef typename _Iterator::iterator_category iterator_category;
    };

    template <class _Ty>
    struct iterator_traits<_Ty*>
    {
        typedef sketch::ptrdiff_t difference_type;
        typedef _Ty value_type;
        typedef _Ty* pointer;
        typedef _Ty& reference;
        typedef random_access_iterator_tag iterator_category;
    };

    template <class _Ty>
    struct iterator_traits<const _Ty*>
    {
        typedef sketch::ptrdiff_t difference_type;
        typedef _Ty value_type;
        typedef const _Ty* pointer;
        typedef const _Ty& reference;
        typedef random_access_iterator_tag iterator_category;
    };

#endif // SKETCH_STL_AVAILABLE

} // namespace sketch {}


namespace sketch
{


template <class _Iterator>
struct iterator_value
{
    typedef typename iterator_traits<_Iterator>::value_type type;
};

template <class _Iterator>
struct iterator_reference
{
    typedef typename iterator_traits<_Iterator>::reference type;
};

template <class _Iterator>
struct iterator_pointer
{
    typedef typename iterator_traits<_Iterator>::pointer type;
};

template <class _Iterator>
struct iterator_difference
{
    typedef typename iterator_traits<_Iterator>::difference_type type;
};

template <class _Iterator>
struct iterator_category
{
    typedef typename iterator_traits<_Iterator>::iterator_category type;
};


template <
    class _Category,
    class _Ty,
    class _Distance = sketch::ptrdiff_t,
    class _Pointer = _Ty*,
    class _Reference = _Ty&
    >
struct Iterator
{
    typedef _Ty value_type;
    typedef _Distance difference_type;
    typedef _Pointer pointer;
    typedef _Reference reference;
    typedef _Category iterator_category;
};


namespace __
{

template <class _InputIterator, class _Distance>
static inline void advance(_InputIterator& i, _Distance n, const input_iterator_tag&)
{
    advance(i, n, forward_iterator_tag());
}

template <class _ForwardIterator, class _Distance>
static inline void advance(_ForwardIterator& i, _Distance n, const forward_iterator_tag&)
{
    for (; 0 < n; --n)
    {
        ++i;
    }
}

template <class _BidirectionalIterator, class _Distance>
static inline void advance(_BidirectionalIterator& i, _Distance n, const bidirectional_iterator_tag&)
{
    if (0 < n)
    {
        advance(i, n, forward_iterator_tag());
    }
    else
    {
        for (; n < 0; ++n)
        {
            --i;
        }
    }
}

template <class _RandomIterator, class _Distance>
static inline void advance(_RandomIterator& i, _Distance n, const random_access_iterator_tag&)
{
    i += n;
}

template <class _InputIterator>
static inline typename iterator_traits<_InputIterator>::difference_type
distance(const _InputIterator& first, const _InputIterator& last, const input_iterator_tag&)
{
    return distance(first, last, forward_iterator_tag());
}

template <class _ForwardIterator>
static inline typename iterator_traits<_ForwardIterator>::difference_type
distance(const _ForwardIterator& first, const _ForwardIterator& last, const forward_iterator_tag&)
{
    _ForwardIterator from(first);
    _ForwardIterator to(last);
    typename iterator_traits<_ForwardIterator>::difference_type d = 0;
    for (; from != to; ++from)
    {
        ++d;
    }
    return d;
}

template <class _BidirectionalIterator>
static inline typename iterator_traits<_BidirectionalIterator>::difference_type
distance(const _BidirectionalIterator& first, const _BidirectionalIterator& last, const bidirectional_iterator_tag&)
{
    return distance(first, last, forward_iterator_tag());
}

template <class _RandomIterator>
static inline typename iterator_traits<_RandomIterator>::difference_type
distance(const _RandomIterator& first, const _RandomIterator& last, const random_access_iterator_tag&)
{
    return last - first;
}

} //namespace __ {}


template <class _AnyIterator, class _Distance>
inline void advance(_AnyIterator& i, _Distance n)
{
    __::advance(i, n, iterator_traits<_AnyIterator>::iterator_category());
}

template <class _AnyIterator>
inline typename iterator_traits<_AnyIterator>::difference_type
distance(_AnyIterator first, _AnyIterator last)
{
    return __::distance(first, last, iterator_traits<_AnyIterator>::iterator_category());
}

template <class InputIterator>
inline InputIterator next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1)
{
    advance(x, n);
    return x;
}

template <class InputIterator>
inline InputIterator prev(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1)
{
    advance(x, -n);
    return x;
}


template <class _Iterator>
class ReverseIterator
    : public Iterator<
        typename iterator_traits<_Iterator>::iterator_category,
        typename iterator_traits<_Iterator>::value_type,
        typename iterator_traits<_Iterator>::difference_type,
        typename iterator_traits<_Iterator>::pointer,
        typename iterator_traits<_Iterator>::reference
    >
{
public:
    typedef _Iterator iterator_type;
    typedef typename iterator_traits<_Iterator>::difference_type difference_type;
    typedef typename iterator_traits<_Iterator>::reference reference;
    typedef typename iterator_traits<_Iterator>::pointer pointer;

    ReverseIterator()
    {
    }

    explicit ReverseIterator(_Iterator x)
        : current(x)
    {
    }

    template <class _U>
    friend class ReverseIterator;

    template <class _U>
    ReverseIterator(const ReverseIterator<_U>& u)
        : current(u.current)
    {
    }

    template <class _U>
    ReverseIterator& operator=(const ReverseIterator<_U>& u)
    {
        current = u.current;
        return *this;
    }

    _Iterator base() const
    {
        return current;
    }

    reference operator*() const
    {
        _Iterator tmp = current;
        return *--tmp;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    ReverseIterator& operator++()
    {
        --current;
        return *this;
    }

    ReverseIterator operator++(int)
    {
        ReverseIterator tmp = *this;
        --current;
        return tmp;
    }

    ReverseIterator& operator--()
    {
        ++current;
        return *this;
    }

    ReverseIterator operator--(int)
    {
        ReverseIterator tmp = *this;
        ++current;
        return tmp;
    }

    ReverseIterator operator+(difference_type n) const
    {
        return ReverseIterator(current - n);
    }

    ReverseIterator& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }

    ReverseIterator operator-(difference_type n) const
    {
        return ReverseIterator(current + n);
    }

    ReverseIterator& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const
    {
        return current[-n - 1];
    }

    template<typename _Iterator2>
    friend bool operator==(const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current == y.base();
    }

    template<typename _Iterator2>
    friend bool operator!=(const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current != y.base();
    }

    template<typename _Iterator2>
    friend bool operator< (const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current > y.base();
    }

    template<typename _Iterator2>
    friend bool operator> (const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current < y.base();
    }

    template<typename _Iterator2>
    friend bool operator>=(const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current <= y.base();
    }

    template<typename _Iterator2>
    friend bool operator<=(const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return x.current >= y.base();
    }

    template<typename _Iterator2>
    friend typename ReverseIterator<_Iterator>::difference_type operator-(const ReverseIterator<_Iterator>& x, const ReverseIterator<_Iterator2>& y)
    {
        return y.base() - x.current;
    }

    friend inline ReverseIterator<_Iterator> operator+(typename ReverseIterator<_Iterator>::difference_type n, const ReverseIterator<_Iterator>& x)
    {
        return ReverseIterator(x.current - n);
    }

protected:
    _Iterator current;
};


template <class _C>
inline auto begin(_C& c) -> decltype(c.begin())
{
    return c.begin();
}

template <class _C>
inline auto begin(const _C& c) -> decltype(c.begin())
{
    return c.begin();
}

template <class _C>
inline auto end(_C& c) -> decltype(c.end())
{
    return c.end();
}

template <class _C>
inline auto end(const _C& c) -> decltype(c.end())
{
    return c.end();
}


template <class _Ty, size_t _N>
inline _Ty* begin(_Ty(&array)[_N])
{
    return array;
}

template <class _Ty, size_t _N>
inline _Ty* end(_Ty(&array)[_N])
{
    return array + _N;
}


template <class _Ty, size_t _N>
inline constexpr bool empty(const _Ty(&array)[_N]) noexcept
{
    return false;
}

template <class _Ty, size_t _N>
inline constexpr size_t size(const _Ty(&array)[_N]) noexcept
{
    return _N;
}

template <class _Ty, size_t _N>
inline constexpr _Ty& front(_Ty(&array)[_N]) noexcept
{
    return array[0];
}

template <class _Ty, size_t _N>
inline constexpr _Ty& back(_Ty(&array)[_N]) noexcept
{
    return array[_N - 1];
}

template <class _Ty, size_t _N>
inline constexpr _Ty* data(_Ty(&array)[_N]) noexcept
{
    return array;
}

template <class _C>
inline constexpr bool empty(const _C& c) noexcept
{
    return c.empty();
}

template <class _C>
inline constexpr auto size(const _C& c) noexcept -> decltype(c.size())
{
    return c.size();
}

template <class _C>
inline constexpr auto front(_C& c) -> decltype(c.front())
{
    return c.front();
}

template <class _C>
inline constexpr auto front(const _C& c) -> decltype(c.front())
{
    return c.front();
}

template <class _C>
inline constexpr auto back(_C& c) -> decltype(c.back())
{
    return c.back();
}

template <class _C>
inline constexpr auto back(const _C& c) -> decltype(c.back())
{
    return c.back();
}

template <class _C>
inline constexpr auto data(_C& c) noexcept -> decltype(c.data())
{
    return c.data();
}

template <class _C>
inline constexpr auto data(const _C& c) noexcept -> decltype(c.data())
{
    return c.data();
}


} // namespace sketch {}
