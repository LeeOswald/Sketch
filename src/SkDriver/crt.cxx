#include "crt.hxx"

#include <SkBase/AtomicOps.hxx>


#pragma comment(linker, "/merge:.CRT=.rdata")

#pragma data_seg(push)

/* c++ initializers */
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

/* c++ terminators */
#pragma section(".CRT$XPA", long, read)
#pragma section(".CRT$XPZ", long, read)

/* termination */
#pragma section(".CRT$XTA", long, read)
#pragma section(".CRT$XTZ", long, read)


__declspec(allocate(".CRT$XCA")) sketch::crt::vfv_t* __xc_a[] = { nullptr };
__declspec(allocate(".CRT$XCZ")) sketch::crt::vfv_t* __xc_z[] = { nullptr };

__declspec(allocate(".CRT$XPA")) sketch::crt::vfv_t* __xp_a[] = { nullptr };
__declspec(allocate(".CRT$XPZ")) sketch::crt::vfv_t* __xp_z[] = { nullptr };

__declspec(allocate(".CRT$XTA")) sketch::crt::vfv_t* __xt_a[] = { nullptr };
__declspec(allocate(".CRT$XTZ")) sketch::crt::vfv_t* __xt_z[] = { nullptr };

#pragma data_seg(pop)



namespace
{

static const size_t exit_list_max = 1024 * 4;
static sketch::crt::vfv_t* exit_list[exit_list_max];
static volatile uint32_t exit_list_count = 0;


void initterm(sketch::crt::vfv_t** from, sketch::crt::vfv_t** to)
{
    while (from < to)
    {
        if (*from)
        {
            (*from)();
        }

        ++from;
    }
}

sketch::crt::vfv_t* onexit(sketch::crt::vfv_t* func)
{
    if (func)
    {
        uint32_t idx = sketch::atomic::exchange_add(&exit_list_count, 1) - 1;
        if (idx < _countof(exit_list))
        {
            return exit_list[idx] = func;
        }
    }

    return 0;
}

void doexit()
{
    for (sketch::crt::vfv_t** f = exit_list + exit_list_count - 1; f >= exit_list; --f)
    {
        (*f)();
    }
}

} // namespace {}


namespace sketch
{

namespace crt
{

bool initialize()
{
    // init static objects
    initterm(__xc_a, __xc_z);
    initterm(__xp_a, __xp_z);
    initterm(__xt_a, __xt_z);

    return true;
}

void finalize()
{
    // free static objects
    doexit();
}


} // namespace crt {}

} // namespace sketch {}


extern "C" int __cdecl atexit(sketch::crt::vfv_t func)
{
    return onexit(func) ? 0 : -1;
}


