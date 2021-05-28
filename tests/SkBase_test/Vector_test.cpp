#include "common.h"
#include "Doll.h"

#include <SkBase/Vector.hxx>


using namespace sketch;

typedef Doll<false> DollT;
typedef Vector<DollT> VectorT;

SKETCH_TEST_BEGIN(Vector, "Vector::Vector()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 0);
        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.begin() == v.end());
        SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() == v.crend());

        v.clear();

        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 0);
        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.begin() == v.end());
        SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() == v.crend());
    }

    {
        VectorT v(5);
        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        SKETCH_TEST_ASSERT(!v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 5);
        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(v.begin() != v.end());
        SKETCH_TEST_ASSERT(v.rbegin() != v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() != v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() != v.crend());

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 0);
        }

        v.clear();

        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 5);
        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.begin() == v.end());
        SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() == v.crend());
    }

    {
        VectorT v(6, DollT(13));
        SKETCH_TEST_ASSERT(DollT::instances() == 6);

        SKETCH_TEST_ASSERT(!v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 6);
        SKETCH_TEST_ASSERT(v.size() == 6);
        SKETCH_TEST_ASSERT(v.begin() != v.end());
        SKETCH_TEST_ASSERT(v.rbegin() != v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() != v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() != v.crend());

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 13);
        }

        v.clear();

        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.empty());
        SKETCH_TEST_ASSERT(v.capacity() == 6);
        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.begin() == v.end());
        SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
        SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
        SKETCH_TEST_ASSERT(v.crbegin() == v.crend());
    }

    {
        VectorT v(5, 34);
        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        {
            VectorT v2(v);
            SKETCH_TEST_ASSERT(DollT::instances() == 10);

            SKETCH_TEST_ASSERT(v.size() == 5);
            SKETCH_TEST_ASSERT(!v.empty());

            SKETCH_TEST_ASSERT(v2.size() == 5);
            SKETCH_TEST_ASSERT(!v2.empty());

            for (const auto& e : v2)
            {
                SKETCH_TEST_ASSERT(e.m_value == 34);
            }

        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);
    }

    {
        VectorT v(5, 77);
        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        {
            VectorT v2(3, 3);
            v2 = v;

            SKETCH_TEST_ASSERT(DollT::instances() == 10);

            SKETCH_TEST_ASSERT(v.size() == 5);
            SKETCH_TEST_ASSERT(!v.empty());

            SKETCH_TEST_ASSERT(v2.size() == 5);
            SKETCH_TEST_ASSERT(!v2.empty());

            for (const auto& e : v2)
            {
                SKETCH_TEST_ASSERT(e.m_value == 77);
            }
        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);
    }

    {
        VectorT v(5, 65);
        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        {
            VectorT v2(sketch::move(v));
            SKETCH_TEST_ASSERT(DollT::instances() == 5);

            SKETCH_TEST_ASSERT(v.empty());
            SKETCH_TEST_ASSERT(v.size() == 0);
            SKETCH_TEST_ASSERT(v.begin() == v.end());
            SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
            SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
            SKETCH_TEST_ASSERT(v.crbegin() == v.crend());

            SKETCH_TEST_ASSERT(v2.size() == 5);
            SKETCH_TEST_ASSERT(!v2.empty());

            for (const auto& e : v2)
            {
                SKETCH_TEST_ASSERT(e.m_value == 65);
            }

        }

        SKETCH_TEST_ASSERT(DollT::instances() == 0);
    }

    {
        VectorT v(5, 6);
        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        {
            VectorT v2(6, 5);
            v2 = sketch::move(v);
            SKETCH_TEST_ASSERT(DollT::instances() == 5);

            SKETCH_TEST_ASSERT(v.empty());
            SKETCH_TEST_ASSERT(v.size() == 0);
            SKETCH_TEST_ASSERT(v.begin() == v.end());
            SKETCH_TEST_ASSERT(v.rbegin() == v.rend());
            SKETCH_TEST_ASSERT(v.cbegin() == v.cend());
            SKETCH_TEST_ASSERT(v.crbegin() == v.crend());

            SKETCH_TEST_ASSERT(v2.size() == 5);
            SKETCH_TEST_ASSERT(!v2.empty());

            for (const auto& e : v2)
            {
                SKETCH_TEST_ASSERT(e.m_value == 6);
            }

        }

        SKETCH_TEST_ASSERT(DollT::instances() == 0);
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::assign()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.assign(5, DollT(6));

        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 6);
        }


        v.assign(1, DollT(2));

        SKETCH_TEST_ASSERT(v.size() == 1);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 1);

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 2);
        }


        v.assign(90, DollT(555));

        SKETCH_TEST_ASSERT(v.size() == 90);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 90);

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 555);
        }

        VectorT v2;
        v2.assign(v.begin(), v.end());
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::push_back()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.push_back(sketch::move(DollT(1)));
        DollT d(2);
        v.push_back(d);
        v.push_back(DollT(3));
        v.push_back(DollT(4));
        v.push_back(DollT(5));

        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 6);

        for (int i = 0; i < 5; i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == (i + 1));
        }

        for (size_t j = 5; j > 0; j--)
        {
            v.pop_back();

            SKETCH_TEST_ASSERT(v.size() == j - 1);
            SKETCH_TEST_ASSERT(DollT::instances() == long(j));

            for (size_t i = 0; i < j - 1; i++)
            {
                SKETCH_TEST_ASSERT(v[i].m_value == long(i + 1));
            }
        }

        SKETCH_TEST_ASSERT(v.empty());
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::erase()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.assign(5, DollT(6));

        auto it = v.begin();
        v.erase(it);

        SKETCH_TEST_ASSERT(v.size() == 4);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 4);

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 6);
        }

        it = v.begin();
        it++;
        v.erase(it);

        SKETCH_TEST_ASSERT(v.size() == 3);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 3);

        for (const auto& e : v)
        {
            SKETCH_TEST_ASSERT(e.m_value == 6);
        }

        do
        {
            it = v.begin();
            if (it == v.end())
            {
                break;
            }

            v.erase(it);

        } while (true);

        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.empty());
    }

    {
        VectorT v;
        v.assign(5, DollT(6));

        v.erase(v.begin(), v.end());

        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 0);
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::resize()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.assign(5, DollT(6));

        SKETCH_TEST_ASSERT(DollT::instances() == 5);
        v.resize(10);
        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == ((i < 5) ? 6 : 0));
        }
        SKETCH_TEST_ASSERT(DollT::instances() == 10);

        SKETCH_TEST_ASSERT(v.size() == 10);
        SKETCH_TEST_ASSERT(!v.empty());

        v.resize(2);
        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == 6);
        }
        SKETCH_TEST_ASSERT(DollT::instances() == 2);

        SKETCH_TEST_ASSERT(v.size() == 2);
        SKETCH_TEST_ASSERT(!v.empty());

        v.resize(0);
        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.empty());
    }

    {
        VectorT v;
        v.assign(5, DollT(6));

        SKETCH_TEST_ASSERT(DollT::instances() == 5);
        v.resize(10, DollT(8));
        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == ((i < 5) ? 6 : 8));
        }
        SKETCH_TEST_ASSERT(DollT::instances() == 10);

        SKETCH_TEST_ASSERT(v.size() == 10);
        SKETCH_TEST_ASSERT(!v.empty());

        v.resize(2, DollT(8));
        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == 6);
        }
        SKETCH_TEST_ASSERT(DollT::instances() == 2);

        SKETCH_TEST_ASSERT(v.size() == 2);
        SKETCH_TEST_ASSERT(!v.empty());

        v.resize(0, DollT(8));
        SKETCH_TEST_ASSERT(DollT::instances() == 0);

        SKETCH_TEST_ASSERT(v.size() == 0);
        SKETCH_TEST_ASSERT(v.empty());
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::reserve()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.assign(5, DollT(6));

        SKETCH_TEST_ASSERT(DollT::instances() == 5);
        v.reserve(10);

        SKETCH_TEST_ASSERT(v.capacity() == 10);
        SKETCH_TEST_ASSERT(v.size() == 5);

        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == 6);
        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);


        v.reserve(2);

        SKETCH_TEST_ASSERT(v.capacity() == 10);
        SKETCH_TEST_ASSERT(v.size() == 5);

        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == 6);
        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);


        v.reserve(0);

        SKETCH_TEST_ASSERT(v.capacity() == 10);
        SKETCH_TEST_ASSERT(v.size() == 5);

        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == 6);
        }

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::emplace()")
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

    {
        VectorT v;
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        v.emplace_back(4);
        v.emplace_back(5);

        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(!v.empty());

        SKETCH_TEST_ASSERT(DollT::instances() == 5);

        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == long(i + 1));
        }

        auto it = v.end();
        it = v.emplace(it, 11);
        SKETCH_TEST_ASSERT(it->m_value == 11);
        SKETCH_TEST_ASSERT(v.size() == 6);
        SKETCH_TEST_ASSERT(!v.empty());
        SKETCH_TEST_ASSERT(DollT::instances() == 6);

        for (size_t i = 0; i < v.size(); i++)
        {
            SKETCH_TEST_ASSERT(v[i].m_value == long((i < 5) ? (i + 1) : 11));
        }

        it = v.begin();
        it = v.emplace(it, 12);
        SKETCH_TEST_ASSERT(it->m_value == 12);
        SKETCH_TEST_ASSERT(v.size() == 7);
        SKETCH_TEST_ASSERT(!v.empty());
        SKETCH_TEST_ASSERT(DollT::instances() == 7);

        for (size_t i = 0; i < v.size(); i++)
        {
            if (i == 0)
                SKETCH_TEST_ASSERT(v[i].m_value == 12);
            else if (i == 6)
                SKETCH_TEST_ASSERT(v[i].m_value == 11);
            else
                SKETCH_TEST_ASSERT(v[i].m_value == long(i));
        }
    }

    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()


