#pragma once


namespace sketch
{

namespace log
{

bool bailOutWriteV(Level level, const char* module, const char* file, int line, const char* format, va_list args) noexcept;


inline bool bailOutWrite(Level level, const char* module, const char* file, int line, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    bailOutWriteV(level, module, file, line, format, args);

    va_end(args);

    return true;
}


} // namespace log {}

} // namespace sketch {}
