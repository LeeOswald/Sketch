#pragma once

#include "../Sketch.hxx"

#ifdef SKETCH_WIN
#include "nt/AtomicOps_nt.hxx"
#endif


namespace sketch
{

namespace atomic
{

#pragma region Atomic increment

uint16_t increment(volatile uint16_t* val);
uint32_t increment(volatile uint32_t* val);
uint64_t increment(volatile uint64_t* val);
int16_t increment(volatile int16_t* val);
int32_t increment(volatile int32_t* val);
int64_t increment(volatile int64_t* val);

#pragma endregion


#pragma region Atomic decrement

uint16_t decrement(volatile uint16_t* val);
uint32_t decrement(volatile uint32_t* val);
uint64_t decrement(volatile uint64_t* val);
int16_t decrement(volatile int16_t* val);
int32_t decrement(volatile int32_t* val);
int64_t decrement(volatile int64_t* val);

#pragma endregion


#pragma region Atomic exchange

uint32_t exchange(volatile uint32_t* dest, uint32_t val);
uint64_t exchange(volatile uint64_t* dest, uint64_t val);
int32_t exchange(volatile int32_t* dest, int32_t val);
int64_t exchange(volatile int64_t* dest, int64_t val);

void* exchange(void* volatile* target, void* value);

#pragma endregion


#pragma region Exchange add

uint32_t exchange_add(volatile uint32_t* dest, uint32_t val);
uint64_t exchange_add(volatile uint64_t* dest, uint64_t val);
int32_t exchange_add(volatile int32_t* dest, int32_t val);
int64_t exchange_add(volatile int64_t* dest, int64_t val);

#pragma endregion


#pragma region Compare exchange

uint16_t compare_exchange(volatile uint16_t* dest, uint16_t exchange, uint16_t comparand);
uint32_t compare_exchange(volatile uint32_t* dest, uint32_t exchange, uint32_t comparand);
uint64_t compare_exchange(volatile uint64_t* dest, uint64_t exchange, uint64_t comparand);
int16_t compare_exchange(volatile int16_t* dest, int16_t exchange, int16_t comparand);
int32_t compare_exchange(volatile int32_t* dest, int32_t exchange, int32_t comparand);
int64_t compare_exchange(volatile int64_t* dest, int64_t exchange, int64_t comparand);

void* compare_exchange(void* volatile* destination, void* exchange, void* comparand);

#pragma endregion


#pragma region Bitwise AND

bool bit_and(volatile bool* val, bool mask);
uint8_t bit_and(volatile uint8_t* val, uint8_t mask);
uint16_t bit_and(volatile uint16_t* val, uint16_t mask);
uint32_t bit_and(volatile uint32_t* val, uint32_t mask);
uint64_t bit_and(volatile uint64_t* val, uint64_t mask);
int8_t bit_and(volatile int8_t* val, int8_t mask);
int16_t bit_and(volatile int16_t* val, int16_t mask);
int32_t bit_and(volatile int32_t* val, int32_t mask);
int64_t bit_and(volatile int64_t* val, int64_t mask);

#pragma endregion


#pragma region Bitwise OR

bool bit_or(volatile bool* val, bool mask);
uint8_t bit_or(volatile uint8_t* val, uint8_t mask);
uint16_t bit_or(volatile uint16_t* val, uint16_t mask);
uint32_t bit_or(volatile uint32_t* val, uint32_t mask);
uint64_t bit_or(volatile uint64_t* val, uint64_t mask);
int8_t bit_or(volatile int8_t* val, int8_t mask);
int16_t bit_or(volatile int16_t* val, int16_t mask);
int32_t bit_or(volatile int32_t* val, int32_t mask);
int64_t bit_or(volatile int64_t* val, int64_t mask);

#pragma endregion


#pragma region Bitwise XOR

bool bit_xor(volatile bool* val, bool mask);
uint8_t bit_xor(volatile uint8_t* val, uint8_t mask);
uint16_t bit_xor(volatile uint16_t* val, uint16_t mask);
uint32_t bit_xor(volatile uint32_t* val, uint32_t mask);
uint64_t bit_xor(volatile uint64_t* val, uint64_t mask);
int8_t bit_xor(volatile int8_t* val, int8_t mask);
int16_t bit_xor(volatile int16_t* val, int16_t mask);
int32_t bit_xor(volatile int32_t* val, int32_t mask);
int64_t bit_xor(volatile int64_t* val, int64_t mask);

#pragma endregion


} // namespace atomic {}

} // namespace sketch {}