SKETCH_TEST_BEGIN(Vector, "Vector::insert()")

    SKETCH_TEST_ASSERT(DollT::instances() == 0);
    {
        VectorT v;
        DollT d(1);

        auto it = v.insert(v.begin(), d);
        SKETCH_TEST_ASSERT(it->m_value == 1);

        it = v.insert(v.end(), sketch::move(DollT(2)));
        SKETCH_TEST_ASSERT(it->m_value == 2);

        it = v.insert(v.end(), 3, DollT(3));
        SKETCH_TEST_ASSERT(it->m_value == 3);

        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(!v.empty());
        SKETCH_TEST_ASSERT(DollT::instances() == 6);


        VectorT v2;
        v2.push_back(1);
        v2.push_back(2);
        v2.push_back(3);
        v2.push_back(4);
        v2.push_back(5);

        SKETCH_TEST_ASSERT(DollT::instances() == 11);

        v.insert(v.end(), v2.begin(), v2.end());

        SKETCH_TEST_ASSERT(DollT::instances() == 16);

        for (size_t i = 0; i < v.size(); i++)
        {
            if (i == 0)
                SKETCH_TEST_ASSERT(v[i].m_value == 1);
            else if (i == 1)
                SKETCH_TEST_ASSERT(v[i].m_value == 2);
            else if (i < 5)
                SKETCH_TEST_ASSERT(v[i].m_value == 3);
            else
                SKETCH_TEST_ASSERT(v[i].m_value == long(i - 4));
        }
    }
    SKETCH_TEST_ASSERT(DollT::instances() == 0);

SKETCH_TEST_END()
