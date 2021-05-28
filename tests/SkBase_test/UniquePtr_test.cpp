#include "common.h"
#include "Doll.h"

#include <SkBase/UniquePtr.hxx>


using namespace sketch;

typedef Doll<false> DollT;
typedef UniquePtr<DollT> DollPtrT;
typedef UniquePtr<DollT[]> DollAPtrT;


SKETCH_TEST_BEGIN(UniquePtr, "UniquePtr<_Ty>")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        DollPtrT p;
        SKETCH_TEST_ASSERT(!p);
        SKETCH_TEST_ASSERT(!p.get());

        DollPtrT p1(new DollT(1));
        SKETCH_TEST_ASSERT(p1);
        SKETCH_TEST_ASSERT(p1.get());
        SKETCH_TEST_ASSERT(p1->m_value == 1);

        SKETCH_TEST_ASSERT(DollT::instances() == 1);

        p = sketch::move(p1);
        SKETCH_TEST_ASSERT(p);
        SKETCH_TEST_ASSERT(p.get());
        SKETCH_TEST_ASSERT(p->m_value == 1);

        SKETCH_TEST_ASSERT(!p1);
        SKETCH_TEST_ASSERT(!p1.get());

        SKETCH_TEST_ASSERT(DollT::instances() == 1);

        DollPtrT p2(sketch::move(p));
        SKETCH_TEST_ASSERT(p2);
        SKETCH_TEST_ASSERT(p2.get());
        SKETCH_TEST_ASSERT(p2->m_value == 1);

        SKETCH_TEST_ASSERT(!p);
        SKETCH_TEST_ASSERT(!p.get());

        SKETCH_TEST_ASSERT(DollT::instances() == 1);
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(UniquePtr, "UniquePtr<_Ty[]>")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        DollAPtrT p;
        SKETCH_TEST_ASSERT(!p);
        SKETCH_TEST_ASSERT(!p.get());

        DollAPtrT p1(new DollT[5]);
        SKETCH_TEST_ASSERT(p1);
        SKETCH_TEST_ASSERT(p1.get());

        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(p1[i]->m_value == 0);
        }

        for (size_t i = 0; i < 5; i++)
        {
            p1[i]->m_value = (long)i;
        }

        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(p1[i]->m_value == long(i));
        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        p = sketch::move(p1);
        SKETCH_TEST_ASSERT(p);
        SKETCH_TEST_ASSERT(p.get());

        SKETCH_TEST_ASSERT(!p1);
        SKETCH_TEST_ASSERT(!p1.get());

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        DollAPtrT p2(sketch::move(p));
        SKETCH_TEST_ASSERT(p2);
        SKETCH_TEST_ASSERT(p2.get());

        SKETCH_TEST_ASSERT(!p);
        SKETCH_TEST_ASSERT(!p.get());

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(p2[i]->m_value == long(i));
        }
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()
