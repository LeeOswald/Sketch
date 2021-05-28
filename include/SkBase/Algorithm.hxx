#pragma once

#include "../Sketch.hxx"


namespace sketch
{


template<class _InputIterator, class _Function>
__forceinline _Function for_each(_InputIterator first, _InputIterator last, _Function f)
{
    while (first != last) f(*first++);
    return f;
}


template<class _InputIterator, class _Ty>
__forceinline _InputIterator find(_InputIterator first, _InputIterator last, const _Ty& value)
{
    while (first != last && !(*first == value)) ++first;
    return first;
}


template<class _InputIterator, class _Predicate>
__forceinline _InputIterator find_if(_InputIterator first, _InputIterator last, _Predicate pred)
{
    while (first != last && !(pred(*first) != false)) ++first;
    return first;
}


template<class _InputIterator, class _Ty>
__forceinline typename iterator_traits<_InputIterator>::difference_type count(_InputIterator first, _InputIterator last, const _Ty& value)
{
    typename iterator_traits<_InputIterator>::difference_type count = 0;

    for (; first != last; ++first)
    {
        if (*first == value) ++count;
    }

    return count;
}


template<class _InputIterator, class _Predicate>
__forceinline typename iterator_traits<_InputIterator>::difference_type count_if(_InputIterator first, _InputIterator last, _Predicate pred)
{
    typename iterator_traits<_InputIterator>::difference_type count = 0;

    for (; first != last; ++first)
    {
        if (pred(*first) != false) ++count;
    }

    return count;
}

} // namespace sketch {}
