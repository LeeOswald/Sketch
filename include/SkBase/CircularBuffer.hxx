#pragma once

#include "../Sketch.hxx"

#include "CircularBuffer_details.hxx"
#include "Pair.hxx"
#include "type_traits.hxx"


namespace sketch
{


// Adaptation of boost.circualr_buffer class

template <class _Ty, class _Allocator = Allocator<_Ty>>
class CircularBuffer
{
    typedef typename _Allocator::template rebind<_Ty>::other allocator;

public:
    typedef _Allocator allocator_type;

    typedef CircularBuffer<_Ty, _Allocator> this_type;

    typedef _Ty value_type;
    typedef typename allocator::pointer pointer;
    typedef typename allocator::const_pointer const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef typename allocator::size_type size_type;
    typedef typename allocator::difference_type difference_type;

    typedef internal::_CircularBuffer_details::iterator<CircularBuffer<_Ty, _Allocator>, internal::_CircularBuffer_details::const_traits<_Allocator>> const_iterator;
    typedef internal::_CircularBuffer_details::iterator<CircularBuffer<_Ty, _Allocator>, internal::_CircularBuffer_details::nonconst_traits<_Allocator>> iterator;

    typedef sketch::ReverseIterator<iterator> reverse_iterator;
    typedef sketch::ReverseIterator<const_iterator> const_reverse_iterator;

    typedef sketch::Pair<pointer, size_type> array_range;
    typedef sketch::Pair<const_pointer, size_type> const_array_range;

    typedef const value_type& param_value_type;
    typedef value_type&& rvalue_type;

private:
    template <class _Val>
    static inline typename sketch::conditional<
        ((sketch::is_nothrow_move_constructible<_Val>::value && sketch::is_nothrow_move_assignable<_Val>::value) || !sketch::is_copy_constructible<_Val>::value),
        rvalue_type,
        param_value_type
    >::type
    move_if_noexcept(_Val& value) noexcept
    {
        return sketch::move(value);
    }

    template <class _Buffer, class _Traits>
    friend struct internal::_CircularBuffer_details::iterator;

public:
    ~CircularBuffer() noexcept
    {
        destroy();
    }

    explicit CircularBuffer(const allocator_type& alloc = allocator_type())
        : m_allocator(alloc)
    {
        // zero capacity
    }

    explicit CircularBuffer(size_type buffer_capacity, const allocator_type& alloc = allocator_type())
        : m_allocator(alloc)
    {
        initialize_buffer(buffer_capacity);
        m_first = m_last = m_buffer;
    }

    CircularBuffer(size_type n, param_value_type item, const allocator_type& alloc = allocator_type())
        : m_size(n)
        , m_allocator(alloc)
    {
        initialize_buffer(n, item);
        m_first = m_last = m_buffer;
    }

    CircularBuffer(size_type buffer_capacity, size_type n, param_value_type item, const allocator_type& alloc = allocator_type())
        : m_size(n)
        , m_allocator(alloc)
    {
        SK_ASSERT(buffer_capacity >= size()); // check for capacity lower than size
        initialize_buffer(buffer_capacity, item);
        m_first = m_buffer;
        m_last = (buffer_capacity == n) ? m_buffer : (m_buffer + n);
    }

    CircularBuffer(const CircularBuffer<_Ty, _Allocator>& cb)
        : m_size(cb.size())
        , m_allocator(cb.get_allocator())
    {
        initialize_buffer(cb.capacity());
        m_first = m_buffer;

        m_last = internal::_CircularBuffer_details::uninitialized_copy<value_type>(cb.begin(), cb.end(), m_buffer);

        if (m_last == m_end)
        {
            m_last = m_buffer;
        }
    }

    CircularBuffer(CircularBuffer<_Ty, _Allocator>&& cb) noexcept
        : m_allocator(cb.get_allocator())
    {
        cb.swap(*this);
    }

    template <class _InputIterator>
    CircularBuffer(_InputIterator first, _InputIterator last, const allocator_type& alloc = allocator_type())
        : m_allocator(alloc)
    {
        initialize(first, last, is_integral<_InputIterator>());
    }

