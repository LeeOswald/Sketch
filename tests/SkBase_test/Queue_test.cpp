#include "common.h"
#include "Doll.h"

#include <SkBase/Queue.hxx>


using namespace sketch;

typedef Doll<false> DollT;


SKETCH_TEST_BEGIN(Queue, "Queue::Queue()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);
    {
        Queue<DollT> q(1000);
        SKETCH_TEST_ASSERT(q.empty());
        SKETCH_TEST_ASSERT(q.size() == 0);

        DollT d1(1);
        q.push(&d1);

        SKETCH_TEST_ASSERT(!q.empty());
        SKETCH_TEST_ASSERT(q.size() == 1);

        DollT d2(2);
        q.push(&d2);

        SKETCH_TEST_ASSERT(!q.empty());
        SKETCH_TEST_ASSERT(q.size() == 2);

        DollT d3(3);
        q.push(&d3);

        SKETCH_TEST_ASSERT(!q.empty());
        SKETCH_TEST_ASSERT(q.size() == 3);

        auto g = q.pop();
        SKETCH_TEST_ASSERT(g->m_value == 1);
        SKETCH_TEST_ASSERT(!q.empty());
        SKETCH_TEST_ASSERT(q.size() == 2);

        g = q.pop();
        SKETCH_TEST_ASSERT(g->m_value == 2);
        SKETCH_TEST_ASSERT(!q.empty());
        SKETCH_TEST_ASSERT(q.size() == 1);

        g = q.pop();
        SKETCH_TEST_ASSERT(g->m_value == 3);
        SKETCH_TEST_ASSERT(q.empty());
        SKETCH_TEST_ASSERT(q.size() == 0);
    }
    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()
