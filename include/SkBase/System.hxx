#pragma once

#include "../Sketch.hxx"



namespace sketch
{

namespace system
{


uint32_t processorCount() noexcept;

bool isDebuggerPresent() noexcept;


struct OsVersion
{
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t build;
    uint32_t spMajor;
    uint32_t spMinor;
};


OsVersion getOsVersion() noexcept;

uint32_t pageSize() noexcept;

String machineName();


} // namespace system {}

} // namespace sketch {}


#if defined(SKETCH_WIN_KM)
#include "nt/System_nt.hxx"
#elif defined(SKETCH_WIN_UM)
#include "win32/System_win32.hxx"
#endif
