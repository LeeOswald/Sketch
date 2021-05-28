#include <SkDll/nt/Dll_nt.hxx>


namespace sketch
{

namespace nt
{

Dll* Dll::s_instance = nullptr;

NTSTATUS Dll::StaticDllInitialize(UNICODE_STRING* registryPath) noexcept
{
    // call Driver::DriverEntryStatic to initialize SkBase and CRT
    DRIVER_OBJECT fakeDriverObject = { 0 };
    auto status = Driver::DriverEntryStatic(&fakeDriverObject, registryPath);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    auto _instance = instance();
    SK_ASSERT(_instance);
    if (!_instance)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return _instance->_dllInitialize(registryPath);
}

NTSTATUS Dll::StaticDllUnload() noexcept
{
    auto _instance = instance();
    SK_ASSERT(_instance);
    if (!_instance)
    {
        return STATUS_UNSUCCESSFUL;
    }

    auto status = _instance->_dllUnload();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    DRIVER_OBJECT fakeDriverObject = { 0 };
    Driver::DriverUnloadStatic(&fakeDriverObject);

    return STATUS_SUCCESS;
}


} // namespace nt {}

} // namespace sketch {}



extern "C" NTSTATUS DllInitialize(UNICODE_STRING* registryPath)
{
    return sketch::nt::Dll::StaticDllInitialize(registryPath);
}

extern "C" NTSTATUS DllUnload()
{
    return sketch::nt::Dll::StaticDllUnload();
}
