#pragma once

#include "../Sketch.hxx"

#include "AtomicOps.hxx"


namespace sketch
{

typedef volatile int32_t atomic32_t;
typedef volatile int64_t atomic64_t;

#ifdef SKETCH_64
typedef atomic64_t atomic_t;
typedef int64_t atomic_base_t;
#else
typedef atomic32_t atomic_t;
typedef int32_t atomic_base_t;
#endif

} // namespace sketch {}


namespace sketch
{

void MemoryBarrier();

namespace atomic
{

namespace nobarrier
{

int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal);
int32_t exchange(atomic32_t* x, int32_t newVal);
int32_t increment(atomic32_t* x, int32_t increment);
void store(atomic32_t* x, int32_t newVal);
int32_t load(atomic32_t* x);

#ifdef SKETCH_64

int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal);
int64_t exchange(atomic64_t* x, int64_t newVal);
int64_t increment(atomic64_t* x, int64_t increment);
void store(atomic64_t* x, int64_t newVal);
int64_t load(atomic64_t* x);

#endif // SKETCH_64

} // namespace nobarrier {}

namespace barrier
{

int32_t increment(atomic32_t* x, int32_t increment);

#ifdef SKETCH_64

int64_t increment(atomic64_t* x, int64_t increment);

#endif // SKETCH_64

} // namespace barrier {}

namespace acquire
{

int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal);
void store(atomic32_t* x, int32_t newVal);
int32_t load(atomic32_t* x);

#ifdef SKETCH_64

int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal);
void store(atomic64_t* x, int64_t newVal);
int64_t load(atomic64_t* x);

#endif // SKETCH_64

} // namespace acquire {}

namespace release
{

int32_t compare_exchange(atomic32_t* x, int32_t newVal, int32_t oldVal);
void store(atomic32_t* x, int32_t newVal);
int32_t load(atomic32_t* x);

#ifdef SKETCH_64

int64_t compare_exchange(atomic64_t* x, int64_t newVal, int64_t oldVal);
void store(atomic64_t* x, int64_t newVal);
int64_t load(atomic64_t* x);

#endif // SKETCH_64

} // namespace release {}

} // namespace atomic {}

} // namespace sketch {}


#ifdef SKETCH_WIN
#include "nt/Atomic_nt.hxx"
#endif



