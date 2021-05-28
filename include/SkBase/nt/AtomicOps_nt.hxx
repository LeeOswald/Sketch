#pragma once


namespace sketch
{

namespace intrinsic
{

#pragma region Interlocked ops

extern "C"
{

long __cdecl _InterlockedAnd(volatile uint32_t*, uint32_t);
char    __cdecl _InterlockedAnd8(volatile uint8_t*, uint8_t);
int16_t __cdecl _InterlockedAnd16(volatile unsigned short*, unsigned short);
int64_t __cdecl _InterlockedAnd64(volatile uint64_t*, uint64_t);

long __cdecl _InterlockedOr(volatile uint32_t*, uint32_t);
char    __cdecl _InterlockedOr8(volatile uint8_t*, uint8_t);
int16_t __cdecl _InterlockedOr16(volatile uint16_t*, uint16_t);
int64_t __cdecl _InterlockedOr64(volatile uint64_t*, uint64_t);

long __cdecl _InterlockedXor(volatile uint32_t*, uint32_t);
char    __cdecl _InterlockedXor8(volatile uint8_t*, uint8_t);
int16_t __cdecl _InterlockedXor16(volatile uint16_t*, uint16_t);
int64_t __cdecl _InterlockedXor64(volatile uint64_t*, uint64_t);

long __cdecl _InterlockedIncrement(volatile uint32_t*);
int16_t __cdecl _InterlockedIncrement16(volatile uint16_t*);
int64_t __cdecl _InterlockedIncrement64(volatile uint64_t*);

long __cdecl _InterlockedDecrement(volatile uint32_t*);
int16_t __cdecl _InterlockedDecrement16(volatile uint16_t*);
int64_t __cdecl _InterlockedDecrement64(volatile uint64_t*);

long __cdecl _InterlockedExchange(volatile uint32_t*, uint32_t);
int64_t __cdecl _InterlockedExchange64(volatile uint64_t*, uint64_t);

long __cdecl _InterlockedExchangeAdd(volatile uint32_t*, uint32_t);
int64_t __cdecl _InterlockedExchangeAdd64(volatile uint64_t*, uint64_t);

long __cdecl _InterlockedCompareExchange(volatile uint32_t*, uint32_t, uint32_t);
int16_t __cdecl _InterlockedCompareExchange16(volatile uint16_t*, uint16_t, uint16_t);
int64_t __cdecl _InterlockedCompareExchange64(volatile uint64_t*, uint64_t, uint64_t);

void*   __cdecl _InterlockedCompareExchangePointer(void* volatile* destination, void* exchange, void* comparand);
void*   __cdecl _InterlockedExchangePointer(void* volatile* target, void* value);

} // extern "C" {}



#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedAnd8)
#pragma intrinsic(_InterlockedAnd16)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedOr8)
#pragma intrinsic(_InterlockedOr16)
#pragma intrinsic(_InterlockedXor)
#pragma intrinsic(_InterlockedXor8)
#pragma intrinsic(_InterlockedXor16)

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedIncrement16)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedDecrement16)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedCompareExchange64)

#ifdef SKETCH_64
#pragma intrinsic(_InterlockedAnd64)
#pragma intrinsic(_InterlockedOr64)
#pragma intrinsic(_InterlockedXor64)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#pragma intrinsic(_InterlockedExchangePointer)
#endif

#pragma endregion

#pragma region Fences and barriers

extern "C"
{

void __cdecl _ReadBarrier(void);
void __cdecl _WriteBarrier(void);
void __cdecl _ReadWriteBarrier(void);


// Guarantees that every load instruction that precedes, in program order, the load fence instruction is globally visible
// before any load instruction that follows the fence in program order. Performs an implicit _ReadBarrier().
void __cdecl _mm_lfence(void);

// Guarantees that every memory access that precedes, in program order, the memory fence instruction is globally visible
// before any memory instruction that follows the fence in program order. Performs an implicit _WriteBarrier().
void __cdecl _mm_mfence(void);

// Guarantees that every preceding store is globally visible before any subsequent store. Peforms an implicit _ReadWriteBarrier().
void __cdecl _mm_sfence(void);


#ifdef SKETCH_64
// Guarantees that every preceding store is globally visible before any subsequent store.
void __cdecl __faststorefence();
#endif

} // extern "C" {}


#pragma intrinsic(_ReadBarrier)
#pragma intrinsic(_WriteBarrier)
#pragma intrinsic(_ReadWriteBarrier)
#pragma intrinsic(_mm_lfence)
#pragma intrinsic(_mm_mfence)
#pragma intrinsic(_mm_sfence)

#ifdef SKETCH_64
#pragma intrinsic(__faststorefence)
#endif

#pragma endregion

} // namespace intrinsic {}


sk_static_assert(sizeof(bool) == sizeof(uint8_t));


