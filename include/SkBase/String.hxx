#pragma once

#include "functional.hxx"


namespace sketch
{

namespace internal
{

template <class _Allocator>
using StringData = SharedData<UChar, _Allocator>;

template <size_t _N, class _Allocator>
using StaticStringData = StaticSharedData<UChar, _N, _Allocator>;


template <class _Allocator>
struct StringDataRef
{
    StringData<_Allocator>* ptr;

    inline static const StringDataRef& empty() noexcept
    {
        static StaticStringData<1, _Allocator> _emptyStringData =
        {
            {
                refcount::Persistent,
                1,
                0,
                sizeof(StringData<_Allocator>)
            },
            {
                0
            }
        };

        static StringDataRef<_Allocator> _empty =
        {
            &_emptyStringData.header
        };

        return _empty;
    }
};


} // namespace internal {}


template <class _Allocator>
class StringT
{
public:
    typedef typename _Allocator::template rebind<UChar>::other allocator_type;

    typedef internal::StringData <_Allocator> StringDataT;
    typedef internal::StringDataRef<_Allocator> StringDataRefT;

    inline ~StringT() noexcept
    {
        if (!refcount::decrement(&m_ref.ptr->ref))
        {
            StringDataT::deallocate(m_ref.ptr);
        }
    }

    inline StringT(StringDataRefT ref, bool addRef) noexcept
        : m_ref(ref)
    {
        if (addRef)
        {
            refcount::increment(&m_ref.ptr->ref);
        }
    }

    inline StringT() noexcept
        : StringT(StringDataRefT::empty(), false)
    {
    }

    inline StringT(const StringT& other) noexcept
        : StringT(other.m_ref, true)
    {
    }

    inline StringT(StringT&& other) noexcept
        : StringT()
    {
        swap(other);
    }

    inline StringT(const UChar* s, size_t length = size_t(-1))
        : StringT(reinterpret_cast<const wchar_t*>(s), length)
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    }

    inline StringT(const wchar_t* u16, size_t length = size_t(-1))
        : StringT(_fromUtf16(u16, length), false)
    {
    }

    inline StringT(const char* u8, size_t length = size_t(-1))
        : StringT(_fromUtf8(u8, length), false)
    {
    }

#ifdef SKETCH_WIN
    inline StringT(const UNICODE_STRING* us)
        : StringT(_fromUtf16(us ? us->Buffer : nullptr, us ? us->Length / sizeof(WCHAR) : 0), false)
    {
    }
#endif

    inline StringT& operator=(const StringT& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            StringT t;
            swap(t);

            m_ref = other.m_ref;
            refcount::increment(&m_ref.ptr->ref);
        }

