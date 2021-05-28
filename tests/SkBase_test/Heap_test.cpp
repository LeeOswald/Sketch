#include "common.h"

#include <SkBase/Heap.Debug.hxx>
#include <SkBase/Heap.Generic.hxx>
#include <SkBase/Heap.Page.hxx>

using namespace sketch;


SKETCH_TEST_BEGIN(DebugHeap, "Global leak")

    for (int i = 1; i <= 2; i++)
    {
        auto c = static_cast<char*>(SkMalloc(100 + i * 3));
        strncpy_s(c, 100, "This is an intentionally leaked block", 100);
    }

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(DebugHeap, "DebugHeap")

    static const char testData[] = "Test contents";
    static const size_t testSize = sizeof(testData);

    DebugHeap<GenericHeap> h;
    auto p = static_cast<char*>(h.allocate(testSize, __FILE__, __LINE__));
    SKETCH_TEST_ASSERT(p);

    memcpy(p, testData, testSize);

    h.dump(DebugHeap<GenericHeap>::DumpStats | DebugHeap<GenericHeap>::DumpBlocks | DebugHeap<GenericHeap>::DumpMemory);

    h.free(p);

SKETCH_TEST_END()

#ifdef SKETCH_WIN_UM

SKETCH_TEST_BEGIN(PageHeap, "PageHeap")

    SketchDebug("Allocation granularity %d", PageHeap<>::granularity());

    static const char testData[] = "Test contents";
    static const size_t testSize = sizeof(testData);

    PageHeap<> h;
    auto p = static_cast<char*>(h.allocate(4096));
    SKETCH_TEST_ASSERT(p);

    SketchDebug("Allocated virtual memory at %p", p);

    memcpy(p, testData, testSize);
    SKETCH_TEST_ASSERT(!memcmp(p, testData, testSize));

    h.free(p);

SKETCH_TEST_END()


#endif // SKETCH_WIN_UM

#ifdef SKETCH_WIN_KM

SKETCH_TEST_BEGIN(PageHeap, "PageHeap")

    SketchDebug("Allocation granularity %d", PageHeap<>::granularity());

    static const char testData[] = "Test contents";
    static const size_t testSize = sizeof(testData);

    PageHeap<> h;
    auto p = static_cast<char*>(h.allocate(4096));
    SKETCH_TEST_ASSERT(p);

    SketchDebug("Allocated virtual memory at %p", p);

    memcpy(p, testData, testSize);
    SKETCH_TEST_ASSERT(!memcmp(p, testData, testSize));

    h.free(p);

SKETCH_TEST_END()


#endif // SKETCH_WIN_KM
