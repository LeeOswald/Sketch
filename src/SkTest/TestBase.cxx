#include <SkTest/SkTest.hxx>


namespace sketch
{

namespace test
{

TestBase* TestBase::_first = nullptr;
size_t TestBase::_count = 0;


TestBase::TestBase(const char* name)
    : m_next(nullptr)
    , m_name(name)
{

    if (!_first)
    {
        _first = this;
    }
    else
    {
        auto p = _first;
        while (p)
        {
            auto next = p->m_next;
            if (!next)
            {
                p->m_next = this;
                break;
            }

            p = next;
        }
    }

    _count++;
}


} // namespace test {}

} // sketch {}
