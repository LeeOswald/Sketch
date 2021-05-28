#pragma once

#include "../Sketch.hxx"

#include "IDelegate.hxx"
#include "UniquePtr.hxx"


namespace sketch
{

namespace thread
{

typedef uint32_t id_t;
typedef uintptr_t affinity_t;

enum class priority : int
{
    low,
    normal,
    high,
    realtime
};


namespace current
{

id_t id() noexcept;

void yield() noexcept;

void sleep(uint32_t msec) noexcept;

bool sleepAlertable(uint32_t msec) noexcept; // returns true if alerted

void setPriority(priority p) noexcept;

void setAffinity(affinity_t a) noexcept;

uint32_t currentProcessor() noexcept;

void setName(const char* name) noexcept;

} // namespace current {}

} // namespace thread {}

namespace internal
{

class ThreadImpl;

} // namespace internal {}


class Thread
    : public NonCopyable
{
public:
    ~Thread() noexcept;
    Thread() noexcept;
    explicit Thread(ISimpleDelegate* task, void* ctx, const char* name = nullptr) noexcept;
    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;

    thread::id_t id() const noexcept;
    bool valid() const noexcept;
    void join() noexcept;
    void alert() noexcept;

private:
    UniquePtr<internal::ThreadImpl> m_impl;
};

} // namespace sketch {}


#if defined(SKETCH_WIN_KM)
#include "nt/Thread_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/Thread_win32.hxx"
#endif