        return *this;
    }

    inline StringT& operator=(StringT&& other) noexcept
    {
        if (m_ref.ptr != other.m_ref.ptr)
        {
            StringT t(sketch::move(other));
            swap(t);
        }

        return *this;
    }

    inline void swap(StringT& other) noexcept
    {
        using sketch::swap;
        swap(m_ref, other.m_ref);
    }

    inline const UChar* str() const noexcept
    {
        return m_ref.ptr->data();
    }

    inline const wchar_t* utf16() const noexcept
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        return reinterpret_cast<const wchar_t*>(m_ref.ptr->data());
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

    inline bool isSame(const StringT& other) const noexcept
    {
        return (m_ref.ptr == other.m_ref.ptr);
    }

    inline int compare(const StringT& other) const noexcept
    {
        if (isSame(other))
        {
            return 0;
        }

        auto l = length();
        auto l2 = other.length();
        auto r = l - l2;

        if (r)
        {
            return int(r);
        }

        return compare(other.str());
    }

    int compare(const UChar* other, size_t length = size_t(-1)) const noexcept
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        const wchar_t* wother = reinterpret_cast<const wchar_t*>(other);

        if (length == size_t(-1))
        {
            return wcscmp(utf16(), wother);
        }
        else
        {
            return wcsncmp(utf16(), wother, length);
        }
    }

    inline int compareNoCase(const UChar* other, size_t length = size_t(-1)) const noexcept
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        const wchar_t* wother = reinterpret_cast<const wchar_t*>(other);

        if (length == size_t(-1))
        {
            return _wcsicmp(utf16(), wother);
        }
        else
        {
            return _wcsnicmp(utf16(), wother, length);
        }
    }

    inline bool append(const StringT& s)
    {
        return append(s.str(), s.length());
    }

    inline bool append(const UChar* s, size_t length = size_t(-1))
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        return append(reinterpret_cast<const wchar_t*>(s), length);
    }

    inline bool append(const wchar_t* u16, size_t length16 = size_t(-1))
    {
        if (!u16 || !*u16)
        {
            return true;
        }

        if (length16 == size_t(-1))
        {
            length16 = wcslen(u16);
        }

        if (!length16)
        {
            return true;
        }

        // check if we can reuse our StringData
        if (m_ref.ptr->writeable())
        {
            // enough space
            if (m_ref.ptr->capacity >= length() + length16 + 1)
            {
                // not shared
                auto lock = refcount::lock_for_scope(&m_ref.ptr->ref);
                if (lock.owned())
                {
                    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
                    auto dst = offset_pointer<wchar_t>(m_ref.ptr->data(), length() * sizeof(UChar));
                    memcpy(dst, u16, length16 * sizeof(wchar_t));
                    m_ref.ptr->length += length16;
                    dst[length16] = 0;

                    return true;
                }
            }
        }

        // need a new StringData
        auto sd = StringDataT::allocate(
            StringDataT::grow(
                length() + 1,
                length() + length16 + 1,
                alignof(UChar)
            ),
            alignof(UChar)
        );

        if (!sd)
        {
            return false;
        }

        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        auto dst = reinterpret_cast<wchar_t*>(sd->data());
        memcpy(dst, utf16(), length() * sizeof(wchar_t));
        dst += length();
        memcpy(dst, u16, length16 * sizeof(wchar_t));
        dst += length16;
        *dst = 0;

        sd->length = length() + length16 + 1;

        // swap StringData
        StringT t({ sd }, false);
        swap(t);

        return true;
    }

    inline StringT& operator+=(const StringT& s)
    {
        append(s);
        return *this;
    }

    inline StringT& operator+=(const UChar* s)
    {
        append(s);
        return *this;
    }

    inline StringT& operator+=(const wchar_t* s)
    {
        append(s);
        return *this;
    }

    inline static StringT format(const UChar* format, ...)
    {
        va_list args;
        va_start(args, format);

        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        auto s = formatV(reinterpret_cast<const wchar_t*>(format), args);

        va_end(args);

        return s;
    }

    inline static StringT format(const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);

        auto s = formatV(format, args);

        va_end(args);

        return s;
    }

    inline static StringT formatV(const UChar* format, va_list args)
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
        return formatV(reinterpret_cast<const wchar_t*>(format), args);
    }

    inline static StringT formatV(const wchar_t* format, va_list args)
    {
        if (!format || !*format)
        {
            return StringT();
        }

        auto formatLen = wcslen(format);
        if (!formatLen)
        {
            return StringT();
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
                alignof(UChar)
                ),
                alignof(UChar)
            );
            if (!sd)
            {
                return StringT();
            }

            result = _vsnwprintf_s(reinterpret_cast<wchar_t*>(sd->data()), bufferSize, _TRUNCATE, format, args);

            bufferSize = sd->capacity;

        } while (result < 0);

        sd->length = result + 1;

        return StringT({ sd }, false);
    }

    inline StringT trimLeft() const
    {
        if (empty())
        {
            return StringT();
        }

        auto p = m_ref.ptr->data();
        while (*p && _iswspace(*p))
        {
            p++;
        }

        if (!*p)
        {
            return StringT();
        }

        return StringT(p);
    }

    inline StringT trimRight() const
    {
        if (empty())
        {
            return StringT();
        }

        auto p = m_ref.ptr->data();
        auto l = length();
        SK_ASSERT(p[l] == 0);

        while (l && _iswspace(p[l - 1]))
        {
            l--;
        }

        if (!l)
        {
            return StringT();
        }

        return StringT(p, l);
    }

    inline StringT trim() const
    {
        return trimLeft().trimRight();
    }

    inline size_t hash() const noexcept
    {
        auto p = reinterpret_cast<const uint8_t*>(m_ref.ptr->data());
        auto l = length() * sizeof(UChar);

#ifdef SKETCH_32
        static const size_t prime = 0x01000193;
        static const size_t seed = 2166136261UL;
#else
        static const size_t prime = 0x100000001B3;
        static const size_t seed = 14695981039346656037ULL;
#endif
        size_t h = seed;

        while (l)
        {
            h = h * prime ^ *p;
            p++;
            l--;
        }

        return h;
    }

