#pragma once


namespace sketch
{

namespace win32
{

typedef HRESULT error_t;

__forceinline bool isErrorStatus(error_t r) noexcept
{
    return ((HRESULT)r < 0);
}

String getErrorText(error_t r) noexcept;

__forceinline error_t lastError() noexcept
{
    return HRESULT_FROM_WIN32(::GetLastError());
}


class Error
    : public ::sketch::Error
{
public:
    const UChar* typeName() const noexcept override
    {
        return u"win32::Error";
    }

    String errorText() const noexcept override
    {
        return getErrorText(code());
    }

    static Ref make(
        error_t code,
        const String& message = String(),
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
    ) noexcept
    {
        return Ref(new(sketch::nothrow) Error(
            code,
            message,
            file,
            line,
            inner
        ));
    }

private:
    Error(
        long code,
        const String& message,
        const char* file,
        int line,
        ::sketch::Error::Ref inner
    ) noexcept
        : ::sketch::Error(::sketch::Error::win32, code, message, file, line, inner)
    {
    }
};


} // namespace win32 {}

} // namespace sketch {}
