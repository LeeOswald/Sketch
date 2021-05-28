#pragma once

#include "IRefCounted.hxx"


namespace sketch
{

class Error
    : public RefCountedBase<IRefCounted>
{
public:
    enum Type
    {
        unspecified = 0,
        generic = 1,
        nt = 2,
        win32 = 3
    };

    typedef RefCountedPtr<Error> Ref;

    static Ref make(
        const String& message,
        long code = 0,
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
    ) noexcept
    {
        return Ref(new(sketch::nothrow) Error(
            unspecified,
            code,
            message,
            file,
            line,
            inner
        ));
    }

    Type type() const noexcept
    {
        return m_type;
    }

    virtual const UChar* typeName() const noexcept
    {
        return u"Error";
    }

    const String& message() const noexcept // this is the original text string passed to make()
    {
        return m_message;
    }

    const char* file() const noexcept
    {
        return m_file;
    }

    int line() const noexcept
    {
        return m_line;
    }

    Ref inner() const noexcept
    {
        return m_inner;
    }

    long code() const noexcept
    {
        return m_code;
    }

    virtual String errorText() const noexcept // this could be a textual representation of the error code
    {
        return String();
    }

    // {{ - left curly brace
    // {T} - type
    // {Cd} - error code (decimal)
    // {Cx} - error code (hex)
    // {M} - message that message() returns
    // {S} - text that errorText() returns
    // {MS} - {M} [{S}]
    // {F} - source file name
    // {L} - source line
    // {FL} - source file:line
    virtual String format(const UChar* format) const noexcept;

protected:
    virtual ~Error() noexcept
    {
    }

    Error(
        Type type,
        long code,
        const String& message,
        const char* file,
        int line,
        Ref inner
    ) noexcept
        : m_type(type)
        , m_code(code)
        , m_message(message)
        , m_file(file)
        , m_line(line)
        , m_inner(inner)
    {
    }

private:
    Type m_type;
    long m_code;
    String m_message;
    const char* m_file;
    int m_line;
    Ref m_inner;
};


class GenericError
    : public Error
{
public:
    enum
    {
        Success = 0,
        EUnknown = -1,
        EOutOfMemory = -2,
        ETimedOut = -3,
        EIvalidArg = -4,
        ENotImplemented = -5,
        EInvalidRequest = -6
    };

    const UChar* typeName() const noexcept override
    {
        return u"GenericError";
    }

    String errorText() const noexcept override;

    static Ref make(
        long code,
        const String& message = String(),
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
    ) noexcept
    {
        return Ref(new(sketch::nothrow) GenericError(
            code,
            message,
            file,
            line,
            inner
        ));
    }

private:
    GenericError(
        long code,
        const String& message,
        const char* file,
        int line,
        Ref inner
    ) noexcept
        : Error(Error::generic, code, message, file, line, inner)
    {
    }

};


void logError(const Error::Ref& r) noexcept;


} // namespace sketch {}


#include "nt/Error_nt.hxx"

#ifdef SKETCH_WIN_UM
#include "win32/Error_win32.hxx"
#endif


#define SkError(text) ::sketch::Error::make(text, 0, __FILE__, __LINE__)
#define SkError2(text, inner) sketch::Error::make(text, 0, __FILE__, __LINE__, inner)

#define SkGenericError(code, text) ::sketch::GenericError::make(code, text, __FILE__, __LINE__)
#define SkGenericError2(code, text, inner) ::sketch::GenericError::make(code, text, __FILE__, __LINE__, inner)

#ifdef SKETCH_WIN_UM

#define SkWin32Error(code, text) ::sketch::win32::Error::make(code, text, __FILE__, __LINE__)
#define SkWin32Error2(code, text, inner) ::sketch::win32::Error::make(code, text, __FILE__, __LINE__, inner)

#define SkWin32ErrorFromLastError(text) ::sketch::win32::Error::make(::sketch::win32::lastError(), text, __FILE__, __LINE__)
#define SkWin32ErrorFromLastError2(text, inner) ::sketch::win32::Error::make(::sketch::win32::lastError(), text, __FILE__, __LINE__, inner)

#endif // SKETCH_WIN_UM

#define SkNtError(code, text) ::sketch::nt::Error::make(code, text, __FILE__, __LINE__)
#define SkNtError2(code, text, inner) ::sketch::nt::Error::make(code, text, __FILE__, __LINE__, inner)


#define SkLogError(e) ::sketch::logError(e)

