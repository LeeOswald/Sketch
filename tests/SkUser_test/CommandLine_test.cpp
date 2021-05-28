#include "common.h"

#include <SkBase/Algorithm.hxx>
#include <SkUser/CommandLine.hxx>


using namespace sketch;


SKETCH_TEST_BEGIN(CommandLine, "CommandLine::CommandLine()")
    CommandLine cl(
        String(R"(/GS /analyze- /W4 /wd"4748" /wd"4603" /wd"4627" /wd"4986" /wd"4987" /wd"4996" /Gy /Zc:wchar_t-
               /I"Sketch\build\msbuild\\..\..\\include\"
               /I"Sketch\build\msbuild\..\..\tmp\SkBase_test\nt\DebugNT_Win32\"
               /analyze:"stacksize1024" /Zi /Gm- /Od /Fd"Sketch\build\msbuild\..\..\tmp\SkBase_test\nt\DebugNT_Win32\vc140.pdb"
               /FI"C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\shared\warning.h"
               /Zc:inline /fp:precise /Zp8 /D "KERNEL_MODE" /D "_X86_=1" /D "i386=1" /D "STD_CALL" /D "DEPRECATE_DDK_FUNCTIONS=1"
               /D "MSC_NOOPT" /D           "_WIN32_WINNT=0x0601" /D "WINVER=0x0601" /D "WINNT=1" /D "NTDDI_VERSION=0x06010000" /D "DBG=1"
               /errorReport:prompt          /GF /WX /Zc:forScope /GR- /Gz /Oy- /Oi /MTd
               /Fa"Sketch\build\msbuild\..\..\tmp\SkBase_test\nt\DebugNT_Win32\"
               /nologo /Fo"Sketch\build\msbuild\..\..\tmp\SkBase_test\nt\DebugNT_Win32\"
               /Fp"Sketch\build\msbuild\..\..\tmp\SkBase_test\nt\DebugNT_Win32\SkBase_test.pch" /D "" --help)")
    );

    SKETCH_TEST_ASSERT(cl.present("GS"));
    SKETCH_TEST_ASSERT(cl.present("analyze-"));
    SKETCH_TEST_ASSERT(cl.present("W4"));
    SKETCH_TEST_ASSERT(cl.present("wd"));
    SKETCH_TEST_ASSERT(cl.present("Gy"));
    SKETCH_TEST_ASSERT(cl.present("Zc"));
    SKETCH_TEST_ASSERT(cl.present("I"));
    SKETCH_TEST_ASSERT(cl.present("analyze"));
    SKETCH_TEST_ASSERT(cl.present("Zi"));
    SKETCH_TEST_ASSERT(cl.present("Gm-"));
    SKETCH_TEST_ASSERT(cl.present("Od"));
    SKETCH_TEST_ASSERT(cl.present("Fd"));
    SKETCH_TEST_ASSERT(cl.present("FI"));
    SKETCH_TEST_ASSERT(cl.present("Zc"));
    SKETCH_TEST_ASSERT(cl.present("fp"));
    SKETCH_TEST_ASSERT(cl.present("Zp8"));
    SKETCH_TEST_ASSERT(cl.present("D"));
    SKETCH_TEST_ASSERT(cl.present("errorReport"));
    SKETCH_TEST_ASSERT(cl.present("GF"));
    SKETCH_TEST_ASSERT(cl.present("WX"));
    SKETCH_TEST_ASSERT(cl.present("GR-"));
    SKETCH_TEST_ASSERT(cl.present("Gz"));
    SKETCH_TEST_ASSERT(cl.present("Oy-"));
    SKETCH_TEST_ASSERT(cl.present("Oi"));
    SKETCH_TEST_ASSERT(cl.present("MTd"));
    SKETCH_TEST_ASSERT(cl.present("Fa"));
    SKETCH_TEST_ASSERT(cl.present("nologo"));
    SKETCH_TEST_ASSERT(cl.present("Fo"));
    SKETCH_TEST_ASSERT(cl.present("Fp"));
    SKETCH_TEST_ASSERT(cl.present("help"));

    SKETCH_TEST_ASSERT(!cl.present("aaa"));

    SKETCH_TEST_ASSERT(cl.values("aaa").empty());

    {
        auto wd = cl.values("wd");
        SKETCH_TEST_ASSERT(wd.size() == 6);

        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4748")) != wd.end());
        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4603")) != wd.end());
        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4627")) != wd.end());
        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4986")) != wd.end());
        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4987")) != wd.end());
        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("4996")) != wd.end());

        SKETCH_TEST_ASSERT(find(wd.begin(), wd.end(), String("0000")) == wd.end());
    }

    {
        auto d = cl.values("D");
        SKETCH_TEST_ASSERT(d.size() == 11);

        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("KERNEL_MODE")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("_X86_=1")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("i386=1")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("STD_CALL")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("DEPRECATE_DDK_FUNCTIONS=1")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("MSC_NOOPT")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("_WIN32_WINNT=0x0601")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("WINVER=0x0601")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("WINNT=1")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("NTDDI_VERSION=0x06010000")) != d.end());
        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("DBG=1")) != d.end());

        SKETCH_TEST_ASSERT(find(d.begin(), d.end(), String("DBG=2")) == d.end());
    }

