#pragma once

#include <Sketch.hxx>

#ifndef SKETCH_WIN_KM
#error This module is kernel-mode only
#endif



namespace sketch
{

namespace nt
{


class Driver
    : public NonCopyable
    , public ErrorState
{
public:
    static NTSTATUS DriverEntryStatic(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath);
    static void DriverUnloadStatic(DRIVER_OBJECT *driverObject);

    inline static Driver* instance() noexcept
    {
        SK_ASSERT(s_instance);
        return s_instance;
    }

    virtual ~Driver() noexcept
    {
        // all complex de-initialization should be done in driverUnload()
        SK_ASSERT(s_instance == this);
        s_instance = nullptr;
    }

    virtual const char* moduleName() const noexcept = 0;
    virtual NTSTATUS driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath) noexcept = 0;
    virtual void driverUnload(DRIVER_OBJECT* driverObject) noexcept = 0;

    inline DRIVER_OBJECT* driverObject() const noexcept
    {
        return m_driverObject;
    }

    inline const String& registryPath() const noexcept
    {
        return m_registryPath;
    }

protected:
    Driver() noexcept
        : ErrorState(ErrorState::Mode::neverThrow)
        , m_driverObject(nullptr)
    {
        SK_ASSERT(s_instance == nullptr);
        s_instance = this;

        // all complex initialization should be done in driverEntry()
    }

private:
    NTSTATUS _driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath);
    void _driverUnload(DRIVER_OBJECT *driverObject);

private:
    static Driver* s_instance;

    DRIVER_OBJECT* m_driverObject;
    String m_registryPath;
};



} // namespace nt {}

} // namespace sketch {}



