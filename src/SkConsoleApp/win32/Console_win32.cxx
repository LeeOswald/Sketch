#include <SkConsoleApp/Console.hxx>

#include <SkBase/Mutex.hxx>

namespace
{

uint8_t _ForegroundColors[16] =
{
    (0),
    (FOREGROUND_BLUE),
    (FOREGROUND_GREEN),
    (FOREGROUND_GREEN | FOREGROUND_BLUE),
    (FOREGROUND_RED),
    (FOREGROUND_RED | FOREGROUND_BLUE),
    (FOREGROUND_RED | FOREGROUND_GREEN),
    (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE),
    (FOREGROUND_INTENSITY),
    (FOREGROUND_INTENSITY | FOREGROUND_BLUE),
    (FOREGROUND_INTENSITY | FOREGROUND_GREEN),
    (FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE),
    (FOREGROUND_INTENSITY | FOREGROUND_RED),
    (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE),
    (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN),
    (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
};


uint8_t _BackgroundColors[16] =
{
    (0),
    (BACKGROUND_BLUE),
    (BACKGROUND_GREEN),
    (BACKGROUND_GREEN | BACKGROUND_BLUE),
    (BACKGROUND_RED),
    (BACKGROUND_RED | BACKGROUND_BLUE),
    (BACKGROUND_RED | BACKGROUND_GREEN),
    (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE),
    (BACKGROUND_INTENSITY),
    (BACKGROUND_INTENSITY | BACKGROUND_BLUE),
    (BACKGROUND_INTENSITY | BACKGROUND_GREEN),
    (BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE),
    (BACKGROUND_INTENSITY | BACKGROUND_RED),
    (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE),
    (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN),
    (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
};


struct GlobalConsoleData
{
    sketch::Mutex lock;
};

static GlobalConsoleData* g_Console = nullptr;

} // namespace {}


namespace sketch
{

namespace internal
{

bool Console::initialize()
{
    SK_ASSERT(!g_Console);
    if (g_Console)
    {
        return false;
    }

    g_Console = new (sketch::nothrow) GlobalConsoleData;
    if (!g_Console)
    {
        return false;
    }

    return true;
}

void Console::finalize()
{
    auto p = g_Console;
    g_Console = nullptr;

    if (p)
    {
        delete p;
    }
}

sketch::Pair<uint32_t, uint32_t> Console::windowSize()
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(Stream::StdOut);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    if (_isRedirectedStream(h))
    {
        return sketch::Pair<uint32_t, uint32_t>(120, 50); // some default values
    }

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
    if (!::GetConsoleScreenBufferInfo(h, &csbiInfo))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    return sketch::Pair<uint32_t, uint32_t>(csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1, csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1);
}

sketch::Pair<uint32_t, uint32_t> Console::cursorPosition()
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(Stream::StdOut);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    if (_isRedirectedStream(h))
    {
        return sketch::Pair<uint32_t, uint32_t>(0, 0); // some default values
    }

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
    if (!::GetConsoleScreenBufferInfo(h, &csbiInfo))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return sketch::Pair<uint32_t, uint32_t>(0, 0);
    }

    return sketch::Pair<uint32_t, uint32_t>(csbiInfo.dwCursorPosition.X, csbiInfo.dwCursorPosition.Y);
}

bool Console::setCursorPosition(uint32_t x, uint32_t y)
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return false;
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(Stream::StdOut);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    if (_isRedirectedStream(h))
    {
        return true; // just ignore the position
    }

    COORD c = { static_cast<SHORT>(x), static_cast<SHORT>(y) };

    if (!::SetConsoleCursorPosition(h, c))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    return true;
}

String Console::title()
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return String();
    }

    Mutex::Guard g(g_Console->lock);

    wchar_t title[512];
    if (!::GetConsoleTitleW(title, _countof(title)))
    {
        return String();
    }

    return String(title);
}

bool Console::setTitle(const String& title)
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return false;
    }

    Mutex::Guard g(g_Console->lock);

    if (!::SetConsoleTitleW(title.utf16()))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    return true;
}

bool Console::_writeRedirected(HANDLE h, const String& text)
{
    auto a = sketch::toSystemLocale(text);
    if (!::WriteFile(h, a.str(), static_cast<DWORD>(a.length() * sizeof(char)), nullptr, nullptr))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    return true;
}

bool Console::write(Stream stream, const String& text, const TextAttributes& ta)
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return false;
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(stream);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    if (_isRedirectedStream(h))
    {
        return _writeRedirected(h, text);
    }

    bool result = false;

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
    if (::GetConsoleScreenBufferInfo(h, &csbiInfo))
    {
        SK_ASSERT(ta.textColor < _countof(_ForegroundColors));
        SK_ASSERT(ta.backColor < _countof(_BackgroundColors));

        WORD wa = _ForegroundColors[ta.textColor & 0x0f];
        wa |= _BackgroundColors[ta.backColor & 0x0f];

        if (!::SetConsoleTextAttribute(h, wa))
        {
            setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        }
        else if (!::WriteConsoleW(h, text.str(), DWORD(text.length()), nullptr, nullptr))
        {
            setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        }
        else
        {
            result = true;
        }

        ::SetConsoleTextAttribute(h, csbiInfo.wAttributes);
    }

    return true;
}

bool Console::write(Stream stream, const String& text)
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return false;
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(stream);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    if (_isRedirectedStream(h))
    {
        return _writeRedirected(h, text);
    }

    if (!::WriteConsoleW(h, text.str(), DWORD(text.length()), nullptr, nullptr))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    return true;
}

ConsoleBase::TextAttributes Console::textAttributes(Stream stream) const
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return TextAttributes();
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(stream);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return TextAttributes();
    }

    if (_isRedirectedStream(h))
    {
        TextAttributes{ Color::Black, Color::White }; // do not fail, but return some "default" attributes
    }

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
    ::GetConsoleScreenBufferInfo(h, &csbiInfo);

    return TextAttributes{ Color((csbiInfo.wAttributes & 0xf0) >> 4), Color(csbiInfo.wAttributes & 0x0f) };
}

bool Console::setTextAttributes(Stream stream, const TextAttributes& ta)
{
    if (!g_Console)
    {
        setLastError(GenericError::make(GenericError::EInvalidRequest, String(), __FILE__, __LINE__));
        return false;
    }

    Mutex::Guard g(g_Console->lock);

    auto h = _getStream(stream);
    SK_ASSERT(h != INVALID_HANDLE_VALUE);
    if (h == INVALID_HANDLE_VALUE)
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    if (_isRedirectedStream(h))
    {
        return true;
    }

    SK_ASSERT(ta.textColor < _countof(_ForegroundColors));
    SK_ASSERT(ta.backColor < _countof(_BackgroundColors));

    WORD wa = _ForegroundColors[ta.textColor & 0x0f];
    wa |= _BackgroundColors[ta.backColor & 0x0f];

    if (!::SetConsoleTextAttribute(h, wa))
    {
        setLastError(win32::Error::make(win32::lastError(), String(), __FILE__, __LINE__));
        return false;
    }

    return true;
}


} // namespace internal {}

} // namespace sketch {}
