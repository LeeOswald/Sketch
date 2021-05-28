#pragma once

#include "Atomic.hxx"


namespace sketch
{

namespace refcount
{

// let's use a POD type to avoid issues like initialization order, etc
typedef atomic_t type;

const type Persistent = -1;


inline bool persistent(type* v) noexcept
{
    auto val = ::sketch::atomic::acquire::load(v);
    return (val == Persistent);
}

inline atomic_base_t increment(type* v) noexcept
{
    if (!persistent(v))
    {
        return ::sketch::atomic::nobarrier::increment(v, 1);
    }

    return 1;
}

inline atomic_base_t decrement(type* v) noexcept
{
    if (!persistent(v))
    {
        SK_ASSERT(*v > 0);

        // use barriers to ensure that state written before the reference count
        // became zero will be visible to a thread that has just made the count zero
        return ::sketch::atomic::barrier::increment(v, -1);
    }

    return 1;
}

inline bool lock(type* v) noexcept
{
    auto result = ::sketch::atomic::acquire::compare_exchange(v, 2, 1);
    return (result == 1);
}

inline void unlock(type* v) noexcept
{
    SK_ASSERT(*v == 2);
    decrement(v);
}


class lock_scope
    : public NonCopyable
{
public:
    ~lock_scope() noexcept
    {
        if (owned())
        {
            unlock(m_v);
        }
    }

    lock_scope() noexcept
        : m_v(nullptr)
    {
    }

    explicit lock_scope(type* v) noexcept
        : m_v(v)
    {
    }

    lock_scope(lock_scope&& other) noexcept
        : lock_scope()
    {
        other.swap(*this);
    }

    lock_scope& operator=(lock_scope&& other) noexcept
    {
        if (m_v != other.m_v)
        {
            lock_scope t(sketch::move(other));
            swap(t);
        }

        return *this;
    }

    bool owned() const noexcept
    {
        return (m_v && (*m_v == 2));
    }

    void swap(lock_scope& other) noexcept
    {
        using sketch::swap;
        swap(m_v, other.m_v);
    }

private:
    type* m_v;
};


inline lock_scope lock_for_scope(type* v) noexcept
{
    if (!lock(v))
    {
        return lock_scope();
    }

    return lock_scope(v);
}


} // namespace refcount {}

} // namespace sketch {}
