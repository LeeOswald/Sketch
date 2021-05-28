#include "common.h"
#include "Doll.h"

#include <SkBase/IntrusiveList.hxx>

using namespace sketch;

typedef Doll<false> DollT;

struct InDoll
    : public DollT
{
    InDoll(long l = 0)
        : DollT(l)
    {
    }

    ListNode<InDoll> nodeA;
    ListNode<InDoll> nodeB;
};


SKETCH_TEST_BEGIN(IntrusiveList, "IntrusiveList::IntrusiveList()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        IntrusiveList<InDoll, &InDoll::nodeA> listA;
        IntrusiveList<InDoll, &InDoll::nodeB> listB;

        InDoll d1(1);
        InDoll d2(2);
        InDoll d3(3);

        listA.push_back(&d1);
        listA.push_back(&d2);
        listA.push_back(&d3);

        SKETCH_TEST_ASSERT(DollT::instances() == 3);

        auto it = listA.begin();
        SK_ASSERT(it->m_value == 1);
        it.increment();
        SK_ASSERT(it->m_value == 2);
        it.increment();
        SK_ASSERT(it->m_value == 3);
        it.increment();
        SK_ASSERT(it == listA.end());

        long l = 1;
        for (const auto& d : listA)
        {
            SKETCH_TEST_ASSERT(d.m_value == l);
            l++;
        }

        listA.erase(&d2);

        it = listA.begin();
        SK_ASSERT(it->m_value == 1);
        it.increment();
        SK_ASSERT(it->m_value == 3);
        it.increment();
        SK_ASSERT(it == listA.end());
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()
