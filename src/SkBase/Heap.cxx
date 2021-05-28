#include <Sketch.hxx>

#include "Heap_impl.hxx"


#ifdef SKETCH_USE_DEBUG_ALLOCATOR
#include <SkBase/Heap.Debug.hxx>
#endif

#include <SkBase/Heap.Generic.hxx>


namespace sketch
{

namespace heap
{

#ifdef SKETCH_USE_DEBUG_ALLOCATOR
typedef DebugHeap<GenericHeap> HeapType;
#else
typedef GenericHeap HeapType;
#endif

static uint8_t _GlobalHeapPlace[sizeof(HeapType)];
static HeapType* g_GlobalHeap = nullptr;


#ifdef SKETCH_USE_DEBUG_ALLOCATOR

static inline void DumpLeaks()
{
    g_GlobalHeap->dump(HeapType::DumpAsLeaks | HeapType::DumpBlocks | HeapType::DumpMemory);
}

static inline void DumpStats()
{
    g_GlobalHeap->dump(HeapType::DumpStats);
}

#endif // SKETCH_USE_DEBUG_ALLOCATOR


bool initialize() noexcept
{
    if (g_GlobalHeap)
    {
        SK_ASSERT(0);
        return false;
    }

    g_GlobalHeap = new (_GlobalHeapPlace) HeapType;

    return true;
}

void finalize() noexcept
{
    if (g_GlobalHeap)
    {
#ifdef SKETCH_USE_DEBUG_ALLOCATOR
        DumpStats();
        DumpLeaks();
#endif

        g_GlobalHeap->~HeapType();
        g_GlobalHeap = nullptr;
    }
}

#ifdef SKETCH_USE_DEBUG_ALLOCATOR

void* allocate(size_t sz, const char* file, int line) noexcept
{
    if (!g_GlobalHeap)
    {
        SK_ASSERT(0);
        return nullptr;
    }

    return g_GlobalHeap->allocate(sz, file, line);
}

#else

void* allocate(size_t sz) noexcept
{
    if (!g_GlobalHeap)
    {
        SK_ASSERT(0);
        return nullptr;
    }

    return g_GlobalHeap->allocate(sz);
}

#endif // SKETCH_USE_DEBUG_ALLOCATOR


void free(void* p) noexcept
{
    if (!g_GlobalHeap)
    {
        SK_ASSERT(0);
        return;
    }

    g_GlobalHeap->free(p);
}


} // namespace heap {}

} // namespace sketch {}
