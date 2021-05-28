#pragma once

#include "functional.hxx"
#include "Iterator.hxx"
#include "Pair.hxx"


namespace sketch
{

namespace _HashMap_details
{

namespace __
{

template<class _Key, class _Value, bool _IsMap>
struct container_policy;

template<class _Key, class _Value>
struct container_policy<_Key, _Value, true>
{
    typedef Pair<const _Key, _Value> value_type;
    typedef _Key key_type;
    typedef _Value mapped_type;
};

template<class _Key, class _Value>
struct container_policy<_Key, _Value, false>
{
    typedef _Value value_type;
    typedef _Key key_type;
    typedef _Value mapped_type;
};


} // namespace __ {}


template<
    class _Key,
    class _Value,
    class _Hash = sketch::hash<_Key>,
    class _Pred = sketch::equal_to<_Key>,
    class _Allocator = sketch::allocator<sketch::Pair<const _Key, _Value>>,
    bool _IsMap = true,
    bool _IsUnique = true
>
class chained_hashtable;

template<class _Key, class _Value, class _Hash, class _Pred, class _Allocator, bool _IsMap, bool _IsUnique>
class chained_hashtable
    : public __::container_policy<_Key, _Value, _IsMap>
{
    typedef chained_hashtable hashtable;
    typedef __::container_policy<_Key, _Value, _IsMap> policy;

    typedef integral_constant<bool, _IsMap> is_map;
    typedef integral_constant<bool, _IsUnique> is_unique;

    typedef typename _Allocator::template rebind<typename policy::value_type>::other allocator;

public:
    static const typename allocator::size_type initial_count = 8;

    typedef typename policy::value_type value_type;
    typedef typename policy::key_type key_type;

    typedef _Hash hasher;
    typedef _Pred key_equal;
    typedef _Allocator allocator_type;

    typedef typename allocator::pointer pointer;
    typedef typename allocator::const_pointer const_pointer;
    typedef typename allocator::reference reference;
    typedef typename allocator::const_reference const_reference;
    typedef typename allocator::size_type size_type;
    typedef typename allocator::difference_type difference_type;

protected:
    typedef size_t hash_t;

    struct node;
    typedef node double_linked;

    struct node
    {
        double_linked* prev;
        double_linked* next;

        hash_t hkey;
        value_type elem;

        node(const value_type& elem, hash_t h)
            : elem(elem)
            , hkey(h)
            , next()
            , prev()
        {
        }

        node(value_type&& elem, hash_t h)
            : elem(sketch::forward<value_type>(elem))
            , hkey(h)
            , next()
            , prev()
        {
        }

        node(node&& x)
            : elem(sketch::move(x.elem))
            , hkey(x.hkey)
            , next(x.next)
            , prev(x.prev)
        {
            x.hkey = 0;
            x.prev = x.next = nullptr;
        }

    protected:
        node(const node& n)
            : elem(n.elem)
            , hkey(n.hkey)
            , next(n.next)
            , prev(n.prev)
        {
        }

    private:
        node& operator=(const node&);

    public:
        void link(double_linked* _prev, double_linked* _next)
        {
            this->prev = _prev;
            this->next = _next;
            _prev->next = this;
            _next->prev = this;
        }

        void link_next(double_linked* _next)
        {
            _next->next = this->next;
            _next->prev = this;
            this->next = _next;
        }

        void link_prev(double_linked* _prev)
        {
            _prev->next = this;
            _prev->prev = this->prev;
            this->prev = _prev;
        }

        void unlink()
        {
            if (prev) prev->next = next;
            if (next) next->prev = prev;
        }
    };

    typedef node node_type;

    struct bucket_type
    {
        node* elems;
        size_type size;
        hash_t hash;
        bool dirty;
    };

    typedef Pair<bucket_type*, bucket_type*> table;

    typedef typename allocator_type::template rebind<node_type>::other node_allocator;
    typedef typename allocator_type::template rebind<bucket_type>::other bucket_allocator;

    struct base_iterator
    {
        node_type* p;
        bucket_type* b;
        bucket_type* be;

        void increment()
        {
            if (!p->next)
            {
                // end of current bucket
                // find next nonempty bucket
                while (++b != be && !b->elems);

                p = (b == be) ? 0 : b->elems;
            }
            else
            {
                p = p->next;
            }
        }

        void decrement()
        {
        }
    };

    struct base_local_iterator
    {
        node_type *p;

        void increment()
        {
            p = p->next;
        }

        void decrement()
        {
        }
    };

    struct iterator_impl
        : sketch::Iterator<forward_iterator_tag, value_type, difference_type, pointer, reference>
        , base_iterator
    {
        iterator_impl()
        {
            p = nullptr;
        }

        iterator_impl(double_linked* p, bucket_type* b, bucket_type* end)
        {
            this->p = p;
            this->b = b;
            this->be = end;
        }

        iterator_impl(bucket_type* b, bucket_type* end)
        {
            this->p = b ? b->elems : nullptr;
            this->b = b;
            this->be = end;
        }

        reference operator*() const
        {
            return p->elem;
        }

        pointer operator->() const
        {
            return &p->elem;
        }

        iterator_impl & operator++()
        {
            increment();
            return *this;
        }

        iterator_impl operator++(int)
        {
            iterator_impl tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator==(const iterator_impl& x, const iterator_impl& y)
        {
            return x.p == y.p;
        }

        friend bool operator!=(const iterator_impl& x, const iterator_impl& y)
        {
            return x.p != y.p;
        }

    private:
        friend class hashtable;
        friend struct const_iterator_impl;
    };

    struct const_iterator_impl
        : sketch::Iterator<forward_iterator_tag, const value_type, difference_type, const_pointer, const_reference>
        , base_iterator
    {
        const_iterator_impl()
        {
            p = nullptr;
        }

        const_iterator_impl(const iterator_impl& i)
        {
            p = i.p;
            b = i.b;
            be = i.be;
        }

        const_iterator_impl(double_linked* p, bucket_type* b, bucket_type* end)
        {
            this->p = p;
            this->b = b;
            this->be = end;
        }

        const_reference operator*() const
        {
            return p->elem;
        }

        const_pointer operator->() const
        {
            return &p->elem;
        }

        const_iterator_impl& operator++()
        {
            increment();
            return *this;
        }

        const_iterator_impl operator++(int)
        {
            const_iterator_impl tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator==(const const_iterator_impl& x, const const_iterator_impl& y)
        {
            return x.p == y.p;
        }

        friend bool operator!=(const const_iterator_impl& x, const const_iterator_impl& y)
        {
            return x.p != y.p;
        }

    private:
        friend class hashtable;
    };

    struct local_iterator_impl
        : sketch::Iterator<forward_iterator_tag, value_type, difference_type, pointer, reference>
        , base_local_iterator
    {
        local_iterator_impl()
        {
            p = nullptr;
        }

        local_iterator_impl(double_linked* p)
        {
            this->p = p;
        }

        reference operator*() const
        {
            return p->elem;
        }

        pointer operator->() const
        {
            return &p->elem;
        }

        local_iterator_impl & operator++()
        {
            increment();
            return *this;
        }

        local_iterator_impl operator++(int)
        {
            local_iterator_impl tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator==(const local_iterator_impl& x, const local_iterator_impl& y)
        {
            return x.p == y.p;
        }

        friend bool operator!=(const local_iterator_impl& x, const local_iterator_impl& y)
        {
            return x.p != y.p;
        }

    private:
        friend class hashtable;
        friend struct const_local_iterator_impl;
    };

    struct const_local_iterator_impl
        : sketch::Iterator<bidirectional_iterator_tag, const value_type, difference_type, const_pointer, const_reference>
        , base_local_iterator
    {
        const_local_iterator_impl()
        {
            p = nullptr;
        }

        const_local_iterator_impl(const local_iterator_impl& i)
        {
            p = i.p;
        }

        const_local_iterator_impl(const double_linked* p)
        {
            this->p = const_cast<node_type*>(p);
        }

        const_reference operator*() const
        {
            return p->elem;
        }

        const_pointer operator->() const
        {
            return &p->elem;
        }

        const_local_iterator_impl& operator++()
        {
            increment();
            return *this;
        }

        const_local_iterator_impl& operator--()
        {
            decrement();
            return *this;
        }

        const_local_iterator_impl operator++(int)
        {
            const_local_iterator_impl tmp(*this);
            ++*this;
            return tmp;
        }

        const_local_iterator_impl operator--(int)
        {
            const_local_iterator_impl tmp(*this);
            --*this;
            return tmp;
        }

        friend bool operator==(const const_local_iterator_impl& x, const const_local_iterator_impl& y)
        {
            return x.p == y.p;
        }

        friend bool operator!=(const const_local_iterator_impl& x, const const_local_iterator_impl& y)
        {
            return x.p != y.p;
        }

    private:
        friend class hashtable;
    };

public:
    typedef iterator_impl iterator;
    typedef const_iterator_impl const_iterator;
    typedef sketch::ReverseIterator<iterator> ReverseIterator;
    typedef sketch::ReverseIterator<const_iterator> const_reverse_iterator;

    typedef local_iterator_impl local_iterator;
    typedef const_local_iterator_impl const_local_iterator;

public:
    ~chained_hashtable()
    {
        clear();

        if (m_buckets.first)
        {
            m_bucketAllocator.deallocate(m_buckets.first);
        }
    }

    explicit chained_hashtable(
        size_type n,
        const hasher& hf = hasher(),
        const key_equal& eql = key_equal(),
        const allocator_type& a = allocator_type()
    )
        : m_nodeAllocator(a)
        , m_bucketAllocator(a)
        , m_hash(hf)
        , m_equal(eql)
        , m_count(0)
        , m_head()
    {
        init_table(n);
    }

    chained_hashtable(const chained_hashtable& r)
        : m_nodeAllocator(r.m_nodeAllocator)
        , m_bucketAllocator(r.m_bucketAllocator)
        , m_hash(r.m_hash)
        , m_equal(r.m_equal)
        , m_count(0)
        , m_head()
    {
        if (r.size())
            copy_from(r.m_buckets);
        else
            init_table(initial_count);
    }

    chained_hashtable(const chained_hashtable& r, const allocator_type& a)
        : m_nodeAllocator(a)
        , m_bucketAllocator(a)
        , m_hash(r.m_hash)
        , m_equal(r.m_equal)
        , m_count(0)
        , m_maxFactor(r.m_maxFactor)
        , m_head()
    {
        if (r.size())
            copy_from(r.m_buckets);
        else
            init_table(initial_count);
    }

    chained_hashtable& operator=(const chained_hashtable& r)
    {
        if (this != &r)
            chained_hashtable(r).swap(*this);

        return *this;
    }

    chained_hashtable(chained_hashtable&& r)
        : m_nodeAllocator(sketch::move(r.m_nodeAllocator))
        , m_bucketAllocator(sketch::move(r.m_bucketAllocator))
        , m_hash(sketch::move(r.m_hash))
        , m_equal(sketch::move(r.m_equal))
        , m_buckets(sketch::move(r.m_buckets))
        , m_count(r.m_count)
        , m_maxFactor(r.m_maxFactor)
        , m_head(sketch::move(r.m_head))
    {
        r.m_count = 0;
        r.m_buckets = table();
    }

    chained_hashtable(chained_hashtable&& r, const allocator_type& a)
        : m_nodeAllocator(a)
        , m_bucketAllocator(a)
        , m_hash(r.m_hash)
        , m_equal(r.m_equal)
        , m_count(0)
        , m_maxFactor(r.m_maxFactor)
        , m_head()
    {
        if (r.m_nodeAllocator == m_nodeAllocator)
        {
            swap(r);
        }
        else
        {
            if (r.size())
                copy_from(r.m_buckets);
            else
                init_table(initial_count);

            r.clear();
        }
    }

    chained_hashtable& operator=(chained_hashtable&& r)
    {
        if (this != &r)
            chained_hashtable(r).swap(*this);

        return *this;
    }

    bool empty() const
    {
        return m_count == 0;
    }

    size_type size() const
    {
        return m_count;
    }

    size_type max_size() const
    {
        return m_nodeAllocator.max_size();
    }

    iterator begin()
    {
        return m_count == 0 ? iterator() : iterator(m_head, m_buckets.second);
    }

    const_iterator begin() const
    {
        return const_cast<hashtable*>(this)->begin();
    }

    const_iterator cbegin() const
    {
        return const_cast<hashtable*>(this)->begin();
    }

    iterator end()
    {
        return iterator();
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    const_iterator cend() const
    {
        return const_iterator();
    }

    sketch::Pair<iterator, bool> insert(const value_type& v)
    {
        return insert_impl(cend(), v);
    }

    iterator insert(const_iterator hint, const value_type& v)
    {
        return insert_impl(hint, v).first;
    }

protected:
    sketch::Pair<iterator, bool> insert_impl(const_iterator hint, const value_type& v)
    {
        const hash_t hkey = m_hash(value2key(v, is_map()));
        const size_type n = mapkey(hkey);
        SK_ASSERT(m_buckets.first && n < static_cast<size_type>(m_buckets.second - m_buckets.first));
        bucket_type& b = m_buckets.first[n];

        if (is_unique::value && b.elems)
        {
            // allow only unique keys
            if (b.hash == hkey || b.dirty)
            {
                for (const node_type* p = b.elems; p; p = p->next)
                {
                    if (p->hkey == hkey && m_equal(value2key(p->elem, is_map()), value2key(v, is_map())))
                    {
                        return sketch::make_pair(iterator(const_cast<node_type*>(p), &b, m_buckets.second), false);
                    }
                }
            }
        }

        node* p = m_nodeAllocator.allocate(1);
        m_nodeAllocator.construct(p, v, hkey);

        if (hint.p && hint.p->hkey == hkey)
        {
            // link with the hint
            hint = hint;
        }

        {
            if (b.elems) {
                const bool to_end = b.hash != hkey;
                if (!to_end)
                {
                    b.elems->link_prev(p);
                }
                else
                {
                    node_type* no = b.elems;
                    while (no->next && no->hkey == hkey)
                    {
                        no = no->next;
                    }

                    no->link_next(p);
                    b.dirty = true;
                }
            }
            else
            {
                // construct bucket
                b.elems = p;
                b.hash = hkey;
                if (m_count == 0 || m_head > &b)
                {
                    m_head = &b;
                }
            }

            b.size++;
        }

        m_count++;

        return sketch::make_pair(iterator(p, &b, m_buckets.second), true);
    }

public:
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        for (iterator i = end(); first != last; ++first)
        {
            i = insert(i, *first);
        }
    }

    iterator erase(const_iterator position)
    {
        if (!position.p)
        {
            return end();
        }

        node_type* next = position.p->next;
        if (position.b->elems == position.p)
        {
            bucket_type* b = position.b;
            const bool head = m_head == b;
            b->elems = next;
            if (!--b->size || (b->dirty && b->hash != next->hkey))
            {
                b->dirty = false;
                if (next)
                {
                    b->hash = next->hkey;
                }
            }

            if (!next || head)
            {
                // find next nonempty bucket for head
                b = m_buckets.first;

                while (b != m_buckets.second && !b->elems)
                {
                    ++b;
                }

                if (b != m_buckets.second)
                {
                    if (head)
                    {
                        m_head = b;
                    }

                    position.b = b;
                    next = b->elems;
                }
                else if (head)
                {
                    m_head = nullptr;
                }
            }
        }
        else
        {
            --position.b->size;
        }

        --m_count;
        position.p->unlink();
        m_nodeAllocator.destroy(position.p);
        m_nodeAllocator.deallocate(position.p, 1);

        return iterator(next, position.b, position.be);
    }

    size_type erase(const key_type& k)
    {
        const_iterator i = find(k);
        if (i == cend())
        {
            return 0;
        }

        if (is_unique::value)
        {
            erase(i);
            return 1;
        }

        Pair<iterator, iterator> range = equal_range(k);
        size_type n = distance(range.first, range.second);
        while (range.first != range.second)
        {
            erase(range.first);
        }

        return n;
    }

    iterator  erase(const_iterator first, const_iterator last)
    {
        while (first != last)
            first = erase(first);
        return iterator(last.p, last.b, m_buckets.second);
    }

    void clear()
    {
        erase(cbegin(), cend());
    }

    void swap(hashtable& x)
    {
        if (this == &x)
        {
            return;
        }

        using sketch::swap;
        swap(m_head, x.m_head);
        swap(m_buckets, x.m_buckets);
        swap(m_nodeAllocator, x.m_nodeAllocator);
        swap(m_bucketAllocator, x.m_bucketAllocator);
        swap(m_hash, x.m_hash);
        swap(m_equal, x.m_equal);
        swap(m_count, x.m_count);
        swap(m_maxFactor, x.m_maxFactor);
    }

    hasher hash_function() const
    {
        return m_hash;
    }

    key_equal key_eq() const
    {
        return m_equal;
    }

    iterator find(const key_type& k)
    {
        const size_type n = bucket(k);
        bucket_type& b = m_buckets.first[n];
        if (b.size == 0)
        {
            return end();
        }

        for (local_iterator li = begin(n), lend = end(n); li != lend; ++li)
        {
            if (m_equal(k, value2key(li.p->elem, is_map())))
            {
                return iterator(li.p, &b, m_buckets.second);
            }
        }

        return end();
    }

    const_iterator find(const key_type& k) const
    {
        return const_cast<hashtable*>(this)->find(k);
    }

    size_type count(const key_type& k) const
    {
        return find(k) != end() ? 1 : 0;
    }

    sketch::Pair<iterator, iterator> equal_range(const key_type& k)
    {
        const hash_t hkey = m_hash(k);
        const size_type n = mapkey(hkey);
        bucket_type& b = m_buckets.first[n];
        iterator e = end();
        if (!b.size)
        {
            return make_pair(e, e);
        }

        node_type* p = b.elems;
        while (p && p->hkey != hkey)
        {
            p = p->next;
        }

        if (!p)
        {
            return make_pair(e, e);
        }

        e.p = p;
        e.b = &b;
        e.be = m_buckets.second;
        iterator i = e;
        if (e.b->dirty)
        {
            while (e.p && e.p->hkey == i.p->hkey && m_equal(value2key(e.p->elem, is_map()), value2key(i.p->elem, is_map())))
            {
                e.p = e.p->next;
            }
        }
        else
        {
            e.p = nullptr;
        }

        return make_pair(i, e);
    }

    sketch::Pair<const_iterator, const_iterator> equal_range(const key_type& k) const
    {
        return const_cast<hashtable*>(this)->equal_range(k);
    }

    size_type bucket_count() const
    {
        return m_buckets.second - m_buckets.first;
    }

    size_type max_bucket_count() const
    {
        return m_bucketAllocator.max_size();
    }

    size_type bucket_size(size_type n) const
    {
        SK_ASSERT(n >= 0 && n < bucket_count());

        bucket_type& b = m_buckets.first[n];
        if (!b.dirty)
        {
            n = b.size;
        }
        else
        {
            n = 0;
            const hash_t hkey = b.hash;

            for (node_type* p = b.elems; p && p->hkey == hkey; p = p->next, ++n);
        }

        return n;
    }

    size_type bucket(const key_type& k) const
    {
        return mapkey(m_hash(k));
    }

    local_iterator begin(size_type n)
    {
        SK_ASSERT(n >= 0 && n < bucket_count());
        return m_buckets.first[n].elems;
    }

    const_local_iterator begin(size_type n) const
    {
        SK_ASSERT(n >= 0 && n < bucket_count());
        return m_buckets.first[n].elems;
    }

    const_local_iterator cbegin(size_type n) const
    {
        return begin(n);
    }

    local_iterator end(size_type n)
    {
        SK_UNUSED(n);
        SK_ASSERT(n >= 0 && n < bucket_count());
        return local_iterator(nullptr);
    }

    const_local_iterator end(size_type n) const
    {
        SK_UNUSED(n);
        SK_ASSERT(n >= 0 && n < bucket_count());
        return const_local_iterator(nullptr);
    }

    const_local_iterator cend(size_type n) const
    {
        return end(n);
    }

    void rehash(size_type n)
    {
        if (bucket_count() >= n)
        {
            return;
        }

        n = capacity_factor(n);

        // init the new bucket table
        bucket_type* b = m_bucketAllocator.allocate(n);
        table buckets(b, b + n);
        memset(b, 0, sizeof(bucket_type)*n);

        // swap it with old
        sketch::swap(buckets, m_buckets);
        m_head = nullptr;
        m_count = 0; // increased by following inserts
        copy_from(buckets);

        // destroy old buckets
        b = buckets.first;
        while (b != buckets.second)
        {
            if (!b->elems || b->size == 0)
            {
                b++;
                continue;
            }

            node* p = b->elems;
            while (b->size--)
            {
                node* d = p;
                p = p->next;
                m_nodeAllocator.deallocate(d, 1);
            }

            b->elems = nullptr;
            b++;
        }

        m_bucketAllocator.deallocate(buckets.first, buckets.second - buckets.first);
    }

protected:
    void init_table(size_type n)
    {
        // init table
        n = capacity_factor(n);
        m_buckets.first = m_bucketAllocator.allocate(n);
        m_buckets.second = m_buckets.first + n;
        memset(m_buckets.first, 0, sizeof(bucket_type)*n);
    }

    void copy_from(const table& buckets)
    {
        const size_type rcount = buckets.second - buckets.first;
        if (bucket_count() < rcount)
        {
            init_table(rcount);
        }

        bucket_type* b = buckets.first;

        // iterate through old table
        const_iterator hint = end();
        while (b != buckets.second)
        {
            if (b->elems)
            {
                node_type* p = b->elems;
                while (p)
                {
                    hint = insert(hint, sketch::move(p->elem));
                    p = p->next;
                }
            }

            ++b;
        }
    }

    template<bool> node_type* move_element(node_type* to, const value_type& v, true_type)
    {
        to->elem = v;
        return to;
    }

    template<bool> node_type* move_element(node_type* to, const value_type& v, false_type)
    {
        node_type* p = m_nodeAllocator.allocate(1);
        m_nodeAllocator.construct(p, v, to->hkey);
        p->next = to->next; p->prev = to->prev;
        m_nodeAllocator.destroy(to);
        m_nodeAllocator.deallocate(to, 1);

        return p;
    }

    template<class V> static const key_type& value2key(const V& x, true_type)
    {
        return x.first;
    }

    template<class V> static const key_type& value2key(const V& x, false_type)
    {
        return x;
    }

protected:
    size_type mapkey(hash_t h) const
    {
        const size_type max = m_buckets.second - m_buckets.first;
        const size_type mask = max - 1;
        size_type n = h & mask;
        if (n >= max)
        {
            n -= mask / 2 + 1;
        }

        return n;
    }

    size_type capacity_factor(size_type n) const
    {
        return static_cast<size_type>(n * 2 - 1);
    }

protected:
    bucket_type* m_head;
    table m_buckets;

    size_type m_count;

    hasher m_hash;
    key_equal m_equal;

    node_allocator m_nodeAllocator;
    bucket_allocator m_bucketAllocator;
};


} // _HashMap_details {}

} // namespace sketch {}
