#pragma once

#include "../../Sketch.hxx"


namespace sketch
{

namespace nt
{

class RegistryKey
    : public NonCopyable
    , public ErrorState
{
public:
    enum Disposition
    {
        KeyOpen,
        KeyCreate
    };

    enum Access
    {
        KeyRead = KEY_READ,
        KeyWrite = KEY_WRITE,
        KeyDelete = DELETE
    };

    enum Type
    {
        TypeNone = REG_NONE,
        TypeSz = REG_SZ,
        TypeExpandSz = REG_EXPAND_SZ,
        TypeBinary = REG_BINARY,
        TypeDWord = REG_DWORD,
        TypeDWordLe = REG_DWORD_LITTLE_ENDIAN,
        TypeDWordBe = REG_DWORD_BIG_ENDIAN,
        TypeLink = REG_LINK,
        TypeMultiSz = REG_MULTI_SZ,
        TypeResourceList = REG_RESOURCE_LIST,
        TypeResourceDescriptor = REG_FULL_RESOURCE_DESCRIPTOR,
        TypeResourceRequirementsList = REG_RESOURCE_REQUIREMENTS_LIST,
        TypeQWord = REG_QWORD,
        TypeQWordLe = REG_QWORD_LITTLE_ENDIAN,
    };

    struct Value
    {
        Type type;
        size_t size;
        UniquePtr<uint8_t[]> data;

        Value() noexcept
            : type(TypeNone)
            , size(0)
        {
        }
    };

    ~RegistryKey() noexcept
    {
        if (m_h)
        {
            ZwClose(m_h);
        }
    }

    RegistryKey(const wchar_t* keyPath, Disposition disposition, Access access, ErrorState::Mode errorMode = ErrorState::Mode::throwIfExceptionsSupported);

    static bool keyExists(const wchar_t* keyPath) noexcept;
    static bool deleteKey(const wchar_t* keyPath) noexcept;

    bool read(const wchar_t* valueName, Value& value) const;
    bool readString(const wchar_t* valueName, String& value) const;
    bool readDWord(const wchar_t* valueName, uint32_t& value) const;

    bool write(const wchar_t* valueName, Type type, const void* data, size_t dataSize);
    bool writeString(const wchar_t* valueName, const wchar_t* value);
    bool writeDWord(const wchar_t* valueName, uint32_t value);

    bool deleteKey();
    bool deleteValue(const wchar_t* value);

    bool valid() const noexcept
    {
        return !!m_h;
    }

    bool keyAt(size_t index, String& name) const;
    bool valueAt(size_t index, String& name) const;

private:
    HANDLE m_h;
};

} // namespace nt {}

} // namespace sketch {}
