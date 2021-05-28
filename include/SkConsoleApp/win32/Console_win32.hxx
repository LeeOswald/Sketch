#pragma once


namespace sketch
{

namespace internal
{

class Console
    : public ConsoleBase
{
public:
    static bool initialize();
    static void finalize();

    inline bool isRedirectedStream(Stream stream)
    {
        return _isRedirectedStream(_getStream(stream));
    }

    sketch::Pair<uint32_t, uint32_t> windowSize();
    sketch::Pair<uint32_t, uint32_t> cursorPosition();
    bool setCursorPosition(uint32_t x, uint32_t y);

    String title();
    bool setTitle(const String& title);
    bool write(Stream stream, const String& text);
    bool write(Stream stream, const String& text, const TextAttributes& ta);
    TextAttributes textAttributes(Stream stream) const;
    bool setTextAttributes(Stream stream, const TextAttributes& ta);

private:
    static inline HANDLE _getStream(Stream stream)
    {
        switch (stream)
        {
        case sketch::ConsoleBase::StdIn:
            return GetStdHandle(STD_INPUT_HANDLE);
        case sketch::ConsoleBase::StdOut:
            return GetStdHandle(STD_OUTPUT_HANDLE);
        case sketch::ConsoleBase::StdErr:
            return GetStdHandle(STD_ERROR_HANDLE);
        }

        return INVALID_HANDLE_VALUE;
    }

    static inline bool _isRedirectedStream(HANDLE h)
    {
        DWORD m = 0;
        return !::GetConsoleMode(h, &m);
    }

    bool _writeRedirected(HANDLE h, const String& text);
};


} // namespace internal {}

} // namespace sketch {}
