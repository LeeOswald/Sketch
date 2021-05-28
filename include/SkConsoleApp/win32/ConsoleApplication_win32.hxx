#pragma once

#include <SkConsoleApp/Console.hxx>

#ifndef SKETCH_WIN_UM
#error This module is user-mode only
#endif


namespace sketch
{

namespace win32
{

class ConsoleApplication
    : public NonCopyable
    , public ErrorState
{
public:
    static int StaticMain(int argc, wchar_t** argv) noexcept;

    inline static ConsoleApplication* instance() noexcept
    {
        return s_instance;
    }

    ~ConsoleApplication() noexcept
    {
        SK_ASSERT(s_instance != nullptr);
        s_instance = nullptr;
    }

    virtual const char* moduleName() const noexcept = 0;
    virtual int main(int argc, wchar_t** argv) noexcept = 0;

protected:
    ConsoleApplication() noexcept
        : ErrorState(ErrorState::Mode::neverThrow)
    {
        SK_ASSERT(s_instance == nullptr);
        s_instance = this;
    }

private:
    int _main(int argc, wchar_t** argv) noexcept;

private:
    static ConsoleApplication* s_instance;
};


} // namespace win32 {}

} // namespace sketch {}
