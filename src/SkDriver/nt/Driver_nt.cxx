#include <SkDriver/nt/Driver_nt.hxx>
#include <SkBase/init.hxx>

#include "../crt.hxx"


static PDRIVER_UNLOAD s_unloadOuter = nullptr;

namespace sketch
{

namespace nt
{

Driver* Driver::s_instance = nullptr;

NTSTATUS Driver::DriverEntryStatic(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath)
{
    PAGED_CODE();

    if (!sketch::SkBase::initialize())
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!sketch::crt::initialize())
    {
        sketch::SkBase::finalize();
        return STATUS_UNSUCCESSFUL;
    }

    auto o = instance();
    SK_ASSERT(o);
    NTSTATUS result = o ? o->_driverEntry(driverObject, registryPath) : STATUS_UNSUCCESSFUL;
    if (!NT_SUCCESS(result))
    {
        sketch::crt::finalize();
        sketch::SkBase::finalize();
        return STATUS_UNSUCCESSFUL;
    }

    s_unloadOuter = driverObject->DriverUnload;
    driverObject->DriverUnload = DriverUnloadStatic;

    return STATUS_SUCCESS;
}

void Driver::DriverUnloadStatic(DRIVER_OBJECT *driverObject)
{
    PAGED_CODE();

    if (s_unloadOuter)
    {
        SketchDebug("Calling outer DriverUnload procedure %p", s_unloadOuter);
        s_unloadOuter(driverObject);
    }

    auto o = instance();
    if (o)
    {
        SketchDebug("Calling _driverUnload()");

        o->_driverUnload(driverObject);
    }

    sketch::crt::finalize();
    sketch::SkBase::finalize();
}

NTSTATUS Driver::_driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath)
{
    m_driverObject = driverObject;
    m_registryPath = String(registryPath);

    log::g_ModuleName = moduleName();

    return driverEntry(driverObject, registryPath);
}

void Driver::_driverUnload(DRIVER_OBJECT *driverObject)
{
    driverUnload(driverObject);
}


} // namespace nt {}

} // namespace sketch {}




extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath)
{
    return sketch::nt::Driver::DriverEntryStatic(driverObject, registryPath);
}

