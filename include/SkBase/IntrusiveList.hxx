#pragma  once

#include "../Sketch.hxx"

#include "Iterator.hxx"


namespace sketch
{

template <class _Ty>
struct ListNode
    : public NonCopyable
{
    inline ListNode() noexcept
        : next(nullptr)
        , prev(nullptr)
    {
    }

    inline ListNode(ListNode* _next, ListNode* _prev) noexcept
        : next(_next)
        , prev(_prev)
    {
    }

    ListNode* next;
    ListNode* prev;
};


template <class _Ty, ListNode<_Ty> _Ty::*NODE>
class IntrusiveList
    : public NonCopyable
{
public:
    typedef _Ty value_type;
    typedef _Ty* pointer;
    typedef const _Ty* const_pointer;
    typedef _Ty& reference;
    typedef const _Ty& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef ListNode<_Ty> node_type;

    struct iterator
        : public sketch::Iterator<bidirectional_iterator_tag, value_type, difference_type, pointer, reference>
    {
        iterator(pointer p = 0) noexcept
            : m_p(p)
        {
        }

        iterator(node_type* node) noexcept
            : m_p(offset_pointer<_Ty>(node, -ptrdiff_t(&(static_cast<_Ty*>(0)->*NODE))))
        {
        }

        void increment() noexcept
        {
            SK_ASSERT(m_p);
            node_type* node = &(m_p->*NODE);
            *this = iterator(node->next);
        }

        void decrement() noexcept
        {
            SK_ASSERT(m_p);
            node_type* node = &(m_p->*NODE);
            *this = iterator(node->prev);
        }

        reference operator*() noexcept
        {
            SK_ASSERT(m_p);
            return *m_p;
        }

        pointer operator->() noexcept
        {
            SK_ASSERT(m_p);
            return m_p;
        }

        iterator& operator++() noexcept
        {
            increment();
            return *this;
        }

        iterator operator++(int) noexcept
        {
            iterator tmp(*this);
            ++*this;
            return tmp;
        }

        iterator& operator--() noexcept
        {
            decrement();
            return *this;
        }

        iterator operator--(int) noexcept
        {
            iterator tmp(*this);
            --*this;
            return tmp;
        }

        friend bool operator==(const iterator& x, const iterator& y) noexcept
        {
            return x.m_p == y.m_p;
        }

        friend bool operator!=(const iterator& x, const iterator& y) noexcept
        {
            return x.m_p != y.m_p;
        }

        pointer m_p;
    };

    struct const_iterator
        : public sketch::Iterator<bidirectional_iterator_tag, const value_type, difference_type, const_pointer, const_reference>
    {
        const_iterator(const_pointer p = 0) noexcept
            : m_p(p)
        {
        }

        const_iterator(node_type const* node) noexcept
            : m_p(offset_pointer<_Ty>(node, -ptrdiff_t(&(static_cast<_Ty const*>(0)->*NODE))))
        {
        }

        void increment() noexcept
        {
            SK_ASSERT(m_p);
            node_type const* node = &(m_p->*NODE);
            *this = const_iterator(node->next);
        }

        void decrement() noexcept
        {
            SK_ASSERT(m_p);
            node_type* node = &(m_p->*NODE);
            *this = const_iterator(node->prev);
        }

        reference operator*() noexcept
        {
            SK_ASSERT(m_p);
            return *m_p;
        }

        pointer operator->() noexcept
        {
            SK_ASSERT(m_p);
            return m_p;
        }

        const_iterator& operator++() noexcept
        {
            increment();
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            const_iterator tmp(*this);
            ++*this;
            return tmp;
        }

        const_iterator& operator--() noexcept
        {
            decrement();
            return *this;
        }

        const_iterator operator--(int) noexcept
        {
            const_iterator tmp(*this);
            --*this;
            return tmp;
        }

        friend bool operator==(const const_iterator& x, const const_iterator& y) noexcept
        {
            return x.m_p == y.m_p;
        }

        friend bool operator!=(const const_iterator& x, const const_iterator& y) noexcept
        {
            return x.m_p != y.m_p;
        }

        pointer m_p;
    };

    typedef sketch::ReverseIterator<iterator> reverse_iterator;
    typedef sketch::ReverseIterator<const_iterator> const_reverse_iterator;

public:
    inline ~IntrusiveList() noexcept
    {
    }

    inline IntrusiveList() noexcept
        : m_root(&m_root, &m_root)
        , m_size(0)
    {
    }

    inline bool empty() const noexcept
    {
        return (m_size == 0);
    }

    inline size_type size() const noexcept
    {
        return m_size;
    }

    inline iterator begin() noexcept
    {
        return iterator(m_root.next);
    }

    inline const_iterator begin() const noexcept
    {
        return const_iterator(m_root.next);
    }

    inline iterator end() noexcept
    {
        return iterator(&m_root);
    }

    inline const_iterator end() const noexcept
    {
        return const_iterator(&m_root);
    }

    inline const_iterator cbegin() const noexcept
    {
        return const_iterator(m_root.next);
    }

    inline const_iterator cend() const noexcept
    {
        return const_iterator(&m_root);
    }

    inline reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&m_root);
    }

    inline const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(&m_root);
    }

    inline reverse_iterator rend() noexcept
    {
        return reverse_iterator(m_root.next)
    }

    inline const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(m_root.next)
    }

    inline const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&m_root);
    }

    inline const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(m_root.next)
    }

    inline void push_front(_Ty* item) noexcept
    {
        node_type* node = &(item->*NODE);
        _add(node, &m_root, m_root.next);
    }

    inline void push_back(_Ty* item) noexcept
    {
        node_type* node = &(item->*NODE);
        _add(node, m_root.prev, &m_root);
    }

    inline _Ty* pull_front() noexcept
    {
        auto it = begin();
        if (it == end())
        {
            return nullptr;
        }

        erase(it.m_p);

        return it.m_p;
    }

    inline _Ty* pull_back() noexcept
    {
        auto it = end();
        it.decrement();

        if (it == end())
        {
            return nullptr;
        }

        erase(it.m_p);

        return it.m_p;
    }

    inline void erase(_Ty* item) noexcept
    {
        SK_ASSERT(exists(item));

        node_type* node = &(item->*NODE);
        _remove(node);
    }

    bool exists(const _Ty* item) const
    {
        for (const_iterator it = begin(); it != end(); it++)
        {
            if (it.m_p == item)
            {
                return true;
            }
        }

        return false;
    }

private:
    inline void _add(node_type* node, node_type* prev, node_type* next) noexcept
    {
        node->next = next;
        node->prev = prev;

        next->prev = node;
        prev->next = node;

        m_size++;
    }

    inline void _remove(node_type* node) noexcept
    {
        SK_ASSERT(m_size);

        node->next->prev = node->prev;
        node->prev->next = node->next;

        node->next = nullptr;
        node->prev = nullptr;

        m_size--;
    }

private:
    node_type m_root;
    size_type m_size;
};


} // namespace sketch {}
