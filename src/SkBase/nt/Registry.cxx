#include <SkBase/nt/Registry.hxx>


namespace sketch
{

namespace nt
{

RegistryKey::RegistryKey(const wchar_t* keyPath, Disposition disposition, Access access, ErrorState::Mode errorMode)
    : ErrorState(errorMode)
    , m_h(0)
{
    UNICODE_STRING us;
    RtlInitUnicodeString(&us, keyPath);

    OBJECT_ATTRIBUTES oa;
#if defined(SKETCH_WIN_KM)
    InitializeObjectAttributes(&oa, &us, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
#else
    InitializeObjectAttributes(&oa, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
#endif

    auto am = static_cast<ACCESS_MASK>(access);

    NTSTATUS status;
    if (disposition == KeyCreate)
    {
        status = ZwCreateKey(&m_h, am, &oa, 0, nullptr, 0, nullptr);
    }
    else if (disposition == KeyOpen)
    {
        status = ZwOpenKey(&m_h, am, &oa);
    }
    else
    {
        SK_ASSERT(0);
        status = STATUS_INVALID_PARAMETER;
    }

    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
    }
}

bool RegistryKey::keyExists(const wchar_t* keyPath) noexcept
{
    return (RtlCheckRegistryKey(RTL_REGISTRY_ABSOLUTE, const_cast<PWSTR>(keyPath)) == STATUS_SUCCESS);
}

bool RegistryKey::deleteKey(const wchar_t* keyPath) noexcept
{
    RegistryKey k(keyPath, KeyOpen, KeyDelete, ErrorState::Mode::neverThrow);
    if (!k.valid())
    {
        return false;
    }

    return k.deleteKey();
}

bool RegistryKey::read(const wchar_t* valueName, Value& value) const
{
    SK_ASSERT(valid());

    UNICODE_STRING us;
    RtlInitUnicodeString(&us, valueName);

    ULONG required = 0;
    auto status = ZwQueryValueKey(
        m_h,
        &us,
        KeyValuePartialInformation,
        nullptr,
        0,
        &required
    );

    if (
        (status != STATUS_BUFFER_TOO_SMALL) &&
        (status != STATUS_BUFFER_OVERFLOW)
        )
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    UniquePtr<uint8_t[]> b(new uint8_t[required]);
    if (!b)
    {
        setLastError(SkNtError(STATUS_NO_MEMORY, String()));
        return false;
    }

    status = ZwQueryValueKey(
        m_h,
        &us,
        KeyValuePartialInformation,
        b.get(),
        required,
        &required
    );

    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    auto info = reinterpret_cast<PKEY_VALUE_PARTIAL_INFORMATION>(b.get());
    UniquePtr<uint8_t[]> v(new uint8_t[info->DataLength]);
    if (!v)
    {
        setLastError(SkNtError(STATUS_NO_MEMORY, String()));
        return false;
    }

    memcpy(v.get(), info->Data, info->DataLength);

    value.type = static_cast<Type>(info->Type);
    value.size = info->DataLength;
    value.data = sketch::move(v);

    return true;
}

bool RegistryKey::readString(const wchar_t* valueName, String& value) const
{
    Value v;
    if (!read(valueName, v))
    {
        return false;
    }

    if (
        (v.type != TypeSz) &&
        (v.type != TypeExpandSz)
        )
    {
        SK_ASSERT(0);
        return false;
    }

    value = String(
        reinterpret_cast<const wchar_t*>(v.data.get()),
        v.size / sizeof(wchar_t)
    );

    return true;
}

bool RegistryKey::readDWord(const wchar_t* valueName, uint32_t& value) const
{
    Value v;
    if (!read(valueName, v))
    {
        return false;
    }

    if (v.type != TypeDWord)
    {
        SK_ASSERT(0);
        return false;
    }

    value = *reinterpret_cast<const uint32_t*>(v.data.get());

    return true;
}

bool RegistryKey::write(const wchar_t* valueName, RegistryKey::Type type, const void* data, size_t dataSize)
{
    SK_ASSERT(valid());

    UNICODE_STRING us;
    RtlInitUnicodeString(&us, valueName);

    auto status = ZwSetValueKey(m_h, &us, 0, type, const_cast<PVOID>(data), static_cast<ULONG>(dataSize));
    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    return true;
}

bool RegistryKey::writeString(const wchar_t* valueName, const wchar_t* value)
{
    auto size = (wcslen(value) + 1) * sizeof(wchar_t);
    return write(valueName, TypeSz, value, size);
}

bool RegistryKey::writeDWord(const wchar_t* valueName, uint32_t value)
{
    return write(valueName, TypeDWord, &value, sizeof(value));
}

bool RegistryKey::deleteKey()
{
    SK_ASSERT(valid());

    auto status = ZwDeleteKey(m_h);
    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    ZwClose(m_h); // the key becomes invalid
    m_h = 0;

    return true;
}

bool RegistryKey::deleteValue(const wchar_t* value)
{
    SK_ASSERT(valid());

    UNICODE_STRING us;
    RtlInitUnicodeString(&us, value);

    auto status = ZwDeleteValueKey(m_h, &us);
    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    return true;
}

bool RegistryKey::keyAt(size_t index, String& name) const
{
    ULONG required = 0;
    NTSTATUS status = ZwEnumerateKey(
        m_h,
        static_cast<ULONG>(index),
        KeyNodeInformation,
        nullptr,
        0,
        (PULONG)&required
    );

    if (status == STATUS_NO_MORE_ENTRIES)
    {
        return false;
    }

    if ((status != STATUS_BUFFER_OVERFLOW) && (status != STATUS_BUFFER_TOO_SMALL))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    UniquePtr<uint8_t[]> buffer(new (sketch::nothrow) uint8_t[required]);
    if (!buffer)
    {
        setLastError(SkNtError(STATUS_NO_MEMORY, String()));
        return false;
    }

    status = ZwEnumerateKey(
        m_h,
        static_cast<ULONG>(index),
        KeyNodeInformation,
        buffer.get(),
        required,
        (PULONG)&required
    );

    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    auto ni = reinterpret_cast<KEY_NODE_INFORMATION*>(buffer.get());

    name = String(ni->Name, ni->NameLength / sizeof(WCHAR));
    return true;
}

bool RegistryKey::valueAt(size_t index, String& name) const
{
    ULONG required = 0;
    NTSTATUS status = ZwEnumerateValueKey(
        m_h,
        static_cast<ULONG>(index),
        KeyValueFullInformation,
        nullptr,
        0,
        &required
    );

    if (status == STATUS_NO_MORE_ENTRIES)
    {
        return false;
    }

    if ((status != STATUS_BUFFER_OVERFLOW) && (status != STATUS_BUFFER_TOO_SMALL))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    UniquePtr<uint8_t[]> buffer(new (sketch::nothrow) uint8_t[required]);
    if (!buffer)
    {
        setLastError(SkNtError(STATUS_NO_MEMORY, String()));
        return false;
    }

    status = ZwEnumerateValueKey(
        m_h,
        static_cast<ULONG>(index),
        KeyValueFullInformation,
        buffer.get(),
        required,
        (PULONG)&required
    );

    if (!NT_SUCCESS(status))
    {
        setLastError(SkNtError(status, String()));
        return false;
    }

    auto kvi = reinterpret_cast<KEY_VALUE_FULL_INFORMATION*>(buffer.get());

    name = String(kvi->Name, kvi->NameLength / sizeof(WCHAR));
    return true;
}


} // namespace nt {}

} // namespace sketch {}
