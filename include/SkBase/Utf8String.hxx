#pragma once

#include "../Sketch.hxx"

#include <stdio.h>

namespace sketch
{

namespace internal
{

template <class _Allocator>
using Utf8StringData = SharedData<char, _Allocator>;

template <size_t _N, class _Allocator>
using StaticUtf8StringData = StaticSharedData<char, _N, _Allocator>;



template <class _Allocator>
struct Utf8StringDataRef
{
    Utf8StringData<_Allocator>* ptr;

    inline static const Utf8StringDataRef& empty() noexcept
    {
        static StaticUtf8StringData<1, _Allocator> _emptyStringData =
        {
            {
                refcount::Persistent,
                1,
                0,
                sizeof(Utf8StringData<_Allocator>)
            },
            {
                0
            }
        };

        static Utf8StringDataRef<_Allocator> _empty =
        {
            &_emptyStringData.header
        };

        return _empty;
    }
};

} // namespace internal {}


template <class _Allocator>
class Utf8StringT
{
public:
    typedef internal::Utf8StringData <_Allocator> StringDataT;
    typedef internal::Utf8StringDataRef<_Allocator> StringDataRefT;

    inline ~Utf8StringT() noexcept
    {
        if (!refcount::decrement(&m_ref.ptr->ref))
        {
            StringDataT::deallocate(m_ref.ptr);
        }
    }

    inline Utf8StringT(StringDataRefT ref, bool addRef) noexcept
        : m_ref(ref)
    {
        if (addRef)
        {
            refcount::increment(&m_ref.ptr->ref);
        }
    }

    inline Utf8StringT() noexcept
        : Utf8StringT(StringDataRefT::empty(), false)
    {
    }

    inline Utf8StringT(const Utf8StringT& other) noexcept
        : Utf8StringT(other.m_ref, true)
    {
    }

    inline Utf8StringT(Utf8StringT&& other) noexcept
        : Utf8StringT()
    {
        swap(other);
    }

    inline explicit Utf8StringT(const UChar* s, size_t length = size_t(-1))
        : Utf8StringT(reinterpret_cast<const wchar_t*>(s), length)
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    }

    inline explicit Utf8StringT(const wchar_t* u16, size_t length = size_t(-1))
        : Utf8StringT(_fromUtf16(u16, length), false)
    {
    }

    inline Utf8StringT(const char* u8, size_t length = size_t(-1))
        : Utf8StringT(_fromUtf8(u8, length), false)
    {
    }

    inline explicit Utf8StringT(const String& s)
        : Utf8StringT(_fromUtf16(s.utf16(), s.length()), false)
    {
    }

    inline Utf8StringT& operator=(const Utf8StringT& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            Utf8StringT t;
            swap(t);

            m_ref = other.m_ref;
            refcount::increment(&m_ref.ptr->ref);
        }