    template <class _InputIterator>
    CircularBuffer(size_type buffer_capacity, _InputIterator first, _InputIterator last, const allocator_type& alloc = allocator_type())
        : m_allocator(alloc)
    {
        initialize(buffer_capacity, first, last, sketch::is_integral<_InputIterator>());
    }

    CircularBuffer<_Ty, _Allocator>& operator=(const CircularBuffer<_Ty, _Allocator>& cb)
    {
        if (this == &cb)
        {
            return *this;
        }

        pointer buff = allocate(cb.capacity());
        reset(buff, internal::_CircularBuffer_details::uninitialized_copy<value_type>(cb.begin(), cb.end(), buff), cb.capacity());

        return *this;
    }

    CircularBuffer<_Ty, _Allocator>& operator=(CircularBuffer<_Ty, _Allocator>&& cb) noexcept
    {
        cb.swap(*this);
        CircularBuffer<_Ty, _Allocator>(get_allocator()).swap(cb);
        return *this;
    }

    bool valid() const noexcept
    {
        return (m_buffer != nullptr);
    }

    const void* raw() const noexcept
    {
        return m_buffer;
    }

    void assign(size_type n, param_value_type item)
    {
        assign_n(n, n, internal::_CircularBuffer_details::assign_n<param_value_type, allocator_type>(n, item, m_allocator));
    }

    void assign(size_type buffer_capacity, size_type n, param_value_type item)
    {
        SK_ASSERT(buffer_capacity >= n); // check for new capacity lower than n
        assign_n(buffer_capacity, n, internal::_CircularBuffer_details::assign_n<param_value_type, allocator_type>(n, item, m_allocator));
    }

    template <class _InputIterator>
    void assign(_InputIterator first, _InputIterator last)
    {
        assign(first, last, sketch::is_integral<_InputIterator>());
    }

    template <class _InputIterator>
    void assign(size_type buffer_capacity, _InputIterator first, _InputIterator last)
    {
        assign(buffer_capacity, first, last, sketch::is_integral<_InputIterator>());
    }

    size_type capacity() const noexcept
    {
        return m_end - m_buffer;
    }

    bool empty() const noexcept
    {
        return (size() == 0);
    }

    bool full() const noexcept
    {
        return (capacity() == size());
    }

    size_type size() const noexcept
    {
        return m_size;
    }

    size_type max_size() const
    {
        return m_allocator.max_size();
    }

    size_type reserve() const noexcept
    {
        return capacity() - size();
    }

    allocator_type get_allocator() const noexcept
    {
        return m_allocator;
    }

    allocator_type& get_allocator() noexcept
    {
        return m_allocator;
    }

    iterator begin() noexcept
    {
        return iterator(this, empty() ? 0 : m_first);
    }