namespace atomic
{

#pragma region Atomic increment

inline uint16_t increment(volatile uint16_t* val)
{
    return intrinsic::_InterlockedIncrement16(val);
}

inline uint32_t increment(volatile uint32_t* val)
{
    return intrinsic::_InterlockedIncrement(val);
}

inline uint64_t increment(volatile uint64_t* val)
{
    return intrinsic::_InterlockedIncrement64(val);
}

inline int16_t increment(volatile int16_t* val)
{
    return intrinsic::_InterlockedIncrement16(reinterpret_cast<volatile uint16_t*>(val));
}

inline int32_t increment(volatile int32_t* val)
{
    return intrinsic::_InterlockedIncrement(reinterpret_cast<volatile uint32_t*>(val));
}

inline int64_t increment(volatile int64_t* val)
{
    return intrinsic::_InterlockedIncrement64(reinterpret_cast<volatile uint64_t*>(val));
}

#pragma endregion


#pragma region Atomic decrement

inline uint16_t decrement(volatile uint16_t* val)
{
    return intrinsic::_InterlockedDecrement16(val);
}

inline uint32_t decrement(volatile uint32_t* val)
{
    return intrinsic::_InterlockedDecrement(val);
}

inline uint64_t decrement(volatile uint64_t* val)
{
    return intrinsic::_InterlockedDecrement64(val);
}

inline int16_t decrement(volatile int16_t* val)
{
    return intrinsic::_InterlockedDecrement16(reinterpret_cast<volatile uint16_t*>(val));
}

inline int32_t decrement(volatile int32_t* val)
{
    return intrinsic::_InterlockedDecrement(reinterpret_cast<volatile uint32_t*>(val));
}

inline int64_t decrement(volatile int64_t* val)
{
    return intrinsic::_InterlockedDecrement64(reinterpret_cast<volatile uint64_t*>(val));
}

#pragma endregion

#pragma region Atomic exchange

inline uint32_t exchange(volatile uint32_t* dest, uint32_t val)
{
    return intrinsic::_InterlockedExchange(dest, val);
}

inline uint64_t exchange(volatile uint64_t* dest, uint64_t val)
{
    return intrinsic::_InterlockedExchange64(dest, val);
}

inline int32_t exchange(volatile int32_t* dest, int32_t val)
{
    return intrinsic::_InterlockedExchange(reinterpret_cast<volatile uint32_t*>(dest), val);
}

inline int64_t exchange(volatile int64_t* dest, int64_t val)
{
    return intrinsic::_InterlockedExchange64(reinterpret_cast<volatile uint64_t*>(dest), val);
}

inline void* exchange(void* volatile* target, void* value)
{
    return intrinsic::_InterlockedExchangePointer(target, value);
}

#pragma endregion

#pragma region Exchange add

inline uint32_t exchange_add(volatile uint32_t* dest, uint32_t val)
{
    return intrinsic::_InterlockedExchangeAdd(dest, val) + val; // _InterlockedExchangeAdd* return the initial value but we need the result
}

inline uint64_t exchange_add(volatile uint64_t* dest, uint64_t val)
{
    return intrinsic::_InterlockedExchangeAdd64(dest, val) + val;
}

inline int32_t exchange_add(volatile int32_t* dest, int32_t val)
{
    return intrinsic::_InterlockedExchangeAdd(reinterpret_cast<volatile uint32_t*>(dest), val) + val;
}

inline int64_t exchange_add(volatile int64_t* dest, int64_t val)
{
    return intrinsic::_InterlockedExchangeAdd64(reinterpret_cast<volatile uint64_t*>(dest), val) + val;
}

#pragma endregion

#pragma region Compare exchange

inline uint16_t compare_exchange(volatile uint16_t* dest, uint16_t exchange, uint16_t comparand)
{
    return intrinsic::_InterlockedCompareExchange16(dest, exchange, comparand);
}

inline uint32_t compare_exchange(volatile uint32_t* dest, uint32_t exchange, uint32_t comparand)
{
    return intrinsic::_InterlockedCompareExchange(dest, exchange, comparand);
}

inline uint64_t compare_exchange(volatile uint64_t* dest, uint64_t exchange, uint64_t comparand)
{
    return intrinsic::_InterlockedCompareExchange64(dest, exchange, comparand);
}

inline int16_t compare_exchange(volatile int16_t* dest, int16_t exchange, int16_t comparand)
{
    return intrinsic::_InterlockedCompareExchange16(reinterpret_cast<volatile uint16_t*>(dest), exchange, comparand);
}

inline int32_t compare_exchange(volatile int32_t* dest, int32_t exchange, int32_t comparand)
{
    return intrinsic::_InterlockedCompareExchange(reinterpret_cast<volatile uint32_t*>(dest), exchange, comparand);
}

inline int64_t compare_exchange(volatile int64_t* dest, int64_t exchange, int64_t comparand)
{
    return intrinsic::_InterlockedCompareExchange64(reinterpret_cast<volatile uint64_t*>(dest), exchange, comparand);
}

inline void* compare_exchange(void* volatile* destination, void* exchange, void* comparand)
{
    return intrinsic::_InterlockedCompareExchangePointer(destination, exchange, comparand);
}

#pragma endregion


#pragma region Bitwise AND

inline bool bit_and(volatile bool* val, bool mask)
{
    return intrinsic::_InterlockedAnd8(reinterpret_cast<volatile uint8_t*>(val), mask) == 1;
}

inline uint8_t bit_and(volatile uint8_t* val, uint8_t mask)
{
    return intrinsic::_InterlockedAnd8(val, mask);
}

inline uint16_t bit_and(volatile uint16_t* val, uint16_t mask)
{
    return intrinsic::_InterlockedAnd16(val, mask);
}

inline uint32_t bit_and(volatile uint32_t* val, uint32_t mask)
{
    return intrinsic::_InterlockedAnd(val, mask);
}

inline uint64_t bit_and(volatile uint64_t* val, uint64_t mask)
{
    return intrinsic::_InterlockedAnd64(val, mask);
}

inline int8_t bit_and(volatile int8_t* val, int8_t mask)
{
    return intrinsic::_InterlockedAnd8(reinterpret_cast<volatile uint8_t*>(val), mask);
}

inline int16_t bit_and(volatile int16_t* val, int16_t mask)
{
    return intrinsic::_InterlockedAnd16(reinterpret_cast<volatile uint16_t*>(val), mask);
}

inline int32_t bit_and(volatile int32_t* val, int32_t mask)
{
    return intrinsic::_InterlockedAnd(reinterpret_cast<volatile uint32_t*>(val), mask);
}

inline int64_t bit_and(volatile int64_t* val, int64_t mask)
{
    return intrinsic::_InterlockedAnd64(reinterpret_cast<volatile uint64_t*>(val), mask);
}

#pragma endregion

#pragma region Bitwise OR

inline bool bit_or(volatile bool* val, bool mask)
{
    return intrinsic::_InterlockedOr8(reinterpret_cast<volatile uint8_t*>(val), mask) == 1;
}

inline uint8_t bit_or(volatile uint8_t* val, uint8_t mask)
{
    return intrinsic::_InterlockedOr8(val, mask);
}

inline uint16_t bit_or(volatile uint16_t* val, uint16_t mask)
{
    return intrinsic::_InterlockedOr16(val, mask);
}

inline uint32_t bit_or(volatile uint32_t* val, uint32_t mask)
{
    return intrinsic::_InterlockedOr(val, mask);
}

inline uint64_t bit_or(volatile uint64_t* val, uint64_t mask)
{
    return intrinsic::_InterlockedOr64(val, mask);
}

inline int8_t bit_or(volatile int8_t* val, int8_t mask)
{
    return intrinsic::_InterlockedOr8(reinterpret_cast<volatile uint8_t*>(val), mask);
}

inline int16_t bit_or(volatile int16_t* val, int16_t mask)
{
    return intrinsic::_InterlockedOr16(reinterpret_cast<volatile uint16_t*>(val), mask);
}

inline int32_t bit_or(volatile int32_t* val, int32_t mask)
{
    return intrinsic::_InterlockedOr(reinterpret_cast<volatile uint32_t*>(val), mask);
}

inline int64_t bit_or(volatile int64_t* val, int64_t mask)
{
    return intrinsic::_InterlockedOr64(reinterpret_cast<volatile uint64_t*>(val), mask);
}

#pragma endregion

#pragma region Bitwise XOR

inline bool bit_xor(volatile bool* val, bool mask)
{
    return intrinsic::_InterlockedXor8(reinterpret_cast<volatile uint8_t*>(val), mask) == 1;
}

inline uint8_t bit_xor(volatile uint8_t* val, uint8_t mask)
{
    return intrinsic::_InterlockedXor8(val, mask);
}

inline uint16_t bit_xor(volatile uint16_t* val, uint16_t mask)
{
    return intrinsic::_InterlockedXor16(val, mask);
}

inline uint32_t bit_xor(volatile uint32_t* val, uint32_t mask)
{
    return intrinsic::_InterlockedXor(val, mask);
}

inline uint64_t bit_xor(volatile uint64_t* val, uint64_t mask)
{
    return intrinsic::_InterlockedXor64(val, mask);
}

inline int8_t bit_xor(volatile int8_t* val, int8_t mask)
{
    return intrinsic::_InterlockedXor8(reinterpret_cast<volatile uint8_t*>(val), mask);
}

inline int16_t bit_xor(volatile int16_t* val, int16_t mask)
{
    return intrinsic::_InterlockedXor16(reinterpret_cast<volatile uint16_t*>(val), mask);
}

inline int32_t bit_xor(volatile int32_t* val, int32_t mask)
{
    return intrinsic::_InterlockedXor(reinterpret_cast<volatile uint32_t*>(val), mask);
}

inline int64_t bit_xor(volatile int64_t* val, int64_t mask)
{
    return intrinsic::_InterlockedXor64(reinterpret_cast<volatile uint64_t*>(val), mask);
}

#pragma endregion

} // namespace atomic {}

} // namespace sketch {}

