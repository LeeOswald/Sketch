#pragma once

#include "../../Sketch.hxx"


namespace sketch
{


namespace internal
{

template <class _Allocator>
using AnsiStringData = SharedData<char, _Allocator>;

template <size_t _N, class _Allocator>
using StaticAnsiStringData = StaticSharedData<char, _N, _Allocator>;


template <class _Allocator>
struct AnsiStringDataRef
{
    AnsiStringData<_Allocator>* ptr;

    inline static const AnsiStringDataRef& empty() noexcept
    {
        static StaticAnsiStringData<1, _Allocator> _emptyStringData =
        {
            {
                refcount::Persistent,
                1,
                0,
                sizeof(AnsiStringData<_Allocator>)
            },
            {
                0
            }
        };

        static AnsiStringDataRef<_Allocator> _empty =
        {
            &_emptyStringData.header
        };

        return _empty;
    }
};


} // namespace internal {}


template <class _Allocator>
class AnsiStringT
{
public:
    typedef typename _Allocator::template rebind<UChar>::other allocator_type;

    typedef internal::AnsiStringData <_Allocator> StringDataT;
    typedef internal::AnsiStringDataRef<_Allocator> StringDataRefT;

    inline ~AnsiStringT() noexcept
    {
        if (!refcount::decrement(&m_ref.ptr->ref))
        {
            StringDataT::deallocate(m_ref.ptr);
        }
    }

    inline AnsiStringT(StringDataRefT ref, bool addRef) noexcept
        : m_ref(ref)
    {
        if (addRef)
        {
            refcount::increment(&m_ref.ptr->ref);
        }
    }

    inline AnsiStringT() noexcept
        : AnsiStringT(StringDataRefT::empty(), false)
    {
    }

    inline AnsiStringT(const char* s, size_t length = size_t(-1))
        : AnsiStringT(_fromAnsi(s, length), false)
    {
    }

    inline AnsiStringT(const wchar_t* s, size_t length = size_t(-1))
        : AnsiStringT(_fromUtf16(s, length), false)
    {
    }

    inline AnsiStringT(const UChar* s, size_t length = size_t(-1))
        : AnsiStringT(_fromUtf16(reinterpret_cast<const wchar_t*>(s), length), false)
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    }

    inline AnsiStringT(const AnsiStringT& other) noexcept
        : AnsiStringT(other.m_ref, true)
    {
    }

    inline AnsiStringT(AnsiStringT&& other) noexcept
        : AnsiStringT()
    {
        swap(other);
    }

    inline AnsiStringT(const ANSI_STRING* as)
        : AnsiStringT(_fromAnsi(as ? as->Buffer : nullptr, as ? as->Length : 0), false)
    {
    }

    inline AnsiStringT(const String& s)
        : AnsiStringT(_fromUtf16(s.utf16(), s.length()), false)
    {
    }

    inline void swap(AnsiStringT& other) noexcept
    {
        using sketch::swap;
        swap(m_ref, other.m_ref);
    }

    inline AnsiStringT& operator=(const AnsiStringT& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            AnsiStringT t;
            swap(t);

            m_ref = other.m_ref;
            refcount::increment(&m_ref.ptr->ref);
        }

