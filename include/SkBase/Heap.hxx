#pragma once

#include "../Sketch.hxx"


namespace sketch
{

namespace heap
{

#ifdef SKETCH_USE_DEBUG_ALLOCATOR
void* allocate(size_t sz, const char* file, int line) noexcept;
#else
void* allocate(size_t sz) noexcept;
#endif // SKETCH_USE_DEBUG_ALLOCATOR

void free(void* p) noexcept;


} // namespace heap {}

} // namespace sketch {}


#ifdef SKETCH_USE_DEBUG_ALLOCATOR
#define SkMalloc(sz) ::sketch::heap::allocate(sz, __FILE__, __LINE__)
#else
#define SkMalloc(sz) ::sketch::heap::allocate(sz)
#endif // SKETCH_USE_DEBUG_ALLOCATOR

#define SkFree(p) ::sketch::heap::free(p)
