#pragma once

#include "../Sketch.hxx"

#include "Iterator.hxx"


namespace sketch
{


template <class _Ty, size_t _N>
struct Array
{
    typedef _Ty* pointer;
    typedef const _Ty* const_pointer;
    typedef _Ty& reference;
    typedef const _Ty& const_reference;
    typedef _Ty* iterator;
    typedef const _Ty* const_iterator;
    typedef size_t size_type;
    typedef sketch::ptrdiff_t difference_type;
    typedef _Ty value_type;
    typedef sketch::ReverseIterator<iterator> reverse_iterator;
    typedef sketch::ReverseIterator<const_iterator> const_reverse_iterator;

    void fill(const _Ty& u)
    {
        for (size_t i = 0; i < _N; i++)
        {
            __elems[i] = u;
        }
    }

    void swap(Array<_Ty, _N>& y) noexcept
    {
        using sketch::swap;
        for (size_t i = 0; i != size(); ++i)
        {
            swap(__elems[i], y.__elems[i]);
        }
    }

    iterator begin() noexcept
    {
        return &__elems[0];
    }

    const_iterator begin() const noexcept
    {
        return &__elems[0];
    }

    iterator end() noexcept
    {
        return &__elems[_N];
    }

    const_iterator end() const noexcept
    {
        return &__elems[_N];
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    constexpr size_type size() const noexcept
    {
        return _N;
    }

    constexpr size_type max_size() const noexcept
    {
        return _N;
    }

    constexpr bool empty() const noexcept
    {
        return (_N != 0);
    }

    reference operator[](size_type n) noexcept
    {
        SK_ASSERT(n < _N);
        return __elems[n];
    }

    const_reference operator[](size_type n) const noexcept
    {
        SK_ASSERT(n < _N);
        return __elems[n];
    }

    reference front() noexcept
    {
        return *begin();
    }

    const_reference front() const noexcept
    {
        return *begin();
    }

    reference back() noexcept
    {
        return __elems[_N - 1];
    }

    const_reference back() const noexcept
    {
        return __elems[_N - 1];
    }

    pointer data() noexcept
    {
        return __elems;
    }

    const_pointer data() const noexcept
    {
        return __elems;
    }

    // aggregate types cannot have private:
    _Ty __elems[_N];
};


template <class _Ty, size_t _N>
inline void swap(Array<_Ty, _N>& x, Array<_Ty, _N>& y)
{
    x.swap(y);
}



} // namespace sketch {}
