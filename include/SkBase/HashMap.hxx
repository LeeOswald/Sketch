#pragma once

#include "HashMap_details.hxx"


namespace sketch
{

template <
    class _Key,
    class _Ty,
    class _Hash = hash<_Key>,
    class _Pred = sketch::equal_to<_Key>,
    class _Allocator = sketch::Allocator<sketch::Pair<const _Key, _Ty>>
    >
class HashMap;



template <class _Key, class _Ty, class _Hash, class _Pred, class _Allocator>
class HashMap
    : public sketch::_HashMap_details::chained_hashtable<_Key, _Ty, _Hash, _Pred, _Allocator, true, true>
{
    typedef sketch::_HashMap_details::chained_hashtable<_Key, _Ty, _Hash, _Pred, _Allocator, true, true> base;

public:
    typedef _Key key_type;
    typedef sketch::Pair<const _Key, _Ty> value_type;
    typedef _Ty mapped_type;

    typedef _Hash hasher;
    typedef _Pred key_equal;
    typedef _Allocator allocator_type;

public:
    using base::insert;

    ~HashMap()
    {
    }

    explicit HashMap(
        size_type n = initial_count,
        const hasher& hf = hasher(),
        const key_equal& eql = key_equal(),
        const allocator_type& a = allocator_type()
        )
        : base(n, hf, eql, a)
    {
    }

    template <class _InputIterator>
    HashMap(
        _InputIterator first,
        _InputIterator last,
        size_type n = initial_count,
        const hasher& hf = hasher(),
        const key_equal& eql = key_equal(),
        const allocator_type& a = allocator_type()
        )
        : base(n, hf, eql, a)
    {
        insert(first, last);
    }

    HashMap(const HashMap& r)
        : base(static_cast<const base&>(r))
    {
    }

    HashMap(const _Allocator& a)
        : base(initial_count, hasher(), key_equal(), a)
    {
    }

    HashMap(const HashMap& r, const _Allocator& a)
        : base(r, a)
    {
    }

    HashMap(HashMap&& r)
        : base(forward<base>(r))
    {
    }

    HashMap(HashMap&& r, const _Allocator& a)
        : base(forward<base>(r), a)
    {
    }

    HashMap& operator=(HashMap&& r)
    {
        base::operator=(r);
        return *this;
    }

    HashMap& operator=(const HashMap& r)
    {
        base::operator=(r);
        return *this;
    }

    allocator_type get_allocator() const
    {
        return allocator_type(m_nodeAllocator);
    }

    mapped_type& operator[](const key_type& k)
    {
        iterator i = find(k);
        if (i == end())
        {
            i = insert(value_type(k, mapped_type())).first;
        }

        return i->second;
    }
};



} // namespace sketch {}
