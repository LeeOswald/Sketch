#pragma once

#include <Sketch.hxx>
#include <SkDriver/nt/Driver_nt.hxx>

namespace sketch
{

namespace nt
{


class Dll
    : public NonCopyable
    , public ErrorState
{
public:
    static NTSTATUS StaticDllInitialize(UNICODE_STRING* registryPath) noexcept;
    static NTSTATUS StaticDllUnload() noexcept;

    ~Dll() noexcept
    {
        SK_ASSERT(s_instance == this);
        s_instance = nullptr;
    }

    static Dll* instance() noexcept
    {
        return s_instance;
    }

    virtual const char* moduleName() const noexcept = 0;
    virtual NTSTATUS dllInitialize(UNICODE_STRING* registryPath) noexcept = 0;
    virtual NTSTATUS dllUnload() noexcept = 0;

protected:
    Dll() noexcept
        : ErrorState(ErrorState::Mode::neverThrow)
        , m_fakeDriver(this)
    {
        SK_ASSERT(!s_instance);
        s_instance = this;
    }

private:
    NTSTATUS _dllInitialize(UNICODE_STRING* registryPath) noexcept
    {
        return dllInitialize(registryPath);
    }

    NTSTATUS _dllUnload() noexcept
    {
        return dllUnload();
    }

private:
    class FakeDriver
        : public Driver
    {
    public:
        FakeDriver(Dll* owner) noexcept
            : m_owner(owner)
        {
        }

        const char* moduleName() const noexcept override
        {
            return m_owner->moduleName();
        }

        NTSTATUS driverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath) noexcept
        {
            SK_UNUSED(driverObject);
            SK_UNUSED(registryPath);
            return STATUS_SUCCESS;
        }

        void driverUnload(DRIVER_OBJECT* driverObject) noexcept override
        {
            SK_UNUSED(driverObject);
        }

    private:
        Dll* m_owner;
    };

private:
    static Dll* s_instance;

    FakeDriver m_fakeDriver;
};

} // namespace nt {}

} // namespace sketch {}

