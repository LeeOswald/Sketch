#include <Sketch.hxx>


namespace sketch
{

namespace
{

String formatMessage(long r, HINSTANCE module = 0) noexcept
{
    DWORD flags;
    if (!module)
    {
        flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    }
    else
    {
        flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS;
    }

    wchar_t* buffer = nullptr;
    DWORD cch = ::FormatMessageW(
        flags,
        module,
        r,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        reinterpret_cast<wchar_t*>(&buffer),
        0,
        nullptr
    );

    String s;

    if (cch > 0)
    {
        s = String(buffer, cch).trimRight(); // for whatever reason they append \n
        ::HeapFree(::GetProcessHeap(), 0, buffer);
    }

    return s;
}

} // namespace {}

namespace win32
{

String getErrorText(error_t r) noexcept
{
    if (r == S_OK)
    {
        return String();
    }

    return formatMessage(r);
}


} // namespace win32 {}


namespace nt
{

String getErrorText(error_t r) noexcept
{
    if (r == STATUS_SUCCESS)
    {
        return String();
    }

    auto h = ::GetModuleHandleW(L"ntdll.dll");
    SK_ASSERT(h);
    if (!h)
    {
        return String();
    }

    return formatMessage(r, h);
}


} // namespace nt {}

} // namespace sketch {}
