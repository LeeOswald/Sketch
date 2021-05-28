#pragma once

#include "../Sketch.hxx"


namespace sketch
{

template<typename _T1, typename _T2>
struct Pair
{
    typedef _T1 first_type;
    typedef _T2 second_type;

    _T1 first;
    _T2 second;

    Pair(const Pair& p)
        : first(p.first)
        , second(p.second)
    {
    }

    Pair()
    {
    }

    constexpr Pair(const _T1& x, const _T2& y)
        : first(x)
        , second(y)
    {
    }

    template<typename _U, typename _V>
    constexpr Pair(const Pair<_U, _V>& p)
        : first(p.first)
        , second(p.second)
    {
    }

    template<typename _U, typename _V>
    constexpr Pair(_U&& x, _V&& y)
        : first(sketch::forward<_U>(x))
        , second(sketch::forward<_V>(y))
    {
    }

    constexpr Pair(Pair&& p)
        : first(sketch::move(p.first))
        , second(sketch::move(p.second))
    {
    }

    template<typename _U, typename _V>
    constexpr Pair(Pair<_U, _V>&& p)
        : first(sketch::forward<_U>(p.first))
        , second(sketch::forward<_V>(p.second))
    {
    }

    Pair& operator=(const Pair& p)
    {
        first = p.first;
        second = p.second;
        return *this;
    }

    template<typename _U, typename _V>
    Pair& operator=(const Pair<_U, _V>& p)
    {
        first = p.first;
        second = p.second;
        return *this;
    }

    Pair& operator=(Pair&& p)
    {
        first = sketch::forward<first_type>(p.first);
        second = sketch::forward<second_type>(p.second);
        return *this;
    }

    template<class _U, class _V>
    Pair& operator=(Pair<_U, _V>&& p)
    {
        first = sketch::forward<_U>(p.first);
        second = sketch::forward<_V>(p.second);
        return *this;
    }

    void swap(Pair& p)
    {
        using sketch::swap;
        swap(first, p.first);
        swap(second, p.second);
    }
};


template<typename _T1, typename _T2>
inline bool operator==(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    return x.first == y.first && x.second == y.second;
}

template<typename _T1, typename _T2>
inline bool operator<(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    const bool less = x.first < y.first;
    return less || (!less && x.second < y.second);
}

template<typename _T1, typename _T2>
inline bool operator!=(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    return !(x == y);
}

template<typename _T1, typename _T2>
inline bool operator>(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    return y < x;
}

template<typename _T1, typename _T2>
inline bool operator>=(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    return !(y < x);
}

template<typename _T1, typename _T2>
inline bool operator<=(const Pair<_T1, _T2>& x, const Pair<_T1, _T2>& y)
{
    return !(x < y);
}

template<typename _T1, typename _T2>
inline void swap(Pair<_T1, _T2>& x, Pair<_T1, _T2>& y)
{
    x.swap(y);
}


template<typename _Ty>
class reference_wrapper;

namespace __
{

template<class _Ty>
struct pair_type
{
    typedef typename decay<_Ty>::type U;
    typedef typename conditional<is_same<U, reference_wrapper<_Ty>>::value, _Ty&, U>::type type;
};

template<class _T1, class _T2>
struct pair_result
{
    typedef typename pair_type<_T1>::type V1;
    typedef typename pair_type<_T2>::type V2;
    typedef typename sketch::Pair<V1, V2> type;
};

} // namespace __ {}

template<class _T1, class _T2>
inline typename __::pair_result<_T1, _T2>::type make_pair(_T1&& x, _T2&& y)
{
    return __::pair_result<_T1, _T2>::type(forward<_T1>(x), forward<_T2>(y));
}


template <class _InputIterator>
inline _InputIterator begin(const sketch::Pair<_InputIterator, _InputIterator>& p)
{
    return p.first;
}

template <class _InputIterator>
inline _InputIterator end(const sketch::Pair<_InputIterator, _InputIterator>& p)
{
    return p.second;
}

} // namespace sketch {}
