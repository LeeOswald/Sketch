#include <SkBase/init.hxx>
#include <SkBase/RefCount.hxx>

#include "Heap_impl.hxx"
#include "Net_impl.hxx"
#include "Random_impl.hxx"
#include "Trace_impl.hxx"


struct ComponentRegistration
{
    typedef bool(*Initializer)();
    typedef void(*Finalizer)();

    Initializer initializer;
    Finalizer finalizer;
    const char* name;
};


namespace
{

sketch::refcount::type g_RefCount = 0;

ComponentRegistration g_Components[] =
{
    { sketch::random::initialize, sketch::random::finalize, "random" },
    { sketch::heap::initialize, sketch::heap::finalize, "heap" },
    { sketch::net::initialize, sketch::net::finalize, "net" }
};

} // namespace {}

namespace sketch
{

namespace SkBase
{


bool initialize() noexcept
{
    if (refcount::increment(&g_RefCount) > 1)
    {
        return true;
    }

    for (size_t i = 0; i < _countof(g_Components); i++)
    {
        if (g_Components[i].initializer)
        {
            if (!g_Components[i].initializer())
            {
                SketchError("SkBase failed to initialize component %s", g_Components[i].name);
                SK_ASSERT(0);

                // rollback
                while (i > 0)
                {
                    i--;
                    if (g_Components[i].finalizer)
                    {
                        g_Components[i].finalizer();
                    }
                }

                return false;
            }
        }
    }

    SketchDebug("SkBase initialized");
    return true;
}

void finalize() noexcept
{
    if (refcount::decrement(&g_RefCount) != 0)
    {
        return;
    }

    SketchDebug("SkBase finalizing...");

    for (size_t i = _countof(g_Components); i > 0; i--)
    {
        auto j = i - 1;
        if (g_Components[j].finalizer)
        {
            g_Components[j].finalizer();
        }
    }
}


} // namespace SkBase {}

} // namespace sketch {}
