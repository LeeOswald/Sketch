#include <SkConsoleApp/win32/ConsoleApplication_win32.hxx>
#include <SkBase/init.hxx>

#include <crtdbg.h>


namespace sketch
{

namespace win32
{

ConsoleApplication* ConsoleApplication::s_instance = nullptr;


int ConsoleApplication::StaticMain(int argc, wchar_t** argv) noexcept
{
    ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

#ifdef SKETCH_DBG
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif

    int result = -1;

    if (!sketch::SkBase::initialize())
    {
        return result;
    }

    if (!sketch::Console::initialize())
    {
        sketch::SkBase::finalize();
        return result;
    }

    auto a = instance();
    SK_ASSERT(a);
    if (a)
    {
        result = a->_main(argc, argv);
    }

    sketch::Console::finalize();
    sketch::SkBase::finalize();

    return result;
}

int ConsoleApplication::_main(int argc, wchar_t** argv) noexcept
{
    log::g_ModuleName = moduleName();
    return main(argc, argv);
}


} // namespace win32 {}

} // namespace sketch {}


extern "C" int __cdecl main(int argc, wchar_t** argv)
{
    return sketch::win32::ConsoleApplication::StaticMain(argc, argv);
}
