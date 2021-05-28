#pragma once


namespace sketch
{

inline void MemoryBarrier()
{
#ifdef SKETCH_64
    ::sketch::intrinsic::__faststorefence();
#else
    ::MemoryBarrier();
#endif
}


namespace atomic
{

#pragma region nobarrier

namespace nobarrier
{

inline int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal)
{
    return ::sketch::atomic::compare_exchange(x, newVal, oldVal);
}

inline int32_t exchange(atomic32_t* x, int32_t newVal)
{
    return ::sketch::atomic::exchange(x, newVal);
}

inline int32_t increment(atomic32_t* x, int32_t increment)
{
    return ::sketch::atomic::exchange_add(x, increment);
}

inline void store(atomic32_t* x, int32_t newVal)
{
    *x = newVal;
}

inline int32_t load(atomic32_t* x)
{
    return *x;
}

#ifdef SKETCH_64

inline int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal)
{
    return ::sketch::atomic::compare_exchange(x, newVal, oldVal);
}

inline int64_t exchange(atomic64_t* x, int64_t newVal)
{
    return ::sketch::atomic::exchange(x, newVal);
}

inline int64_t increment(atomic64_t* x, int64_t increment)
{
    return ::sketch::atomic::exchange_add(x, increment);
}

inline void store(atomic64_t* x, int64_t newVal)
{
    *x = newVal;
}

inline int64_t load(atomic64_t* x)
{
    return *x;
}

#endif // SKETCH_64

} // namespace nobarrier {}

#pragma endregion

#pragma region barrier

namespace barrier
{

inline int32_t increment(atomic32_t* x, int32_t increment)
{
    return ::sketch::atomic::nobarrier::increment(x, increment);
}

#ifdef SKETCH_64

inline int64_t increment(atomic64_t* x, int64_t increment)
{
    return ::sketch::atomic::exchange_add(x, increment);
}

#endif // SKETCH_64

} // namespace barrier {}

#pragma endregion

#pragma region acquire

namespace acquire
{

inline int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal)
{
    return ::sketch::atomic::nobarrier::compare_exchange(x, newVal, oldVal);
}

inline void store(atomic32_t* x, int32_t newVal)
{
    // acts as a barrier in this implementation
    ::sketch::atomic::nobarrier::exchange(x, newVal);
}

inline int32_t load(atomic32_t* x)
{
    atomic32_t v = *x;
    return v;
}

#ifdef SKETCH_64

inline int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal)
{
    return ::sketch::atomic::nobarrier::compare_exchange(x, newVal, oldVal);
}

inline void store(atomic64_t* x, int64_t newVal)
{
    // acts as a barrier in this implementation
    ::sketch::atomic::nobarrier::exchange(x, newVal);
}

inline int64_t load(atomic64_t* x)
{
    atomic64_t v = *x;
    return v;
}

#endif // SKETCH_64

} // namespace acquire {}

#pragma endregion

#pragma region release

namespace release
{

inline int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal)
{
    return ::sketch::atomic::nobarrier::compare_exchange(x, newVal, oldVal);
}

inline void store(atomic32_t* x, int32_t newVal)
{
    // works w/o barrier
    *x = newVal;
}

inline int32_t load(atomic32_t* x)
{
    ::sketch::MemoryBarrier();
    return *x;
}

#ifdef SKETCH_64

inline int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal)
{
    return ::sketch::atomic::nobarrier::compare_exchange(x, newVal, oldVal);
}

inline void store(atomic64_t* x, int64_t newVal)
{
    // works w/o barrier
    *x = newVal;
}

inline int64_t load(atomic64_t* x)
{
    ::sketch::MemoryBarrier();
    return *x;
}

#endif // SKETCH_64

} // namespace release {}

#pragma endregion

} // namespace atomic {}

} // namespace sketch {}