        return *this;
    }

    inline Utf8StringT& operator=(Utf8StringT&& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            Utf8StringT t(sketch::move(other));
            swap(t);
        }

        return *this;
    }

    inline void swap(Utf8StringT& other) noexcept
    {
        using sketch::swap;
        swap(m_ref, other.m_ref);
    }

    inline const char* str() const noexcept
    {
        return m_ref.ptr->data();
    }

    inline String utf16() const
    {
        return String(str(), length());
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

    inline bool isSame(const Utf8StringT& other) const noexcept
    {
        return (m_ref.ptr == other.m_ref.ptr);
    }

    inline int compare(const Utf8StringT& other) const noexcept
    {
        if (isSame(other))
        {
            return 0;
        }

        return compare(other.str());
    }

    inline int compare(const char* other, size_t length = size_t(-1)) const noexcept
    {
        if (length == size_t(-1))
        {
            return strcmp(str(), other);
        }
        else
        {
            return strncmp(str(), other, length);
        }
    }

    inline int compareNoCase(const char* other, size_t length = size_t(-1)) const noexcept
    {
        if (length == size_t(-1))
        {
            return _stricmp(str(), other);
        }
        else
        {
            return _strnicmp(str(), other, length);
        }
    }

    inline bool append(const char* u8, size_t length8 = size_t(-1))
    {
        if (!u8 || !*u8)
        {
            return true;
        }

        if (length8 == size_t(-1))
        {
            length8 = strlen(u8);
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
                    memcpy(dst, u8, length8 * sizeof(char));
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
        memcpy(dst, u8, length8 * sizeof(char));
        dst += length8;
        *dst = 0;

        sd->length = length() + length8 + 1;

        // swap StringData
        Utf8StringT t({ sd }, false);
        swap(t);

        return true;
    }

    inline bool append(const Utf8StringT& s)
    {
        return append(s.str(), s.length());
    }

    inline Utf8StringT& operator+=(const Utf8StringT& s)
    {
        append(s);
        return *this;
    }

    inline Utf8StringT& operator+=(const char* s)
    {
        append(s);
        return *this;
    }

    inline static Utf8StringT formatV(const char* format, va_list args)
    {
        if (!format || !*format)
        {
            return Utf8StringT();
        }

        auto formatLen = strlen(format);
        if (!formatLen)
        {
            return Utf8StringT();
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
                return Utf8StringT();
            }

            result = _vsnprintf_s(sd->data(), bufferSize, _TRUNCATE, format, args);

            bufferSize = sd->capacity;

        } while (result < 0);

        sd->length = result + 1;
        return Utf8StringT({ sd }, false);
    }

    inline static Utf8StringT format(const char* format, ...)
    {
        va_list args;
        va_start(args, format);

        auto s = formatV(format, args);

        va_end(args);

        return s;
    }

private:
    inline static size_t _Utf16ToUtf8Len(const wchar_t *str, size_t len) noexcept
    {
        if (!str)
        {
            return 0;
        }

        size_t result = 0;

        while (len)
        {
            auto uch = *str++;
            len--;

            if (!uch)
            {
                break;
            }

            if (uch < 0x0080)
            {
                result++;
            }
            else if (uch < 0x0800)
            {
                result += 2;
            }
            else
            {
                result += 3;
            }
        }

        return result;
    }

    inline static StringDataRefT _fromUtf16(const wchar_t* u16, size_t length = size_t(-1))
    {
        if (!u16)
        {
            return StringDataRefT::empty();
        }

        auto required = _Utf16ToUtf8Len(u16, length);
        if (!required)
        {
            return StringDataRefT::empty();
        }

        auto sd = StringDataT::allocate(required + 1, alignof(char));
        if (!sd)
        {
            return StringDataRefT::empty();
        }

        auto dstU8 = sd->data();
        size_t out = 0;

        while ((out < required) && length)
        {
            auto uch = *u16++;
            length--;

            if (!uch)
            {
                break;
            }

            if (uch < 0x0080)
            {
                *dstU8++ = (char)uch;
                out++;
            }
            else if (uch < 0x0800)
            {
                if (out >= required - 1)
                {
                    break;
                }

                *dstU8++ = (char)(0xc0 | (uch >> 6));
                *dstU8++ = (char)(0x80 | (uch & 0x3f));

                out += 2;
            }
            else
            {
                if (out >= required - 2)
                {
                    break;
                }

                *dstU8++ = (char)(0xe0 | (uch >> 12));
                *dstU8++ = (char)(0x80 | ((uch >> 6) & 0x3f));
                *dstU8++ = (char)(0x80 | (uch & 0x3f));

                out += 3;
            }
        }

        *dstU8 = '\0';
        sd->length = out + 1;

        return{ sd };
    }

    inline static StringDataRefT _fromUtf8(const char* u8, size_t length = size_t(-1))
    {
        if (!u8)
        {
            return StringDataRefT::empty();
        }

        if (length == size_t(-1))
        {
            length = strlen(u8);
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
        memcpy(p, u8, length * sizeof(char));
        p[length] = 0;

        sd->length = length + 1;

        return{ sd };
    }

private:
    StringDataRefT m_ref;
};


template <class _A>
inline void swap(Utf8StringT<_A>& a, Utf8StringT<_A>& b) noexcept
{
    a.swap(b);
}

#pragma region Comparison with Utf8StringT

template <class _A1, class _A2>
inline bool operator==(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) == 0);
}

template <class _A1, class _A2>
inline bool operator!=(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) != 0);
}

template <class _A1, class _A2>
inline bool operator<(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) < 0);
}

template <class _A1, class _A2>
inline bool operator<=(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) <= 0);
}

template <class _A1, class _A2>
inline bool operator>(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) > 0);
}

template <class _A1, class _A2>
inline bool operator>=(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b) noexcept
{
    return (a.compare(b) >= 0);
}

#pragma endregion

#pragma region Comparison with char*

template <class _A>
inline bool operator==(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) == 0);
}

template <class _A>
inline bool operator!=(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) != 0);
}

template <class _A>
inline bool operator<(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) < 0);
}

template <class _A>
inline bool operator<=(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) <= 0);
}

template <class _A>
inline bool operator>(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) > 0);
}

template <class _A>
inline bool operator>=(const Utf8StringT<_A>& a, const char* b) noexcept
{
    return (a.compare(b) >= 0);
}

#pragma endregion

#pragma region Concatenation

template <class _A1, class _A2>
inline Utf8StringT<_A1> operator+(const Utf8StringT<_A1>& a, const Utf8StringT<_A2>& b)
{
    Utf8StringT<_A1> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline Utf8StringT<_A> operator+(const Utf8StringT<_A>& a, const char* b)
{
    Utf8StringT<_A> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline Utf8StringT<_A> operator+(const char* a, const Utf8StringT<_A>& b)
{
    Utf8StringT<_A> s(a);
    s.append(b);
    return s;
}

#pragma endregion


typedef Utf8StringT<Allocator<char>> Utf8String;


#define SK_UTF8_LITERAL_II(str) u8"" str
#define SK_UTF8_LITERAL(str) SK_UTF8_LITERAL_II(str)

#define SkU8(str) \
    ([]() -> Utf8StringT<::sketch::Allocator<char>> { \
        enum { Size = sizeof(SK_UTF8_LITERAL(str)) / sizeof(char) }; \
        static const ::sketch::internal::StaticUtf8StringData<Size, ::sketch::Allocator<char>> string_literal = { \
            { ::sketch::refcount::Persistent, Size, 0, sizeof(::sketch::internal::Utf8StringData<::sketch::Allocator<char>>) }, \
            SK_UTF8_LITERAL(str) }; \
        ::sketch::internal::Utf8StringDataRef<::sketch::Allocator<char>> holder = { const_cast<::sketch::internal::Utf8StringData<::sketch::Allocator<char>>*>(&string_literal.header) }; \
        const ::sketch::Utf8StringT<::sketch::Allocator<char>> s(holder, false); \
        return s; \
    }()) \




} // namespace sketch {}
