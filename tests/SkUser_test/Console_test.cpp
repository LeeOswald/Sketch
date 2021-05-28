#include "common.h"

#include <SkConsoleApp/Console.hxx>
#include <SkBase/Thread.hxx>


using namespace sketch;


SKETCH_TEST_BEGIN(Console, "Console")

    Console co;
    auto redirectedStdIn = co.isRedirectedStream(Console::StdIn);
    auto redirectedStdOut = co.isRedirectedStream(Console::StdOut);
    auto redirectedStdErr = co.isRedirectedStream(Console::StdErr);

    SketchInfo("STDIN redirected: %s", redirectedStdIn ? "YES" : "NO");
    SketchInfo("STDOUT redirected: %s", redirectedStdOut ? "YES" : "NO");
    SketchInfo("STDERR redirected: %s", redirectedStdErr ? "YES" : "NO");

    auto ws = co.windowSize();
    SketchInfo("Window size %dx%d", ws.first, ws.second);

    auto cp = co.cursorPosition();
    SketchInfo("Cursor position (%d:%d)", cp.first, cp.second);

    {
        SketchFlush();

        cp = co.cursorPosition();
        SketchInfo("Progress test: 0%%");

        for (int i = 1; i <= 100; i++)
        {
            co.setCursorPosition(cp.first, cp.second);
            SketchInfo("Progress test: %d%%", i);

            thread::current::sleep(50);

            SketchFlush();
        }
    }

    auto title = co.title();
    SketchInfo("Old console title: [%s]", toSystemLocale(title).str());
    co.setTitle("This is my title");

    auto ta = co.textAttributes(Console::StdOut);
    co.setTextAttributes(Console::StdOut, Console::TextAttributes{Console::White, Console::Red});
    co.write(Console::StdOut, "RED on WHITE\n");
    co.setTextAttributes(Console::StdOut, Console::TextAttributes{Console::White, Console::Green});
    co.write(Console::StdOut, "GREEN on WHITE\n");
    co.setTextAttributes(Console::StdOut, ta);
    co.write(Console::StdOut, "DEFAULT colors\n");

    SketchDebug("Debug message");
    SketchInfo("Info message");
    SketchWarning("Warning message");
    SketchError("Error message");
    SketchHighest("Highest message");

SKETCH_TEST_END()
