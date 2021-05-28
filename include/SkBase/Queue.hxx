#pragma once


#include "Atomic.hxx"
#include "murmurhash.hxx"
#include "Thread.hxx"


namespace sketch
{


template <class _Ty, class _Allocator = Allocator<_Ty*>>
class Queue
    : public NonCopyable
{
public:
    typedef _Ty* pointer;
    typedef const _Ty* const_pointer;
    typedef _Ty& reference;
    typedef const _Ty& const_reference;
    typedef _Ty* iterator;
    typedef const _Ty* const_iterator;
    typedef size_t size_type;
    typedef sketch::ptrdiff_t difference_type;
    typedef _Ty value_type;

    typedef typename _Allocator::template rebind<pointer>::other allocator_type;

public:
    ~Queue() noexcept
    {
        if (m_queue)
        {
            m_allocator.deallocate(const_cast<pointer*>(m_queue), m_capacity);
        }
    }

    explicit Queue(size_type size)
        : m_allocator(allocator_type())
        , m_puts(0)
        , m_gets(0)
        , m_capacity(size)
    {
        SK_ASSERT(size);
        m_queue = m_allocator.allocate(size);
        if (m_queue)
        {
            for (size_type i = 0; i < size; i++)
            {
                m_queue[i] = _makeVacantTag(i);
            }
        }
    }

    bool valid() const noexcept
    {
        return !!m_queue;
    }

    size_type capacity() const noexcept
    {
        return m_capacity;
    }

    bool empty() const noexcept
    {
        return (m_puts == m_gets);
    }

    size_type size() const noexcept
    {
        return m_puts - m_gets;
    }

    bool push(pointer p) noexcept
    {
        if (_isVacantTag(p))
        {
            SK_ASSERT(0); // NULLs or unaligned pointers are unsupported
            return false;
        }

        auto putCnt = m_puts;

        while (putCnt - m_gets < ssize_t(m_capacity))
        {
            auto slot = &m_queue[putCnt % m_capacity];
            auto tag = _makeVacantTag(putCnt);

            auto res = (tag == (pointer)atomic::compare_exchange((void* volatile*)slot, (void*)p, (void*)tag));

            // INVARINT: Another item has been added to the queue (by us or whoever)
            // Now need to make sure the m_puts reflects that
            atomic::compare_exchange(&m_puts, putCnt + 1, putCnt);

            // INVARIANT: the m_puts as been updated (by us or whoever)
            if (res)
            {
                // It was us who added the queue item
                return true;
            }

            // Someone else has added an item to the PutSlot
            putCnt = m_puts;
        }

        // POST CONDITION: Queue is full
        return false;
    }

    pointer pop() noexcept
    {
        auto getCnt = m_gets;

        while (m_puts - getCnt > 0)
        {
            auto slot = &m_queue[getCnt % m_capacity];
            auto tag = _makeVacantTag(getCnt + m_capacity);

            auto item = *slot;

            auto res = (item == (pointer)atomic::compare_exchange((void* volatile*)slot, (void*)tag, (void*)item));

            // INVARINT: Another item has been removed (by us or whoever)
            // Now need to make sure the GetCount reflects that
            atomic::compare_exchange(&m_gets, getCnt + 1, getCnt);

            // INVARIANT: the GetCount as been updated (by us or whoever)

            if (res)
            {
                // It was us who removed the queue item
                if (_isVacantTag(item))
                {
                    // ASSUME:
                    // Instead of data we got a vacant slot tag.
                    // Another consumer thread must have removed this element
                    // using m_gets value that must have fallen behind the cycle.
                    // Other than the fact that this thread would have received
                    // an out of order element, no harm is done.
                    // We have set the vacant tag value back to what it should
                    // have been, so other put operations could use it OK.
                }
                else
                {
                    // The removed queue item is not empty
                    return item;
                }
            }

            // Someone else has removed an item from the GetSlot
            getCnt = m_gets;
        }

        // POST CONDITION: Queue is empty
        return nullptr;
    }

private:
    inline pointer _makeVacantTag(size_type i)
    {
        SK_ASSERT(valid());

        // aligned pointer's LSB is always 0
        // we can distinguish actual data from free tags this way
        return reinterpret_cast<pointer>(static_cast<uintptr_t>(murmurHash3(&i, sizeof(i), 'MMH3') | 1));
    }

    inline static bool _isVacantTag(pointer v)
    {
        if (!v || ((uintptr_t(v) & 1) == 1))
        {
            return true;
        }

        return false;
    }

private:
    volatile pointer* m_queue; // array of pointers
    atomic_t m_puts;
    atomic_t m_gets;
    size_type m_capacity;
    allocator_type m_allocator;
};


} // namespace sketch {}