SKETCH_TEST_END()

SKETCH_TEST_BEGIN(CommandLine, "CommandLine::singleValue()")
    CommandLine cl(String(R"(-L:1 -L:2 -L:3.5 -L"5.6" -D"text with spaces " -D:3 -L:6 999 888 "nameless value" -K:0.09 -M:6 -N:h)"));

    {
        auto v = cl.values(L"L");
        SKETCH_TEST_ASSERT(v.size() == 5);
        SKETCH_TEST_ASSERT(v[0] == L"1");
        SKETCH_TEST_ASSERT(v[1] == L"2");
        SKETCH_TEST_ASSERT(v[2] == L"3.5");
        SKETCH_TEST_ASSERT(v[3] == L"5.6");
        SKETCH_TEST_ASSERT(v[4] == L"6");

        String sv;
        SKETCH_TEST_ASSERT(cl.singleValue(sv, L"L"));
        SKETCH_TEST_ASSERT(sv == L"1");
    }

    {
        auto v = cl.values(L"D");
        SKETCH_TEST_ASSERT(v.size() == 2);
        SKETCH_TEST_ASSERT(v[0] == L"text with spaces ");
        SKETCH_TEST_ASSERT(v[1] == L"3");

        String sv;
        SKETCH_TEST_ASSERT(cl.singleValue(sv, L"D"));
        SKETCH_TEST_ASSERT(sv == L"text with spaces ");
    }

    {
        auto v = cl.values();
        SKETCH_TEST_ASSERT(v.size() == 3);
        SKETCH_TEST_ASSERT(v[0] == L"999");
        SKETCH_TEST_ASSERT(v[1] == L"888");
        SKETCH_TEST_ASSERT(v[2] == L"nameless value");

        String sv;
        SKETCH_TEST_ASSERT(cl.singleValue(sv));
        SKETCH_TEST_ASSERT(sv == L"999");
    }

#ifdef SKETCH_STL_AVAILABLE

    {
        long l = 0;
        SKETCH_TEST_ASSERT(cl.singleValue(l, L"M"));
        SKETCH_TEST_ASSERT(l == 6);

        SKETCH_TEST_ASSERT(!cl.singleValue(l, L"N"));
    }

    {
        double l = 0;
        SKETCH_TEST_ASSERT(cl.singleValue(l, L"M"));
        SKETCH_TEST_ASSERT(l == 6);

        SKETCH_TEST_ASSERT(cl.singleValue(l, L"K"));
        SKETCH_TEST_ASSERT(l == 0.09);

        SKETCH_TEST_ASSERT(!cl.singleValue(l, L"N"));
    }

#endif // SKETCH_STL_AVAILABLE

SKETCH_TEST_END()