private:
    inline static StringDataRefT _fromUtf16(const wchar_t* u16, size_t length = size_t(-1))
    {
        static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");

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

        auto sd = StringDataT::allocate(length + 1, alignof(UChar));
        if (!sd)
        {
            return StringDataRefT::empty();
        }

        auto p = sd->data();
        memcpy(p, u16, length * sizeof(wchar_t));
        p[length] = 0;

        sd->length = length + 1;

        return{ sd };
    }

    inline static size_t _Utf8ToUtf16Len(const char* srcU8, size_t length) noexcept
    {
        if (!srcU8)
        {
            return 0;
        }

        size_t result = 0;
        auto us = reinterpret_cast<const uint8_t*>(srcU8);
        size_t i = 0;

        while (length)
        {
            uint8_t ch0 = us[i++];
            length--;

            if (!ch0)
            {
                break;
            }

            if (ch0 < 0x80)
            {
                result++;
            }
            else if (ch0 < 0x80 + 0x40)
            {
                // Invalid UTF8: only continuation characters start with 10
                return 0;
            }
            else
            {
                if (!length)
                {
                    break;
                }

                auto ch1 = us[i++];
                length--;

                if (!ch1)
                {
                    break;
                }

                if (ch0 < 0x80 + 0x40 + 0x20)
                {
                    result++;
                }
                else
                {
                    auto ch2 = us[i++];
                    length--;

                    if (!ch2)
                    {
                        break;
                    }

                    result++;
                }
            }
        }

        return result; // w/out '\0'
    }

    inline static StringDataRefT _fromUtf8(const char* u8, size_t length = size_t(-1))
    {
        if (!u8)
        {
            return StringDataRefT::empty();
        }

        auto required = _Utf8ToUtf16Len(u8, length);
        if (!required)
        {
            return StringDataRefT::empty();
        }

        auto sd = StringDataT::allocate(required + 1, alignof(UChar));
        if (!sd)
        {
            return StringDataRefT::empty();
        }

        auto dstU16 = sd->data();

        auto us = reinterpret_cast<const uint8_t*>(u8);
        size_t ui = 0;

        while ((ui < required) && length)
        {
            uint8_t ch0 = *(us++);

            if (!ch0)
            {
                break;
            }

            length--;

            if (ch0 < 0x80)
            {
                dstU16[ui++] = ch0;
            }
            else if (ch0 < 0x80 + 0x40)
            {
                // Invalid UTF8: only continuation characters start with 10
                StringDataT::deallocate(sd);
                return StringDataRefT::empty();
            }
            else
            {
                if (!length)
                {
                    break;
                }

                uint8_t ch1 = *(us++);
                length--;

                if (!ch1)
                {
                    break;
                }

                if (ch0 < 0x80 + 0x40 + 0x20)
                {
                    dstU16[ui++] = (wchar_t)(((ch0 & 0x1F) << 6) + (ch1 & 0x7F));
                }
                else
                {
                    if (!length)
                    {
                        break;
                    }

                    uint8_t ch2 = *(us++);
                    length--;

                    if (!ch2)
                    {
                        break;
                    }

                    dstU16[ui++] = (wchar_t)(((ch0 & 0xF) << 12) + ((ch1 & 0x7F) << 6) + (ch2 & 0x7F));
                }
            }
        }

        dstU16[ui] = 0;
        sd->length = ui + 1;

        return{ sd };
    }

#ifdef SKETCH_WIN_KM
    inline static bool _iswspace(wchar_t c) noexcept
    {
        static const wchar_t sWhitespaces[] =
        {
            0x0009, /* CHARACTER TABULATION */
            0x000A, /* LINE FEED (LF) */
            0x000B, /* LINE TABULATION */
            0x000C, /* FORM FEED (FF) */
            0x000D, /* CARRIAGE RETURN (CR) */
            0x0020, /* SPACE */
            0x0085, /* NEXT LINE (NEL) */
            0x00A0, /* NO-BREAK SPACE */
            0x1680, /* OGHAM SPACE MARK */
            0x2000, /* EN QUAD */
            0x2001, /* EM QUAD */
            0x2002, /* EN SPACE */
            0x2003, /* EM SPACE */
            0x2004, /* THREE-PER-EM SPACE */
            0x2005, /* FOUR-PER-EM SPACE */
            0x2006, /* SIX-PER-EM SPACE */
            0x2007, /* FIGURE SPACE */
            0x2008, /* PUNCTUATION SPACE */
            0x2009, /* THIN SPACE */
            0x200A, /* HAIR SPACE */
            0x2028, /* LINE SEPARATOR */
            0x2029, /* PARAGRAPH SEPARATOR */
            0x202F, /* NARROW NO-BREAK SPACE */
            0x205F, /* MEDIUM MATHEMATICAL SPACE */
            0x3000, /* IDEOGRAPHIC SPACE */
            0
        };

        auto p = sWhitespaces;
        while (*p)
        {
            if (*p == c)
            {
                return true;
            }

            p++;
        }

        return false;
    }
