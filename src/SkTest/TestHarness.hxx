#include <Sketch.hxx>

#include <SkBase/Thread.hxx>


class TestHarness
    : public sketch::test::ITestHarness
    , public sketch::NonCopyable
    , private sketch::ISimpleDelegate
{
public:
    ~TestHarness() noexcept;
    TestHarness();

    bool valid() const noexcept
    {
        return m_thread.valid();
    }

    void wait() noexcept
    {
        if (valid())
        {
            m_thread.join();
        }
    }

private:
    bool sketch::test::ITestHarness::shouldAbort() const override;
    void sketch::test::ITestHarness::startTest(sketch::test::TestBase* test) override;
    void sketch::test::ITestHarness::pass() override;
    void sketch::test::ITestHarness::fail(const char* expr, const char* file, int line) override;
    void sketch::test::ITestHarness::finishTest() override;

private:
    void sketch::ISimpleDelegate::run(void* ctx) override;

private:
    struct Context
    {
        sketch::test::TestBase* test;
        size_t passed;
        size_t failed;

        Context(sketch::test::TestBase* _test = nullptr)
            : test(_test)
            , passed(0)
            , failed(0)
        {
        }

        bool valid() const
        {
            return !!test;
        }
    };

    Context m_context;
    bool m_inside;
    bool m_abort;

    sketch::Thread m_thread;
};
