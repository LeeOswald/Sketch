#pragma once

#include <Sketch.hxx>

#if defined(SKETCH_WIN_UM)
#include <SkConsoleApp/win32/ConsoleApplication_win32.hxx>

namespace sketch
{

namespace test
{


class TestApplication
    : public sketch::win32::ConsoleApplication
{
public:
    typedef sketch::win32::ConsoleApplication base;

    TestApplication(const char* name) noexcept
        : m_moduleName(name)
    {
    }

    const char* moduleName() const noexcept override
    {
        return m_moduleName;
    }

    int main(int argc, wchar_t** argv) noexcept override;

private:
    const char* m_moduleName;
};

} // namespace test {}

} // namespace sketch {}

#define SKETCH_TEST_MODULE(name) \
    sketch::test::TestApplication g_ ## name ## module(#name)


#elif defined(SKETCH_WIN_KM)
#include <SkDriver/nt/Driver_nt.hxx>

namespace sketch
{

namespace test
{

class TestDriver
    : public sketch::nt::Driver
{
public:
    typedef sketch::nt::Driver base;

    TestDriver(const char* name) noexcept
        : m_moduleName(name)
    {
    }

    const char* moduleName() const noexcept override
    {
        return m_moduleName;
    }

    NTSTATUS driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath) noexcept override;
    void driverUnload(DRIVER_OBJECT* driverObject) noexcept override;

private:
    const char* m_moduleName;
};

} // namespace test {}

} // namespace sketch {}


#define SKETCH_TEST_MODULE(name) \
    sketch::test::TestDriver g_ ## name ## module(#name);


#endif


namespace sketch
{

namespace test
{

struct ITest;
struct ITestHarness;


class TestBase
{
public:
    explicit TestBase(const char* name);

    static size_t count()
    {
        return _count;
    }

    static TestBase* first()
    {
        return _first;
    }

    TestBase* next() const
    {
        return m_next;
    }

    String name() const
    {
        return String(m_name);
    }

    virtual bool run(ITestHarness* harness) = 0;

private:
    static TestBase* _first;
    static size_t _count;

    TestBase* m_next;
    const char* m_name;
};


struct ITestHarness
{
    virtual bool shouldAbort() const = 0;
    virtual void startTest(TestBase* test) = 0;
    virtual void pass() = 0;
    virtual void fail(const char* expr, const char* file, int line) = 0;
    virtual void finishTest() = 0;
};


} // namespace test {}

} // namespace sketch {}


#define __SKETCH_TEST_NAME_(base, unique) base ## unique
#define __SKETCH_TEST_NAME(base, unique) __SKETCH_TEST_NAME_(base, unique)



#define SKETCH_TEST_START_UNIQUE(name, display) \
class name\
    : public sketch::test::TestBase\
{\
public:\
    name()\
        : sketch::test::TestBase(display)\
    {\
    }\
\
    bool run(sketch::test::ITestHarness* harness) override;\
};\
\
static name __SKETCH_TEST_NAME(s_, name);\
\
bool name::run(sketch::test::ITestHarness* _harness)\
{\
    if (_harness->shouldAbort()) return false;\
    _harness->startTest(this);\



#define SKETCH_TEST_BEGIN(name, display) SKETCH_TEST_START_UNIQUE(__SKETCH_TEST_NAME(name, __COUNTER__), display)


#define SKETCH_TEST_END() \
    _harness->finishTest();\
    return true;\
}\


#define SKETCH_TEST_ASSERT(expr) \
    do  \
    { \
        if (_harness->shouldAbort()) return false;\
        if (expr)\
        {\
            _harness->pass();\
        }\
        else\
        {\
            _harness->fail(#expr, __FILE__, __LINE__); \
        }\
    } while (false)

