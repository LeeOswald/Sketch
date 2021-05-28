#pragma once

namespace sketch
{

namespace nt
{

typedef NTSTATUS error_t;

__forceinline bool isErrorStatus(error_t r) noexcept
{
    return !NT_SUCCESS(r);
}

String getErrorText(error_t r) noexcept;


class Error
    : public ::sketch::Error
{
public:
    const UChar* typeName() const noexcept override
    {
        return u"nt::Error";
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
        : ::sketch::Error(::sketch::Error::nt, code, message, file, line, inner)
    {
    }
};


} // namespace nt {}

} // namespace sketch {}
