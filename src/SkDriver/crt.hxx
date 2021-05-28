#pragma once

#include <Sketch.hxx>


namespace sketch
{

namespace crt
{
	
bool initialize();
void finalize();
	

typedef void __cdecl vfv_t(void);
typedef int  __cdecl ifv_t(void);
typedef void __cdecl vfi_t(int);

extern "C"
{

int __cdecl atexit(vfv_t func);

} // extern "C" {}


} // namespace crt {}

} // namespace sketch {}
