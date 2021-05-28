#pragma once

#include "../Sketch.hxx"

#include "Iterator.hxx"
#include "type_traits.hxx"


namespace sketch
{

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template<class _Ty>
__forceinline const _Ty& min(const _Ty& a, const _Ty& b)
{
    return (b < a) ? b : a;
}

template<class _Ty>
__forceinline const _Ty& max(const _Ty& a, const _Ty& b)
{
    return (a < b) ? b : a;
}


template <typename _Ty>
inline _Ty&& forward(typename remove_reference<_Ty>::type& t)
{
    return static_cast<_Ty&&>(t);
}

template <typename _Ty>
inline _Ty&& forward(typename remove_reference<_Ty>::type&& t)
{
    static_assert(!sketch::is_lvalue_reference<_Ty>::value, "lvalue not allowed");
    return static_cast<_Ty&&>(t);
}

template <typename _Ty>
inline typename remove_reference<_Ty>::type&& move(_Ty&& t)
{
    return static_cast<typename remove_reference<_Ty>::type&&>(t);
}

template<typename _Ty>
inline void swap(_Ty& a, _Ty& b)
{
    _Ty temp(move(a));
    a = move(b);
    b = move(temp);
}


template <class _InputIterator, class _ForwardIterator>
__forceinline _ForwardIterator uninitialized_copy(_InputIterator first, _InputIterator last, _ForwardIterator result)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; first != last; ++result, ++first)
    {
        __assume(&*result);
        new (static_cast<void*>(&*result)) value_type(*first);
    }

    return result;
}

template <class _InputIterator, class _Size, class _ForwardIterator>
__forceinline _ForwardIterator uninitialized_copy_n(_InputIterator first, _Size n, _ForwardIterator result)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; n--; ++result, ++first)
    {
        __assume(&*result);
        new (static_cast<void*>(&*result)) value_type(*first);
    }

    return result;
}

template <class _InputIterator, class _ForwardIterator>
__forceinline _ForwardIterator uninitialized_move(_InputIterator first, _InputIterator last, _ForwardIterator result)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; first != last; ++result, ++first)
    {
        __assume(&*result);
        new (static_cast<void*>(&*result)) value_type(move(*first));
    }

    return result;
}

template <class _InputIterator, class _Size, class _ForwardIterator>
__forceinline _ForwardIterator uninitialized_move_n(_InputIterator first, _Size n, _ForwardIterator result)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; n--; ++result, ++first)
    {
        __assume(&*result);
        new (static_cast<void*>(&*result)) value_type(move(*first));
    }

    return result;
}

template <class _ForwardIterator, class _Ty>
__forceinline void uninitialized_fill(_ForwardIterator first, _ForwardIterator last, const _Ty& x)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; first != last; ++first)
    {
        __assume(&*first);
        new (static_cast<void*>(&*first)) value_type(x);
    }
}

template <class _ForwardIterator, class _Size, class _Ty>
__forceinline void uninitialized_fill_n(_ForwardIterator first, _Size n, const _Ty& x)
{
    typedef typename iterator_traits<_ForwardIterator>::value_type value_type;
    for (; n--; ++first)
    {
        __assume(&*first);
        new (static_cast<void*>(&*first)) value_type(x);
    }
}


template<class _InputIterator, class _OutputIterator>
__forceinline _OutputIterator copy(_InputIterator first, _InputIterator last, _OutputIterator result)
{
    while (first != last)
    {
        *result = *first;
        ++result;
        ++first;
    }

    return result;
}

namespace __
{

template<class _InputIterator, class _Size, class _OutputIterator>
inline _OutputIterator copy_n(_InputIterator first, _Size n, _OutputIterator result, const input_iterator_tag&)
{
    for (; n; --n, ++first, ++result)
    {
        *result = *first;
    }

    return result;
}

template<class _RandomAccessIterator, class _Size, class _OutputIterator>
inline _OutputIterator copy_n(_RandomAccessIterator first, _Size n, _OutputIterator result, const random_access_iterator_tag&)
{
    return copy(first, first + n, result);
}

template<class _InputIterator, class _OutputIterator>
inline _OutputIterator move_impl(_InputIterator first, _InputIterator last, _OutputIterator result, false_type)
{
    while (first != last)
    {
        *result = sketch::move(*first);
        ++result;
        ++first;
    }

    return result;
}

template<class _InputRandomIterator, class _OutputRandomIterator>
inline _OutputRandomIterator move_impl(_InputRandomIterator first, _InputRandomIterator last, _OutputRandomIterator result, true_type)
{
    memmove(result, first, (last - first) * sizeof(typename iterator_traits<_InputRandomIterator>::value_type));
    return result + (last - first);
}

template<class I, class O = I>
struct is_memmoveable_check
{
    typedef typename iterator_traits<I>::iterator_category itype;
    typedef typename iterator_traits<O>::iterator_category otype;
    typedef typename iterator_traits<I>::value_type src_type;
    typedef typename iterator_traits<O>::value_type dest_type;

    typedef bool_type<is_pointer<I>::value && is_pointer<O>::value> ptrs;
    static const bool same = is_same<typename remove_cv<src_type>::type, typename remove_cv<dest_type>::type>::value;
    static const bool value = ptrs::value && same && has_trivial_copy_constructor<dest_type>::value;
};

template<class I, class O>
struct is_memmoveable :
    integral_constant<bool, is_memmoveable_check<I, O>::value>
{
};


} // namespace __ {}


template<class _InputIterator, class _Size, class _OutputIterator>
inline _OutputIterator copy_n(_InputIterator first, _Size n, _OutputIterator result)
{
    return __::copy_n(first, n, result, iterator_traits<_InputIterator>::iterator_category());
}

template<class _InputIterator, class _OutputIterator, class _Predicate>
inline _OutputIterator copy_if(_InputIterator first, _InputIterator last, _OutputIterator result, _Predicate pred)
{
    for (; first != last; ++first, ++result)
    {
        if (pred(*first))
        {
            *result = *first;
        }
    }

    return result;
}


template<class _BidirectionalIterator1, class _BidirectionalIterator2>
__forceinline _BidirectionalIterator2 copy_backward(_BidirectionalIterator1 first, _BidirectionalIterator1 last, _BidirectionalIterator2 result_last)
{
    while (first != last)
    {
        *--result_last = *--last;
    }

    return result_last;
}


template<class _InputIterator, class _OutputIterator>
inline _OutputIterator move(_InputIterator first, _InputIterator last, _OutputIterator result)
{
    return __::move_impl<_InputIterator, _OutputIterator>(first, last, result, __::is_memmoveable<_InputIterator, _OutputIterator>());
}

template<class _BidirectionalIterator1, class _BidirectionalIterator2>
inline _BidirectionalIterator2 move_backward(_BidirectionalIterator1 first, _BidirectionalIterator1 last, _BidirectionalIterator2 result_last)
{
    while (first != last)
    {
        *--result_last = move(*--last);
    }

    return result_last;
}


} // namespace sketch {}
