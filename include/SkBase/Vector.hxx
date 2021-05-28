#pragma once


#include "Iterator.hxx"


namespace sketch
{

// std::vector adaptation that can live without exception support

template <class _Ty, class _Allocator = Allocator<_Ty>>
class Vector
{
    typedef typename _Allocator::template rebind<_Ty>::other allocator;

public:
    typedef _Ty value_type;
    typedef _Allocator allocator_type;
    typedef typename allocator::pointer pointer;
    typedef typename allocator::const_pointer const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename allocator::size_type size_type;
    typedef typename allocator::difference_type difference_type;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef sketch::ReverseIterator<iterator> reverse_iterator;
    typedef sketch::ReverseIterator<const_iterator> const_reverse_iterator;

    bool valid() const noexcept
    {
        return !!m_begin;
    }

    __forceinline ~Vector() noexcept
    {
        if (m_begin)
        {
            clear();
            m_allocator.deallocate(m_begin, m_capacity);
        }
    }

    __forceinline Vector() noexcept
        : m_begin(nullptr)
        , m_end(nullptr)
        , m_capacity(0)
    {
    }

    explicit Vector(size_type n)
        : Vector()
    {
        m_begin = m_allocator.allocate(n);
        if (m_begin)
        {
            m_end = m_begin;
            m_capacity = n;

            while (n--)
            {
                m_allocator.construct(m_end++, forward<value_type>(_Ty()));
            }
        }
    }

    Vector(size_type n, const _Ty& value)
        : Vector()
    {
        if (n)
        {
            m_begin = m_allocator.allocate(n);
        }

        if (m_begin)
        {
            m_capacity = n;
            _construct(n, value);
        }
    }

    Vector(const Vector& x)
        : Vector()
    {
        if (x.capacity())
        {
            m_begin = m_allocator.allocate(x.capacity());
            if (m_begin)
            {
                m_capacity = x.capacity();
                _construct(x.begin(), x.size());
            }
        }
    }

    Vector(Vector&& x) noexcept
        : Vector()
    {
        swap(x);
    }

    Vector<_Ty, _Allocator>& operator=(const Vector<_Ty, _Allocator>& x)
    {
        if (this != &x)
        {
            m_allocator = x.m_allocator;
            assign(x.begin(), x.end());
        }

        return *this;
    }

    Vector<_Ty, _Allocator>& operator=(Vector<_Ty, _Allocator>&& x) noexcept
    {
        if (this != &x)
        {
            Vector(sketch::move(x)).swap(*this);
        }

        return *this;
    }

    template <class _InputIterator>
    bool assign(_InputIterator first, _InputIterator last, typename enable_if<!is_integral<_InputIterator>::value>::type* = 0)
    {
        return _assign_disp(first, last, typename iterator_traits<_InputIterator>::iterator_category());
    }

    bool assign(size_type n, const _Ty& u)
    {
        clear();
        if (!n)
        {
            return true;
        }

        if (capacity() < n)
        {
            m_allocator.deallocate(m_begin, m_capacity);

            m_begin = m_allocator.allocate(n);
            if (!m_begin)
            {
                m_end = nullptr;
                m_capacity = 0;

                return false;
            }

            m_capacity = n;
        }

        _construct(n, u);

        return true;
    }

    iterator erase(const_iterator position) noexcept
    {
        iterator i = &const_cast<value_type&>(*position);
        if (position + 1 != end())
        {
            sketch::move(i + 1, end(), i);
        }

        if (!__::no_dtor<_Ty>::value)
        {
            m_allocator.destroy(--m_end);
        }

        return i;
    }

    iterator erase(const_iterator first, const_iterator last) noexcept
    {
        iterator first_ = &const_cast<value_type&>(*first);
        iterator last_ = &const_cast<value_type&>(*last);
        if (first_ != last_)
        {
            if (!__::no_dtor<_Ty>::value)
            {
                for (iterator i = last_; i != first_; )
                {
                    m_allocator.destroy(--i);
                }
            }

            m_end = sketch::move(last_, end(), first_);

        }
        return first_;
    }

    void clear() noexcept // leaves the capacity unchanged (similar to std::vector)
    {
        difference_type n = m_end - m_begin;
        m_end = m_begin;
        if (n > 0 && !__::no_dtor<_Ty>::value)
        {
            while (n)
            {
                m_allocator.destroy(m_begin + --n);
            }
        }
    }

    void swap(Vector<_Ty, _Allocator>& x) noexcept
    {
        if (this != &x)
        {
            using sketch::swap;
            swap(m_begin, x.m_begin);
            swap(m_end, x.m_end);
            swap(m_capacity, x.m_capacity);
        }
    }

    iterator begin() noexcept
    {
        return m_begin;
    }

