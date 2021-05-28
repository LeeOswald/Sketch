#pragma once

#include "SkUser.hxx"

#include <SkBase/Pair.hxx>
#include <SkBase/Vector.hxx>

#ifdef SKETCH_STL_AVAILABLE
#include <cstdlib>
#endif


namespace sketch
{


class CommandLine
{
public:
    bool valid() const noexcept
    {
        return m_valid;
    }

    ~CommandLine() noexcept
    {
    }

    CommandLine() noexcept
    {
    }

    explicit CommandLine(const String& s);

    const Vector<Pair<String, String>>& items() const noexcept
    {
        return m_items;
    }

    bool present(const String& flag) const noexcept
    {
        for (const auto& it : m_items)
        {
            if (it.first == flag)
            {
                return true;
            }
        }

        return false;
    }

    Vector<String> values(const String& name = String()) const
    {
        Vector<String> v;

        for (const auto& it : m_items)
        {
            if (it.first == name)
            {
                if (!it.second.empty())
                {
                    v.push_back(it.second);
                }
            }
        }

        return v;
    }

    bool singleValue(String& value, const String& name = String()) const
    {
        for (const auto& it : m_items)
        {
            if (it.first == name)
            {
                if (!it.second.empty())
                {
                    value = it.second;
                    return true;
                }
            }
        }

        return false;
    }

#ifdef SKETCH_STL_AVAILABLE

    bool singleValue(long& value, const String& name) const
    {
        String s;
        if (!singleValue(s, name))
        {
            return false;
        }

        auto begin = s.utf16();
        wchar_t* end = nullptr;
        auto l = std::wcstol(begin, &end, 10);
        if (end == begin)
        {
            return false;
        }

        value = l;
        return true;
    }

    bool singleValue(double& value, const String& name) const
    {
        String s;
        if (!singleValue(s, name))
        {
            return false;
        }

        auto begin = s.utf16();
        wchar_t* end = nullptr;
        auto l = std::wcstod(begin, &end);
        if (end == begin)
        {
            return false;
        }

        value = l;
        return true;
    }

#endif // SKETCH_STL_AVAILABLE

private:
    Vector<Pair<String, String>> m_items;
    bool m_valid;
};


} // namespace sketch {}
