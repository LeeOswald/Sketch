#include <SkTest/SkTest.hxx>

#include "TestHarness.hxx"


TestHarness::~TestHarness() noexcept
{
    m_abort = true;
    if (m_thread.valid())
    {
        m_thread.join();
    }
}

TestHarness::TestHarness()
    : m_inside(false)
    , m_abort(false)
    , m_thread(this, "TestHarness_thread")
{
}

bool TestHarness::shouldAbort() const
{
    return m_abort;
}

void TestHarness::run(void* ctx)
{
    SK_UNUSED(ctx);

    size_t passed = 0;
    size_t failed = 0;
    bool aborted = false;

    SketchInfo("---------------------------------------------------------------------------------");
    SketchInfo("                              Running tests                                      ");
    SketchInfo("---------------------------------------------------------------------------------");

    auto p = sketch::test::TestBase::first();
    size_t current = 0;
    while (p)
    {
        if (m_abort)
        {
            aborted = true;
            break;
        }

#ifdef SKETCH_CXX_EXCEPTIONS
        try
        {
#endif
            if (!p->run(this))
            {
                aborted = true;
                break;
            }
#ifdef SKETCH_CXX_EXCEPTIONS
        }
        catch (sketch::Exception& e)
        {
            auto inner = e.error();
            auto f = inner->format(u"Unexpected exception: {MS} {FL}");

            auto l = sketch::toSystemLocale(f);
            SketchError(l.str());

            aborted = true;
            break;
        }
        catch (std::exception& e)
        {
            SketchError(e.what());
            aborted = true;
            break;
        }
#endif

        if (m_context.failed)
        {
            failed++;
            SketchInfo("Testcase #%d: failed", current + 1);
        }
        else if (m_context.passed)
        {
            passed++;
            SketchInfo("Testcase #%d: passed", current + 1);
        }
        else
        {
            SketchInfo("Testcase #%d: skipped", current + 1);
        }

        p = p->next();
        current++;
    }

    SketchInfo("---------------------------------------------------------------------------------");
    if (aborted)
    {
        SketchError("Test execution aborted");
    }
    else
    {
        SketchInfo("Passed: %d", passed);
        SketchInfo("Failed: %d", failed);
        SketchInfo("Total: %d", current);
    }
}

void TestHarness::startTest(sketch::test::TestBase* test)
{
    SK_ASSERT(!m_inside);
    m_inside = true;
    m_context = Context(test);
}

void TestHarness::pass()
{
    SK_ASSERT(m_inside);
    m_context.passed++;
}

void TestHarness::fail(const char* expr, const char* file, int line)
{
    SK_ASSERT(m_inside);
    m_context.failed++;

    SketchError("FAIL at [%s]:%d %s", file, line, expr);
}

void TestHarness::finishTest()
{
    SK_ASSERT(m_inside);
    m_inside = false;
}