    const_iterator begin() const noexcept
    {
        return m_begin;
    }

    iterator end() noexcept
    {
        return m_end;
    }

    const_iterator end() const noexcept
    {
        return m_end;
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(m_end);
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(m_end);
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(m_begin);
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(m_begin);
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

    size_type size() const noexcept
    {
        return static_cast<size_type>(m_end - m_begin);
    }

    size_type max_size() const noexcept
    {
        return m_allocator.max_size();
    }

    size_type capacity() const noexcept
    {
        return m_capacity;
    }

    bool empty() const noexcept
    {
        return m_begin == m_end;
    }

    reference operator[](size_type n) noexcept
    {
        SK_ASSERT(n < size());
        return *(m_begin + n);
    }

    const_reference operator[](size_type n) const noexcept
    {
        SK_ASSERT(n < size());
        return *(m_begin + n);
    }

    reference front() noexcept
    {
        SK_ASSERT(!empty());
        return *begin();
    }

    const_reference front() const noexcept
    {
        SK_ASSERT(!empty());
        return *begin();
    }

    reference back() noexcept
    {
        SK_ASSERT(!empty());
        return *(end() - 1);
    }

    const_reference back() const noexcept
    {
        SK_ASSERT(!empty());
        return *(end() - 1);
    }

    pointer data() noexcept
    {
        return m_begin;
    }

    const_pointer data() const noexcept
    {
        return m_begin;
    }

    bool resize(size_type sz)
    {
        if (sz > max_size())
        {
            return false;
        }

        iterator new_end = m_begin + sz;
        if (new_end < m_end)
        {
            erase(new_end, m_end);
            return true;
        }
        else if (new_end > m_end)
        {
            return !!_insert_impl(m_end, new_end - m_end, _Ty());
        }

        return true; // nothing to do
    }

    bool resize(size_type sz, const _Ty& c)
    {
        if (sz > max_size())
        {
            return false;
        }

        iterator new_end = m_begin + sz;
        if (new_end < m_end)
        {
            erase(new_end, m_end);
            return true;
        }
        else if (new_end > m_end)
        {
            return !!insert(m_end, new_end - m_end, c);
        }

        return true; // nothing to do
    }

    bool reserve(size_type n)
    {
        if (n > max_size())
        {
            return false;
        }

        if (capacity() < n)
        {
            return _realloc(n);
        }

        return true;
    }

    template <class... Args> bool emplace_back(Args&&... args)
    {
        return (emplace(end(), sketch::forward<Args>(args)...) != end());
    }

    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args)
    {
        iterator r_dest = _insert_blank_space(position, 1);
        if (!r_dest)
        {
            return end();
        }

        m_allocator.construct(--r_dest, sketch::forward<Args>(args)...);
        return r_dest;
    }

    bool push_back(_Ty&& x)
    {
        if (size() == capacity())
        {
            if (!_realloc(_capacity_factor()))
            {
                return false;
            }
        }

        m_allocator.construct(m_end++, forward<value_type>(x));

        return true;
    }

    __forceinline bool push_back(const _Ty& x)
    {
        if (size() == capacity())
        {
            if (!_realloc(_capacity_factor()))
            {
                return false;
            }
        }

        m_allocator.construct(m_end++, (x));

        return true;
    }

    void pop_back() noexcept
    {
        m_allocator.destroy(--m_end);
    }

    iterator insert(const_iterator position, const _Ty& x)
    {
        auto r = _insert_impl(position, 1, x);
        return r ? r : end();
    }

    iterator insert(const_iterator position, _Ty&& x)
    {
        auto r = _insert_impl(position, 1, forward<value_type>((x)));
        return r ? r : end();
    }

    iterator insert(const_iterator position, size_type n, const _Ty& x)
    {
        auto r = _insert_impl(position, n, x);
        return r ? r : end();
    }

    template <class InputIterator>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
        auto r = _insert_disp(&const_cast<value_type&>(*position), first, last, is_integral<InputIterator>::type());
        return r ? r : end();
    }

private:
    template <class _InputIterator>
    bool _assign_disp(_InputIterator first, _InputIterator last, input_iterator_tag)
    {
        clear();
        while (first != last)
        {
            if (!push_back(*first))
            {
                return false; // not all elements could be added; always check size() after assign()
            }

            ++first;
        }

        return true;
    }

    template <class _InputIterator>
    bool _assign_disp(_InputIterator first, _InputIterator last, forward_iterator_tag)
    {
        clear();

        size_type n = static_cast<size_type>(sketch::distance(first, last));
        if (capacity() < n)
        {
            if (m_begin)
            {
                m_allocator.deallocate(m_begin, m_capacity);
            }

            m_begin = m_allocator.allocate(n);
            if (m_begin)
            {
                m_capacity = n;
            }
            else
            {
                m_end = nullptr;
                m_capacity = 0;

                return false;
            }
        }

        _construct(first, n);

        return true;
    }