    iterator end() noexcept
    {
        return iterator(this, 0);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(this, empty() ? 0 : m_first);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(this, 0);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    reference front()
    {
        SK_ASSERT(!empty());
        return *m_first;
    }

    reference back()
    {
        SK_ASSERT(!empty());
        return *((m_last == m_buffer ? m_end : m_last) - 1);
    }

    const_reference front() const
    {
        SK_ASSERT(!empty());
        return *m_first;
    }

    const_reference back() const
    {
        SK_ASSERT(!empty());
        return *((m_last == m_buffer ? m_end : m_last) - 1);
    }

    reference operator[](size_type index)
    {
        SK_ASSERT(index < size());
        return *add(m_first, index);
    }

    const_reference operator[](size_type index) const
    {
        SK_ASSERT(index < size());
        return *add(m_first, index);
    }

    array_range array_one()
    {
        return array_range(m_first, (m_last <= m_first && !empty() ? m_end : m_last) - m_first);
    }

    array_range array_two()
    {
        return array_range(m_buffer, m_last <= m_first && !empty() ? m_last - m_buffer : 0);
    }

    const_array_range array_one() const
    {
        return const_array_range(m_first, (m_last <= m_first && !empty() ? m_end : m_last) - m_first);
    }

    const_array_range array_two() const
    {
        return const_array_range(m_buffer, m_last <= m_first && !empty() ? m_last - m_buffer : 0);
    }

    bool is_linear() const noexcept
    {
        return ((m_first < m_last) || (m_last == m_buffer));
    }

    pointer linearize()
    {
        if (empty())
        {
            return 0;
        }

        if (m_first < m_last || m_last == m_buffer)
        {
            return m_first;
        }

        pointer src = m_first;
        pointer dest = m_buffer;
        size_type moved = 0;
        size_type constructed = 0;

        for (pointer first = m_first; dest < src; src = first)
        {
            for (size_type ii = 0; src < m_end; ++src, ++dest, ++moved, ++ii)
            {
                if (moved == size())
                {
                    first = dest;
                    break;
                }

                if (dest == first)
                {
                    first += ii;
                    break;
                }

                if (is_uninitialized(dest))
                {
                    ::new (dest) value_type(this_type::move_if_noexcept(*src));
                    ++constructed;
                }
                else
                {
                    value_type tmp = this_type::move_if_noexcept(*src);
                    replace(src, this_type::move_if_noexcept(*dest));
                    replace(dest, sketch::move(tmp));
                }
            }
        }

        for (src = m_end - constructed; src < m_end; ++src)
        {
            destroy_item(src);
        }

        m_first = m_buffer;
        m_last = add(m_buffer, size());

        return m_buffer;
    }

    void set_capacity(size_type new_capacity)
    {
        if (new_capacity == capacity())
        {
            return;
        }

        pointer buff = allocate(new_capacity);
        iterator b = begin();

        reset(buff, internal::_CircularBuffer_details::uninitialized_move_if_noexcept<value_type>(b, b + sketch::min(new_capacity, size()), buff), new_capacity);
    }

    void rset_capacity(size_type new_capacity)
    {
        if (new_capacity == capacity())
        {
            return;
        }

        pointer buff = allocate(new_capacity);
        iterator e = end();

        reset(buff, internal::_CircularBuffer_details::uninitialized_move_if_noexcept<value_type>(e - sketch::min(new_capacity, size()), e, buff), new_capacity);
    }

    void resize(size_type new_size, param_value_type item = value_type())
    {
        if (new_size > size())
        {
            if (new_size > capacity())
            {
                set_capacity(new_size);
            }

            insert(end(), new_size - size(), item);
        }
        else
        {
            iterator e = end();
            erase(e - (size() - new_size), e);
        }
    }

    void rresize(size_type new_size, param_value_type item = value_type())
    {
        if (new_size > size())
        {
            if (new_size > capacity())
            {
                set_capacity(new_size);
            }

            rinsert(begin(), new_size - size(), item);
        }
        else
        {
            rerase(begin(), end() - new_size);
        }
    }

    void clear() noexcept
    {
        destroy_content();
        m_size = 0;
    }

    void swap(CircularBuffer<_Ty, _Allocator>& cb) noexcept
    {
        swap_allocator(cb, sketch::is_stateless<allocator_type>());
        sketch::swap(m_buffer, cb.m_buffer);
        sketch::swap(m_end, cb.m_end);
        sketch::swap(m_first, cb.m_first);
        sketch::swap(m_last, cb.m_last);
        sketch::swap(m_size, cb.m_size);
    }

    void push_back(param_value_type item)
    {
        push_back_impl<param_value_type>(item);
    }

    void push_back(rvalue_type item)
    {
        push_back_impl<rvalue_type>(sketch::move(item));
    }

    void push_back()
    {
        value_type temp;
        push_back(sketch::move(temp));
    }

    void push_front(param_value_type item)
    {
        push_front_impl<param_value_type>(item);
    }

    void push_front(rvalue_type item)
    {
        push_front_impl<rvalue_type>(sketch::move(item));
    }

    void push_front()
    {
        value_type temp;
        push_front(sketch::move(temp));
    }

    void pop_back()
    {
        SK_ASSERT(!empty()); // check for empty buffer (back element not available)
        decrement(m_last);
        destroy_item(m_last);
        --m_size;
    }

    void pop_front()
    {
       SK_ASSERT(!empty()); // check for empty buffer (front element not available)
        destroy_item(m_first);
        increment(m_first);
        --m_size;
    }

    iterator insert(iterator pos, param_value_type item)
    {
        return insert_impl<param_value_type>(pos, item);
    }

    iterator insert(iterator pos, rvalue_type item)
    {
        return insert_impl<rvalue_type>(pos, sketch::move(item));
    }

    iterator insert(iterator pos)
    {
        value_type temp;
        return insert(pos, sketch::move(temp));
    }

    void insert(iterator pos, size_type n, param_value_type item)
    {
        if (n == 0)
        {
            return;
        }

        size_type copy = capacity() - (end() - pos);

        if (copy == 0)
        {
            return;
        }

        if (n > copy)
        {
            n = copy;
        }

        insert_n(pos, n, internal::_CircularBuffer_details::item_wrapper<const_pointer, param_value_type>(item));
    }

    template <class _InputIterator>
    void insert(iterator pos, _InputIterator first, _InputIterator last)
    {
        insert(pos, first, last, sketch::is_integral<_InputIterator>());
    }

    iterator rinsert(iterator pos, param_value_type item)
    {
        return rinsert_impl<param_value_type>(pos, item);
    }

    iterator rinsert(iterator pos, rvalue_type item)
    {
        return rinsert_impl<rvalue_type>(pos, sketch::move(item));
    }

    iterator rinsert(iterator pos)
    {
        value_type temp;
        return rinsert(pos, sketch::move(temp));
    }

    void rinsert(iterator pos, size_type n, param_value_type item)
    {
        rinsert_n(pos, n, internal::_CircularBuffer_details::item_wrapper<const_pointer, param_value_type>(item));
    }

    template <class _InputIterator>
    void rinsert(iterator pos, _InputIterator first, _InputIterator last)
    {
        rinsert(pos, first, last, sketch::is_integral<_InputIterator>());
    }

    iterator erase(iterator pos)
    {
        SK_ASSERT(pos.m_iterator != 0);      // check for iterator pointing to end()

        pointer next = pos.m_iterator;
        increment(next);
        for (pointer p = pos.m_iterator; next != m_last; p = next, increment(next))
        {
            replace(p, this_type::move_if_noexcept(*next));
        }

        decrement(m_last);
        destroy_item(m_last);
        --m_size;

        return m_last == pos.m_iterator ? end() : pos;
    }

    iterator erase(iterator first, iterator last)
    {
        SK_ASSERT(first <= last);        // check for wrong range

        if (first == last)
        {
            return first;
        }

        pointer p = first.m_iterator;
        while (last.m_iterator != 0)
        {
            replace((first++).m_iterator, this_type::move_if_noexcept(*last++));
        }

        do
        {
            decrement(m_last);
            destroy_item(m_last);
            --m_size;
        } while (m_last != first.m_iterator);

        return m_last == p ? end() : iterator(this, p);
    }

    iterator rerase(iterator pos)
    {
        SK_ASSERT(pos.m_iterator != 0);      // check for iterator pointing to end()

        pointer prev = pos.m_iterator;
        pointer p = prev;

        for (decrement(prev); p != m_first; p = prev, decrement(prev))
        {
            replace(p, this_type::move_if_noexcept(*prev));
        }

        destroy_item(m_first);
        increment(m_first);
        --m_size;

        return p == pos.m_iterator ? begin() : pos;
    }

    iterator rerase(iterator first, iterator last)
    {
        SK_ASSERT(first <= last);        // check for wrong range

        if (first == last)
        {
            return first;
        }

        pointer p = map_pointer(last.m_iterator);
        last.m_iterator = p;

        while (first.m_iterator != m_first)
        {
            decrement(first.m_iterator);
            decrement(p);
            replace(p, this_type::move_if_noexcept(*first.m_iterator));
        }

        do
        {
            destroy_item(m_first);
            increment(m_first);
            --m_size;
        } while (m_first != p);

        if (m_first == last.m_iterator)
        {
            return begin();
        }

        decrement(last.m_iterator);
        return iterator(this, last.m_iterator);
    }

    void erase_begin(size_type n)
    {
        SK_ASSERT(n <= size()); // check for n greater than size

        erase_begin(n, sketch::is_scalar<value_type>());
    }

    void erase_end(size_type n)
    {
        SK_ASSERT(n <= size()); // check for n greater than size

        erase_end(n, sketch::is_scalar<value_type>());
    }

private:
    template <class _Val>
    iterator rinsert_impl(iterator pos, _Val item)
    {
        if (full() && pos.m_iterator == 0)
        {
            return end();
        }

        if (pos == begin())
        {
            decrement(m_first);
            construct_or_replace(!full(), m_first, static_cast<_Val>(item));

            pos.m_iterator = m_first;
        }
        else
        {
            pointer src = m_first;
            pointer dest = m_first;
            decrement(dest);
            pos.m_iterator = map_pointer(pos.m_iterator);
            bool construct = !full();

            while (src != pos.m_iterator)
            {
                construct_or_replace(construct, dest, this_type::move_if_noexcept(*src));
                increment(src);
                increment(dest);
                construct = false;
            }

            decrement(pos.m_iterator);
            replace(pos.m_iterator, static_cast<_Val>(item));

            decrement(m_first);
        }

        if (full())
        {
            m_last = m_first;
        }
        else
        {
            ++m_size;
        }

        return iterator(this, pos.m_iterator);
    }

    template <class _Val>
    iterator insert_impl(iterator pos, _Val item)
    {
        iterator b = begin();
        if (full() && pos == b)
        {
            return b;
        }

        return insert_item<_Val>(pos, static_cast<_Val>(item));
    }


    template <class _Val>
    void push_back_impl(_Val item)
    {
        if (full())
        {
            if (empty())
            {
                SK_ASSERT(0);
                return;
            }

            replace(m_last, static_cast<_Val>(item));
            increment(m_last);
            m_first = m_last;
        }
        else
        {
            ::new (m_last) value_type(static_cast<_Val>(item));
            increment(m_last);
            ++m_size;
        }
    }

    template <class _Val>
    void push_front_impl(_Val item)
    {
        if (full())
        {
            if (empty())
            {
                return;
            }

            decrement(m_first);
            replace(m_first, static_cast<_Val>(item));
            m_last = m_first;
        }
        else
        {
            decrement(m_first);
            ::new (m_first) value_type(static_cast<_Val>(item));
            ++m_size;
        }
    }

    template <class _Pointer>
    void increment(_Pointer& p) const
    {
        if (++p == m_end)
        {
            p = m_buffer;
        }
    }

    template <class _Pointer>
    void decrement(_Pointer& p) const
    {
        if (p == m_buffer)
        {
            p = m_end;
        }

        --p;
    }

    template <class _Pointer>
    _Pointer add(_Pointer p, difference_type n) const
    {
        return p + (n < (m_end - p) ? n : n - capacity());
    }

    template <class _Pointer>
    _Pointer sub(_Pointer p, difference_type n) const
    {
        return p - (n > (p - m_buffer) ? n - capacity() : n);
    }

    pointer map_pointer(pointer p) const
    {
        return p == 0 ? m_last : p;
    }

    pointer allocate(size_type n)
    {
        return (n == 0) ? nullptr : m_allocator.allocate(n, 0);
    }

    void deallocate(pointer p, size_type n)
    {
        if (p)
        {
            m_allocator.deallocate(p, n);
        }
    }

    bool is_uninitialized(const_pointer p) const noexcept
    {
        return p >= m_last && (m_first < m_last || p < m_first);
    }

    void replace(pointer pos, param_value_type item)
    {
        *pos = item;
    }

    void replace(pointer pos, rvalue_type item)
    {
        *pos = sketch::move(item);
    }

    void construct_or_replace(bool construct, pointer pos, param_value_type item)
    {
        if (construct)
        {
            ::new (pos)value_type(item);
        }
        else
        {
            replace(pos, item);
        }
    }

    void construct_or_replace(bool construct, pointer pos, rvalue_type item)
    {
        if (construct)
        {
            ::new (pos) value_type(sketch::move(item));
        }
        else
        {
            replace(pos, sketch::move(item));
        }
    }

    void destroy_item(pointer p)
    {
        m_allocator.destroy(p);
    }

    void destroy_if_constructed(pointer pos)
    {
        if (is_uninitialized(pos))
        {
            destroy_item(pos);
        }
    }

    void destroy_content()
    {
        destroy_content(sketch::is_scalar<value_type>());
    }

    void destroy_content(const sketch::true_type&)
    {
        m_first = add(m_first, size());
    }

    void destroy_content(const sketch::false_type&)
    {
        for (size_type ii = 0; ii < size(); ++ii, increment(m_first))
        {
            destroy_item(m_first);
        }
    }

    void destroy() noexcept
    {
        destroy_content();
        deallocate(m_buffer, capacity());
    }

    void initialize_buffer(size_type buffer_capacity)
    {
        m_buffer = allocate(buffer_capacity);
        m_end = m_buffer + buffer_capacity;
    }

    void initialize_buffer(size_type buffer_capacity, param_value_type item)
    {
        initialize_buffer(buffer_capacity);

        internal::_CircularBuffer_details::uninitialized_fill_n_with_alloc(m_buffer, size(), item, m_allocator);
    }

    template <class _IntegralType>
    void initialize(_IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        m_size = static_cast<size_type>(n);
        initialize_buffer(size(), item);
        m_first = m_last = m_buffer;
    }

    template <class _Iterator>
    void initialize(_Iterator first, _Iterator last, const sketch::false_type&)
    {
        initialize(first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    template <class _ForwardIterator>
    void initialize(_ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range
        size_type distance = sketch::distance(first, last);
        initialize(distance, first, last, distance);
    }

    template <class _IntegralType>
    void initialize(size_type buffer_capacity, _IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        SK_ASSERT(size_type >= static_cast<size_type>(n)); // check for capacity lower than n
        m_size = static_cast<size_type>(n);
        initialize_buffer(buffer_capacity, item);
        m_first = m_buffer;
        m_last = (buffer_capacity == size()) ? m_buffer : (m_buffer + size());
    }

    template <class _Iterator>
    void initialize(size_type buffer_capacity, _Iterator first, _Iterator last, const sketch::false_type&)
    {
        initialize(buffer_capacity, first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    template <class _InputIterator>
    void initialize(size_type buffer_capacity, _InputIterator first, _InputIterator last, const sketch::input_iterator_tag&)
    {
        initialize_buffer(buffer_capacity);
        m_first = m_last = m_buffer;
        m_size = 0;
        if (buffer_capacity == 0)
        {
            return;
        }

        while (first != last && !full())
        {
            ::new (m_last) value_type(*first++);
            increment(m_last);
            ++m_size;
        }

        while (first != last)
        {
            replace(m_last, *first++);
            increment(m_last);
            m_first = m_last;
        }
    }

    template <class _ForwardIterator>
    void initialize(size_type buffer_capacity, _ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range
        initialize(buffer_capacity, first, last, sketch::distance(first, last));
    }

    template <class _ForwardIterator>
    void initialize(size_type buffer_capacity, _ForwardIterator first, _ForwardIterator last, size_type distance)
    {
        initialize_buffer(buffer_capacity);
        m_first = m_buffer;
        if (distance > buffer_capacity)
        {
            sketch::advance(first, distance - buffer_capacity);
            m_size = buffer_capacity;
        }
        else
        {
            m_size = distance;
        }

        m_last = internal::_CircularBuffer_details::uninitialized_copy<value_type>(first, last, m_buffer);

        if (m_last == m_end)
        {
            m_last = m_buffer;
        }
    }

    void reset(pointer buff, pointer last, size_type new_capacity)
    {
        destroy();
        m_size = last - buff;
        m_first = m_buffer = buff;
        m_end = m_buffer + new_capacity;
        m_last = last == m_end ? m_buffer : last;
    }

    void swap_allocator(CircularBuffer<_Ty, _Allocator>&, const sketch::true_type&)
    {
        // Swap is not needed because allocators have no state.
    }

    void swap_allocator(CircularBuffer<_Ty, _Allocator>& cb, const sketch::false_type&)
    {
        sketch::swap(m_allocator, cb.m_allocator);
    }

    template <class _IntegralType>
    void assign(_IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        assign(static_cast<size_type>(n), static_cast<value_type>(item));
    }

    //! Specialized assign method.
    template <class _Iterator>
    void assign(_Iterator first, _Iterator last, const sketch::false_type&)
    {
        assign(first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    template <class _ForwardIterator>
    void assign(_ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range
        size_type distance = sketch::distance(first, last);
        assign_n(distance, distance, internal::_CircularBuffer_details::make_assign_range<value_type>(first, last));
    }

    template <class _IntegralType>
    void assign(size_type new_capacity, _IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        assign(new_capacity, static_cast<size_type>(n), static_cast<value_type>(item));
    }

    template <class _Iterator>
    void assign(size_type new_capacity, _Iterator first, _Iterator last, const sketch::false_type&)
    {
        assign(new_capacity, first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    template <class _InputIterator>
    void assign(size_type new_capacity, _InputIterator first, _InputIterator last, const sketch::input_iterator_tag&)
    {
        if (new_capacity == capacity())
        {
            clear();
            insert(begin(), first, last);
        }
        else
        {
            CircularBuffer<value_type, allocator_type> tmp(new_capacity, first, last, m_allocator);
        }
    }

    template <class _ForwardIterator>
    void assign(size_type new_capacity, _ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range
        size_type distance = sketch::distance(first, last);
        if (distance > new_capacity)
        {
            sketch::advance(first, distance - new_capacity);
            distance = new_capacity;
        }

        assign_n(new_capacity, distance, internal::_CircularBuffer_details::make_assign_range<value_type>(first, last));
    }

    template <class _Functor>
    void assign_n(size_type new_capacity, size_type n, const _Functor& fnc)
    {
        if (new_capacity == capacity())
        {
            destroy_content();

            fnc(m_buffer);
        }
        else
        {
            pointer buff = allocate(new_capacity);

            fnc(buff);

            destroy();

            m_buffer = buff;
            m_end = m_buffer + new_capacity;
        }

        m_size = n;
        m_first = m_buffer;
        m_last = add(m_buffer, size());
    }

    template <class _Val>
    iterator insert_item(const iterator& pos, _Val item)
    {
        pointer p = pos.m_iterator;
        if (!p)
        {
            construct_or_replace(!full(), m_last, static_cast<_Val>(item));
            p = m_last;
        }
        else
        {
            pointer src = m_last;
            pointer dest = m_last;
            bool construct = !full();
            while (src != p)
            {
                decrement(src);
                construct_or_replace(construct, dest, this_type::move_if_noexcept(*src));
                decrement(dest);
                construct = false;
            }
            replace(p, static_cast<_Val>(item));
        }

        increment(m_last);

        if (full())
        {
            m_first = m_last;
        }
        else
        {
            ++m_size;
        }

        return iterator(this, p);
    }

    template <class _IntegralType>
    void insert(const iterator& pos, _IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        insert(pos, static_cast<size_type>(n), static_cast<value_type>(item));
    }

    template <class _Iterator>
    void insert(const iterator& pos, _Iterator first, _Iterator last, const sketch::false_type&)
    {
        insert(pos, first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    template <class _InputIterator>
    void insert(iterator pos, _InputIterator first, _InputIterator last, const sketch::input_iterator_tag&)
    {
        if (!full() || pos != begin())
        {
            for (; first != last; ++pos)
            {
                pos = insert(pos, *first++);
            }
        }
    }

    template <class _ForwardIterator>
    void insert(const iterator& pos, _ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range

        size_type n = sketch::distance(first, last);
        if (n == 0)
        {
            return;
        }

        size_type copy = capacity() - (end() - pos);
        if (copy == 0)
        {
            return;
        }

        if (n > copy)
        {
            sketch::advance(first, n - copy);
            n = copy;
        }

        insert_n(pos, n, internal::_CircularBuffer_details::iterator_wrapper<_ForwardIterator>(first));
    }

    template <class _Wrapper>
    void insert_n(const iterator& pos, size_type n, const _Wrapper& wrapper)
    {
        size_type construct = reserve();
        if (construct > n)
        {
            construct = n;
        }

        if (pos.m_iterator == 0)
        {
            size_type ii = 0;
            pointer p = m_last;

            for (; ii < construct; ++ii, increment(p))
            {
                ::new (p) value_type(*wrapper());
            }

            for (; ii < n; ++ii, increment(p))
            {
                replace(p, *wrapper());
            }
        }
        else
        {
            pointer src = m_last;
            pointer dest = add(m_last, n - 1);
            pointer p = pos.m_iterator;
            size_type ii = 0;

            while (src != pos.m_iterator)
            {
                decrement(src);
                construct_or_replace(is_uninitialized(dest), dest, *src);
                decrement(dest);
            }

            for (; ii < n; ++ii, increment(p))
            {
                construct_or_replace(is_uninitialized(p), p, *wrapper());
            }
        }

        m_last = add(m_last, n);
        m_first = add(m_first, n - construct);
        m_size += construct;
    }

    template <class _IntegralType>
    void rinsert(const iterator& pos, _IntegralType n, _IntegralType item, const sketch::true_type&)
    {
        rinsert(pos, static_cast<size_type>(n), static_cast<value_type>(item));
    }

    //! Specialized rinsert method.
    template <class _Iterator>
    void rinsert(const iterator& pos, _Iterator first, _Iterator last, const sketch::false_type&)
    {
        rinsert(pos, first, last, typename sketch::iterator_category<_Iterator>::type());
    }

    //! Specialized insert method.
    template <class _InputIterator>
    void rinsert(iterator pos, _InputIterator first, _InputIterator last, const sketch::input_iterator_tag&)
    {
        if (!full() || pos.m_iterator != 0)
        {
            for (; first != last; ++pos)
            {
                pos = rinsert(pos, *first++);
                if (pos.m_iterator == 0)
                {
                    break;
                }
            }
        }
    }

    template <class _ForwardIterator>
    void rinsert(const iterator& pos, _ForwardIterator first, _ForwardIterator last, const sketch::forward_iterator_tag&)
    {
        SK_ASSERT(sketch::distance(first, last) >= 0); // check for wrong range
        rinsert_n(pos, sketch::distance(first, last), internal::_CircularBuffer_details::iterator_wrapper<_ForwardIterator>(first));
    }

    template <class _Wrapper>
    void rinsert_n(const iterator& pos, size_type n, const _Wrapper& wrapper)
    {
        if (n == 0)
        {
            return;
        }

        iterator b = begin();
        size_type copy = capacity() - (pos - b);

        if (copy == 0)
        {
            return;
        }

        if (n > copy)
        {
            n = copy;
        }

        size_type construct = reserve();
        if (construct > n)
        {
            construct = n;
        }

        if (pos == b)
        {
            pointer p = sub(m_first, n);
            size_type ii = n;

            for (; ii > construct; --ii, increment(p))
            {
                replace(p, *wrapper());
            }

            for (; ii > 0; --ii, increment(p))
            {
                ::new (p) value_type(*wrapper());
            }
        }
        else
        {
            pointer src = m_first;
            pointer dest = sub(m_first, n);
            pointer p = map_pointer(pos.m_iterator);

            while (src != p)
            {
                construct_or_replace(is_uninitialized(dest), dest, *src);
                increment(src);
                increment(dest);
            }

            for (size_type ii = 0; ii < n; ++ii, increment(dest))
            {
                construct_or_replace(is_uninitialized(dest), dest, *wrapper());
            }
        }

        m_first = sub(m_first, n);
        m_last = sub(m_last, n - construct);
        m_size += construct;
    }

    void erase_begin(size_type n, const sketch::true_type&)
    {
        m_first = add(m_first, n);
        m_size -= n;
    }

    void erase_begin(size_type n, const sketch::false_type&)
    {
        iterator b = begin();
        rerase(b, b + n);
    }

    void erase_end(size_type n, const sketch::true_type&)
    {
        m_last = sub(m_last, n);
        m_size -= n;
    }

    void erase_end(size_type n, const sketch::false_type&)
    {
        iterator e = end();
        erase(e - n, e);
    }

private:
    pointer m_buffer { nullptr };
    pointer m_end { nullptr }; // end of the storage space

    pointer m_first { nullptr }; // first item
    pointer m_last { nullptr }; // one behind the last element

    size_type m_size { 0 }; // current number of elements

    allocator_type m_allocator;
};


template <class _Ty, class _Allocator>
inline void swap(CircularBuffer<_Ty, _Allocator>& lhs, CircularBuffer<_Ty, _Allocator>& rhs) noexcept
{
    lhs.swap(rhs);
}



} // namespace sketch {}
