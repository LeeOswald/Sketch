#define HAVE_SKETCH_HXX

#include <Sketch.hxx>
#include <SkBase/Random.hxx>


#include "jansson_port.h"


extern "C"
{

void* __cdecl sketch_c_calloc(size_t _Count, size_t _Size)
{
    auto req = _Count * _Size;
    auto p = SkMalloc(req);
    if (p) memset(p, 0, req);
    return p;
}

void __cdecl sketch_c_free(void* _Block)
{
    SkFree(_Block);
}

void* __cdecl sketch_c_malloc(size_t _Size)
{
    return SkMalloc(_Size);
}

void sketch_c_rand(void* buffer, size_t size)
{
    sketch::random::generate(buffer, size);
}


} // extern "C" {}