    void _construct(size_type n, const _Ty& value)
    {
        uninitialized_fill_n(m_begin, n, value);
        m_end = m_begin + n;
    }

    template <class _InputIterator>
    __forceinline void _construct(_InputIterator first, size_type n)
    {
        iterator i = m_begin;
        while (n)
        {
            m_allocator.construct(i++, *first++);
            n--;
        }

        m_end = i;
    }

    void _move(const iterator to, const iterator from) const
    {
        m_allocator.construct(to, sketch::forward<value_type>(*from));
        m_allocator.destroy(from);
    }

    bool _realloc(size_type n)
    {
        const iterator new_mem = m_allocator.allocate(n);
        if (!new_mem)
        {
            return false;
        }

        const size_type old_capacity = m_capacity;
        m_capacity = n;
        iterator dest = new_mem;

        for (iterator src = m_begin; src != m_end; ++src, ++dest)
        {
            _move(dest, src);
        }

        if (m_begin)
        {
            m_allocator.deallocate(m_begin, old_capacity);
        }

        m_begin = new_mem;
        m_end = dest;

        return true;
    }

    iterator _insert_blank_space(const_iterator position, const size_type n)
    {
        const size_type old_capacity = m_capacity;
        iterator old_mem = 0;
        iterator new_end = m_end + n;

        difference_type tail_size = m_end - position;

        if (m_capacity < m_end - m_begin + n)
        {
            old_mem = m_begin;
            size_type cap = n + _capacity_factor();
            const iterator new_mem = m_allocator.allocate(cap);
            if (!new_mem)
            {
                return 0;
            }

            m_capacity = cap;
            new_end = new_mem + difference_type(new_end - old_mem);

            iterator dest = m_begin = new_mem;

            for (iterator src = old_mem; src != position; ++src, ++dest)
            {
                _move(dest, src);
            }
        }

        iterator r_src = end();
        iterator r_dest = m_end = new_end;
        while (tail_size--)
        {
            _move(--r_dest, --r_src);
        }

        if (old_mem)
        {
            m_allocator.deallocate(old_mem, old_capacity);
        }

        return r_dest;
    }

    iterator _insert_impl(const_iterator position, size_type n, const _Ty& x)
    {
        iterator r_dest = _insert_blank_space(position, n);
        if (!r_dest)
        {
            return 0;
        }

        r_dest -= n;
        uninitialized_fill_n(r_dest, n, x);

        return r_dest;
    }

    iterator _insert_impl(const_iterator position, size_type n, _Ty&& x)
    {
        iterator r_dest = _insert_blank_space(position, n);
        if (!r_dest)
        {
            return 0;
        }

        while (n--)
        {
            m_allocator.construct(--r_dest, sketch::move(x));
        }

        return r_dest;
    }

    template <class _InputIterator>
    iterator _insert_disp_it(iterator position, _InputIterator first, _InputIterator last, const input_iterator_tag&)
    {
        while (first != last)
        {
            auto it = _insert_impl(position, 1, *first++);
            if (!it)
            {
                break;
            }

            position = it + 1;
        }

        return position;
    }

    template <class _InputIterator>
    iterator _insert_disp_it(iterator position, _InputIterator first, _InputIterator last, const random_access_iterator_tag&)
    {
        size_t const n = static_cast<size_type>(last - first);
        bool const insert_from_self = begin() <= &*first && &*first < end();
        iterator i = _insert_blank_space(position, n);
        if (!i)
        {
            return 0;
        }

        const difference_type disp = insert_from_self ? i - position - n : 0;
        position = i;
        while (first != last)
        {
            m_allocator.construct(--position, (--last)[disp]);
        }

        return i;
    }

    template <class _InputIterator>
    iterator _insert_disp(iterator position, _InputIterator first, _InputIterator last, const false_type&)
    {
        return _insert_disp_it(position, first, last, iterator_traits<_InputIterator>::iterator_category());
    }

    template <class IntegralType>
    iterator _insert_disp(iterator position, IntegralType n, IntegralType x, const true_type&)
    {
        return _insert_impl(position, static_cast<size_type>(n), static_cast<value_type>(x));
    }

    size_type _capacity_factor() const
    {
        return (m_capacity + 4) * 2;
    }

private:
    pointer m_begin;
    pointer m_end;
    size_type m_capacity;
    mutable allocator m_allocator;
};


template <class _Ty, class _Allocator>
inline void swap(Vector<_Ty, _Allocator>& x, Vector<_Ty, _Allocator>& y) noexcept
{
    x.swap(y);
}


} // namespace sketch {}
