#include "common.h"
#include "Doll.h"

#include <SkBase/CircularBuffer.hxx>

using namespace sketch;

typedef Doll<false> DollT;

SKETCH_TEST_BEGIN(CircularBuffer, "CircularBuffer::CircularBuffer()")
    CircularBuffer<int, sketch::Allocator<int>> aa;
    SKETCH_TEST_ASSERT(aa.empty());
    SKETCH_TEST_ASSERT(!aa.valid());
    SKETCH_TEST_ASSERT(!aa.raw());
    SKETCH_TEST_ASSERT(aa.capacity() == 0);
    SKETCH_TEST_ASSERT(aa.full());
    SKETCH_TEST_ASSERT(aa.size() == 0);
    SKETCH_TEST_ASSERT(aa.begin() == aa.end());
    SKETCH_TEST_ASSERT(aa.rbegin() == aa.rend());
SKETCH_TEST_END()

SKETCH_TEST_BEGIN(CircularBuffer, "CircularBuffer::push_back()")
    {
        CircularBuffer<DollT, sketch::Allocator<DollT>> aa(5);
        SKETCH_TEST_ASSERT(aa.empty());
        SKETCH_TEST_ASSERT(aa.valid());
        SKETCH_TEST_ASSERT(aa.raw());
        SKETCH_TEST_ASSERT(aa.capacity() == 5);
        SKETCH_TEST_ASSERT(!aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 0);
        SKETCH_TEST_ASSERT(aa.begin() == aa.end());
        SKETCH_TEST_ASSERT(aa.rbegin() == aa.rend());

        aa.push_back();
        SKETCH_TEST_ASSERT(!aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 1);

        aa.push_back(sketch::move(DollT(1)));
        SKETCH_TEST_ASSERT(!aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 2);

        aa.push_back((DollT(2)));
        SKETCH_TEST_ASSERT(!aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 3);

        aa.push_back((DollT(3)));
        SKETCH_TEST_ASSERT(!aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 4);

        aa.push_back((DollT(4)));

        SKETCH_TEST_ASSERT(aa.full());
        SKETCH_TEST_ASSERT(aa.size() == 5);

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        long l = 0;
        for (const auto& a : aa)
        {
            SKETCH_TEST_ASSERT(a.m_value == l);
            l++;
        }
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()

SKETCH_TEST_BEGIN(CircularBuffer, "CircularBuffer::insert()")
    CircularBuffer<int, sketch::Allocator<int>> b(10);

    static const int samples[20] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    };

    SKETCH_TEST_ASSERT(b.empty());
    SKETCH_TEST_ASSERT(b.size() == 0);

    b.insert(b.end(), samples, samples + 5);
    SKETCH_TEST_ASSERT(!b.empty());
    SKETCH_TEST_ASSERT(b.size() == 5);

    {
        auto arr1 = b.array_one();
        SKETCH_TEST_ASSERT(arr1.second == 5);
        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(arr1.first[i] == int(i));
        }

        auto arr2 = b.array_two();
        SKETCH_TEST_ASSERT(arr2.second == 0);
    }

    b.insert(b.end(), samples + 5, samples + 10);
    SKETCH_TEST_ASSERT(!b.empty());
    SKETCH_TEST_ASSERT(b.size() == 10);

    {
        auto arr1 = b.array_one();
        SKETCH_TEST_ASSERT(arr1.second == 10);
        for (size_t i = 0; i < 10; i++)
        {
            SKETCH_TEST_ASSERT(arr1.first[i] == int(i));
        }

        auto arr2 = b.array_two();
        SKETCH_TEST_ASSERT(arr2.second == 0);
    }

    b.insert(b.end(), samples + 10, samples + 15);
    SKETCH_TEST_ASSERT(!b.empty());
    SKETCH_TEST_ASSERT(b.size() == 10);

    {
        auto arr1 = b.array_one();
        SKETCH_TEST_ASSERT(arr1.second == 5);
        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(arr1.first[i] == int(i) + 5);
        }

        auto arr2 = b.array_two();
        SKETCH_TEST_ASSERT(arr2.second == 5);
        for (size_t i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(arr2.first[i] == int(i) + 10);
        }
    }

    b.insert(b.end(), samples + 15, samples + 20);
    SKETCH_TEST_ASSERT(!b.empty());
    SKETCH_TEST_ASSERT(b.size() == 10);

    {
        auto arr1 = b.array_one();
        SKETCH_TEST_ASSERT(arr1.second == 10);
        for (size_t i = 0; i < 10; i++)
        {
            SKETCH_TEST_ASSERT(arr1.first[i] == int(i) + 10);
        }

        auto arr2 = b.array_two();
        SKETCH_TEST_ASSERT(arr2.second == 0);
    }

SKETCH_TEST_END()
