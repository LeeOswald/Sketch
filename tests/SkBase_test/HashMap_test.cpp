#include "common.h"
#include "Doll.h"

#include <SkBase/HashMap.hxx>

using namespace sketch;

typedef Doll<false> DollT;
typedef HashMap<int, DollT> MapIntT;
typedef HashMap<String, DollT> MapStringT;


SKETCH_TEST_BEGIN(HashMap, "HashMap::HashMap()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        MapIntT m;
        m[0] = DollT(0);
        m[1] = DollT(1);
        m[2] = DollT(2);
        m[3] = DollT(3);
        m[4] = DollT(4);

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        SKETCH_TEST_ASSERT(m[0].m_value == 0);
        SKETCH_TEST_ASSERT(m[1].m_value == 1);
        SKETCH_TEST_ASSERT(m[2].m_value == 2);
        SKETCH_TEST_ASSERT(m[3].m_value == 3);
        SKETCH_TEST_ASSERT(m[4].m_value == 4);
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        MapStringT m;
        m[String("0")] = DollT(0);
        m[String("11")] = DollT(1);
        m[String("222")] = DollT(2);
        m[String("3333")] = DollT(3);
        m[String("44444")] = DollT(4);

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        SKETCH_TEST_ASSERT(m[String("0")].m_value == 0);
        SKETCH_TEST_ASSERT(m[String("11")].m_value == 1);
        SKETCH_TEST_ASSERT(m[String("222")].m_value == 2);
        SKETCH_TEST_ASSERT(m[String("3333")].m_value == 3);
        SKETCH_TEST_ASSERT(m[String("44444")].m_value == 4);
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()