        return *this;
    }

    inline AnsiStringT& operator=(AnsiStringT&& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            AnsiStringT t(sketch::move(other));
            swap(t);
        }

        return *this;
    }

    inline const char* str() const noexcept
    {
        return m_ref.ptr->data();
    }

    inline size_t length() const noexcept
    {
        SK_ASSERT(m_ref.ptr->length > 0); // even an empty string must have '\0'
        return m_ref.ptr->length - 1;
    }

    inline bool empty() const noexcept
    {
        return !length();
    }

    inline bool append(const char* a, size_t length8 = size_t(-1))
    {
        if (!a || !*a)
        {
            return true;
        }

        if (length8 == size_t(-1))
        {
            length8 = strlen(a);
        }

        if (!length8)
        {
            return true;
        }

        // check if we can reuse our StringData
        if (m_ref.ptr->writeable())
        {
            // enough space
            if (m_ref.ptr->capacity >= length() + length8 + 1)
            {
                // not shared
                auto lock = refcount::lock_for_scope(&m_ref.ptr->ref);
                if (lock.owned())
                {
                    auto dst = offset_pointer<char>(m_ref.ptr->data(), length() * sizeof(char));
                    memcpy(dst, a, length8 * sizeof(char));
                    m_ref.ptr->length += length8;
                    dst[length8] = 0;

                    return true;
                }
            }
        }

        // need a new StringData
        auto sd = StringDataT::allocate(
            StringDataT::grow(
                length() + 1,
                length() + length8 + 1,
                alignof(char)
            ),
            alignof(char)
        );

        if (!sd)
        {
            return false;
        }

        auto dst = sd->data();
        memcpy(dst, str(), length() * sizeof(char));
        dst += length();
        memcpy(dst, a, length8 * sizeof(char));
        dst += length8;
        *dst = 0;

        sd->length = length() + length8 + 1;

        // swap StringData
        AnsiStringT t({ sd }, false);
        swap(t);

        return true;
    }

    inline static AnsiStringT formatV(const char* format, va_list args)
    {
        if (!format || !*format)
        {
            return AnsiStringT();
        }

        auto formatLen = strlen(format);
        if (!formatLen)
        {
            return AnsiStringT();
        }

        auto bufferSize = formatLen;
        StringDataT* sd = nullptr;

        int result;
        do
        {
            if (sd)
            {
                StringDataT::deallocate(sd);
            }

            sd = StringDataT::allocate(
                StringDataT::grow(
                    bufferSize + 1,
                    alignof(char)
                ),
                alignof(char)
            );

            if (!sd)
            {
                return AnsiStringT();
            }

            result = _vsnprintf_s(sd->data(), bufferSize, _TRUNCATE, format, args);

            bufferSize = sd->capacity;

        } while (result < 0);

        sd->length = result + 1;
        return AnsiStringT({ sd }, false);
    }

    inline static AnsiStringT format(const char* format, ...)
    {
        va_list args;
        va_start(args, format);

        auto s = formatV(format, args);

        va_end(args);

        return s;
    }

private:
    inline static StringDataRefT _fromUtf16(const wchar_t* u16, size_t length = size_t(-1))
    {
        if (!u16)
        {
            return StringDataRefT::empty();
        }

        if (length == size_t(-1))
        {
            length = wcslen(u16);
        }

        if (!length)
        {
            return StringDataRefT::empty();
        }

        length *= sizeof(wchar_t);

        UNICODE_STRING us = { USHORT(length), USHORT(length), PWCH(u16) };
        ANSI_STRING as = { 0 };
        auto status = RtlUnicodeStringToAnsiString(&as, &us, TRUE);
        if (status != STATUS_SUCCESS)
        {
            StringDataRefT::empty();
        }

        auto ref = _fromAnsi(as.Buffer, as.Length);
        RtlFreeAnsiString(&as);

        return ref;
    }

    inline static StringDataRefT _fromAnsi(const char* s, size_t length = size_t(-1))
    {
        if (!s)
        {
            return StringDataRefT::empty();
        }

        if (length == size_t(-1))
        {
            length = strlen(s);
        }

        if (!length)
        {
            return StringDataRefT::empty();
        }

        auto sd = StringDataT::allocate(length + 1, alignof(char));
        if (!sd)
        {
            return StringDataRefT::empty();
        }

        auto p = sd->data();
        memcpy(p, s, length * sizeof(char));
        p[length] = 0;

        sd->length = length + 1;

        return{ sd };
    }

private:
    StringDataRefT m_ref;
};


typedef AnsiStringT<Allocator<char>> AnsiString;


} // namespace sketch {}
