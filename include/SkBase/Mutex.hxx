#pragma once

#include "../Sketch.hxx"
#include "UniquePtr.hxx"

namespace sketch
{

namespace internal
{

class MutexImpl;

} // namespace internal {}


// non-recursive mutex

class Mutex
    : public NonCopyable
{
public:
    class Guard
        : public NonCopyable
    {
    public:
        inline ~Guard() noexcept
        {
            m_.unlock();
        }

        inline explicit Guard(Mutex& s) noexcept
            : m_(s)
        {
            m_.lock();
        }

    private:
        Mutex& m_;
    };

public:
    ~Mutex() noexcept;
    Mutex() noexcept;
    Mutex(Mutex&& other) noexcept;
    Mutex& operator=(Mutex&& other) noexcept;

    bool valid() const noexcept;
    void lock() noexcept;
    void unlock() noexcept;

private:
    sketch::UniquePtr<internal::MutexImpl> m_impl;
};


} // namespace sketch {}
