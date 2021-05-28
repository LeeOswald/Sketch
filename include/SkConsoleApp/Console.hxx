#pragma once

#include <SkBase/Pair.hxx>


namespace sketch
{

class ConsoleBase
    : public NonCopyable
    , public ErrorState
{
public:
    enum Color
    {
        Black = 0,
        DarkBlue,
        DarkGreen,
        DarkCyan,
        DarkRed,
        DarkMagenta,
        DarkYellow,
        DarkGray,
        Gray,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Yellow,
        White
    };

    enum Stream
    {
        StdIn = 0,
        StdOut,
        StdErr
    };

    struct TextAttributes
    {
        Color backColor;
        Color textColor;
    };
};


} // namespace sketch {}


#ifdef SKETCH_WIN_UM
#include "win32/Console_win32.hxx"
#endif


namespace sketch
{

class Console
    : public internal::Console
{
    typedef internal::Console base;

public:
    static inline bool initialize()
    {
        return base::initialize();
    }

    static inline void finalize()
    {
        base::finalize();
    }

    inline bool isRedirectedStream(Stream stream)
    {
        return base::isRedirectedStream(stream);
    }

    inline sketch::Pair<uint32_t, uint32_t> windowSize()
    {
        return base::windowSize();
    }

    inline sketch::Pair<uint32_t, uint32_t> cursorPosition()
    {
        return base::cursorPosition();
    }

    inline bool setCursorPosition(uint32_t x, uint32_t y)
    {
        return base::setCursorPosition(x, y);
    }

    inline bool setTitle(const String& title)
    {
        return base::setTitle(title);
    }

    inline bool write(Stream stream, const String& text)
    {
        return base::write(stream, text);
    }

    inline bool write(Stream stream, const String& text, const TextAttributes& ta)
    {
        return base::write(stream, text, ta);
    }

    inline TextAttributes textAttributes(Stream stream) const
    {
        return base::textAttributes(stream);
    }

    inline bool setTextAttributes(Stream stream, const TextAttributes& ta)
    {
        return base::setTextAttributes(stream, ta);
    }
};


namespace log
{

class TraceSink1
    : public sketch::RefCountedBase<sketch::log::ITraceSink>
{
public:
    static Ref make() noexcept
    {
        return Ref(new (sketch::nothrow) TraceSink1());
    }

    void write(sketch::log::Record::Ref r) noexcept override
    {
#ifdef SKETCH_CXX_EXCEPTIONS
        try
        {
#endif
            if (r)
            {
                auto s = r->text();
                s.append("\n", 1);
                auto l = sketch::fromSystemLocale(s.str(), s.length());

                auto stream = sketch::ConsoleBase::StdOut;
                auto ta = m_console.textAttributes(stream);

                switch (r->level())
                {
                case sketch::log::Debug:
                    ta.textColor = sketch::ConsoleBase::Gray;
                    break;
                case sketch::log::Info:
                    ta.textColor = sketch::ConsoleBase::White;
                    break;
                case sketch::log::Warning:
                    ta.textColor = sketch::ConsoleBase::Magenta;
                    break;
                case sketch::log::Error:
                    ta.textColor = sketch::ConsoleBase::Red;
                    break;
                case sketch::log::Highest:
                    ta.textColor = sketch::ConsoleBase::Yellow;
                    break;
                }

                m_console.write(stream, l, ta);
            }
#ifdef SKETCH_CXX_EXCEPTIONS
        }
        catch (sketch::Exception&)
        {
        }
#endif
    }

private:
    ~TraceSink1() noexcept
    {
    }

    TraceSink1() noexcept
    {
    }

private:
    sketch::Console m_console;
};

} // namespace log {}


} // namespace sketch {}