#else
    static __forceinline bool _iswspace(wchar_t c) noexcept
    {
        return !!iswspace(c);
    }
#endif

private:
    StringDataRefT m_ref;
};


template <class _A>
inline void swap(StringT<_A>& a, StringT<_A>& b) noexcept
{
    a.swap(b);
}

#pragma region Comparison with StringT

template <class _A1, class _A2>
inline bool operator==(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) == 0);
}

template <class _A1, class _A2>
inline bool operator!=(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) != 0);
}

template <class _A1, class _A2>
inline bool operator<(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) < 0);
}

template <class _A1, class _A2>
inline bool operator<=(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) <= 0);
}

template <class _A1, class _A2>
inline bool operator>(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) > 0);
}

template <class _A1, class _A2>
inline bool operator>=(const StringT<_A1>& a, const StringT<_A2>& b) noexcept
{
    return (a.compare(b) >= 0);
}

#pragma endregion

#pragma region Comparison with UChar*

template <class _A>
inline bool operator==(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) == 0);
}

template <class _A>
inline bool operator!=(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) != 0);
}

template <class _A>
inline bool operator<(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) < 0);
}

template <class _A>
inline bool operator<=(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) <= 0);
}

template <class _A>
inline bool operator>(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) > 0);
}

template <class _A>
inline bool operator>=(const StringT<_A>& a, const UChar* b) noexcept
{
    return (a.compare(b) >= 0);
}

#pragma endregion

#pragma region Comparison with wchar_t*

template <class _A>
inline bool operator==(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) == 0);
}

template <class _A>
inline bool operator!=(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) != 0);
}

template <class _A>
inline bool operator<(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) < 0);
}

template <class _A>
inline bool operator<=(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) <= 0);
}

template <class _A>
inline bool operator>(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) > 0);
}

template <class _A>
inline bool operator>=(const StringT<_A>& a, const wchar_t* b) noexcept
{
    static_assert(sizeof(wchar_t) == sizeof(UChar), "check implementation");
    return (a.compare(reinterpret_cast<const UChar*>(b)) >= 0);
}

#pragma endregion

#pragma region Concatenation

template <class _A1, class _A2>
inline StringT<_A1> operator+(const StringT<_A1>& a, const StringT<_A2>& b)
{
    StringT<_A1> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline StringT<_A> operator+(const StringT<_A>& a, const UChar* b)
{
    StringT<_A> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline StringT<_A> operator+(const UChar* a, const StringT<_A>& b)
{
    StringT<_A> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline StringT<_A> operator+(const StringT<_A>& a, const wchar_t* b)
{
    StringT<_A> s(a);
    s.append(b);
    return s;
}

template <class _A>
inline StringT<_A> operator+(const wchar_t* a, const StringT<_A>& b)
{
    StringT<_A> s(a);
    s.append(b);
    return s;
}


#pragma endregion


typedef StringT<Allocator<UChar>> String;


template<>
struct hash<String>
    : unary_function<String, size_t>
{
    inline size_t operator()(const String& val) const noexcept
    {
        return val.hash();
    }
};



#define SK_UTF16_LITERAL_II(str) u"" str
#define SK_UTF16_LITERAL(str) SK_UTF16_LITERAL_II(str)


#define SkU16(str) \
    ([]() -> StringT<::sketch::Allocator<UChar>> { \
        enum { Size = sizeof(SK_UTF16_LITERAL(str)) / sizeof(::sketch::UChar) }; \
        static const ::sketch::internal::StaticStringData<Size, ::sketch::Allocator<UChar>> string_literal = { \
            { ::sketch::refcount::Persistent, Size, 0, sizeof(::sketch::internal::StringData<::sketch::Allocator<UChar>>) }, \
            SK_UTF16_LITERAL(str) }; \
        ::sketch::internal::StringDataRef<::sketch::Allocator<UChar>> holder = { const_cast<::sketch::internal::StringData<::sketch::Allocator<UChar>>*>(&string_literal.header) }; \
        const ::sketch::StringT<::sketch::Allocator<UChar>> s(holder, false); \
        return s; \
    }()) \




} // namespace sketch {}
