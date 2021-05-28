#include <SkUser/CommandLine.hxx>

#include <cctype>


namespace sketch
{

namespace
{

String _unquote(const UChar* s, size_t length)
{
    if ((*s != u'\"') || (length < 2))
    {
        return String(s, length);
    }

    if (s[length - 1] != u'\"')
    {
        return String(s, length);
    }

    return String(s + 1, length - 2);
}

bool _split(const String& s, Vector<String>& v)
{
    size_t parts = 0;
    auto p = s.str();
    while (*p)
    {
        // skip spaces
        while (*p && ::iswspace(*p)) p++;

        auto start = p;
        size_t length = 0;
        bool quotes = false;

        while (auto c = *p)
        {
            p++;

            if (c == u'\"')
            {
                quotes = !quotes;
            }
            else if (::iswspace(c))
            {
                if (!quotes)
                {
                    break;
                }
            }

            length++;
        }

        if (length)
        {
            if (!v.push_back(String(start, length)))
            {
                return false;
            }

            parts++;
        }
    }

    return (parts > 0);
}

} // namespace {}


CommandLine::CommandLine(const String& cl)
    : m_valid(false)
{
    Vector<String> rawItems;
    if (_split(cl, rawItems))
    {
        for (const auto& i : rawItems)
        {
            auto s = i.str();
            if (
                (*s == u'/') ||
                (*s == u'-')
                )
            {
                String name;
                String value;

                if ((*s == '-') && (*(s + 1) == '-'))
                {
                    s += 2;
                }
                else
                {
                    s++;
                }

                auto start = s;
                size_t length = 0;

                while (*s)
                {
                    if (
                        (*s == u'\"') ||
                        (*s == u':') ||
                        (*s == u'=')
                        )
                    {
                        break;
                    }

                    length++;
                    s++;
                }

                if (length)
                {
                    name = String(start, length);
                }

                if (!*s)
                {
                    // just a flag like /D or -D
                    m_items.push_back(Pair<String, String>(name, String()));
                    continue;
                }

                // flag with a value like /D:90 or -D=0 or -D"67"
                SK_ASSERT((*s == u'\"') || (*s == u':') || (*s == u'='));
                bool quote = (*s == u'\"');

                s++;

                start = s;
                length = 0;

                while (*s)
                {
                    if ((*s == u'\"') && quote)
                    {
                        break;
                    }

                    length++;
                    s++;
                }

                if (length)
                {
                    value = _unquote(start, length);
                }

                m_items.push_back(Pair<String, String>(name, value));
                continue;
            }

            // a value
            String value = _unquote(i.str(), i.length());
            if (!m_items.empty())
            {
                auto it = m_items.end() - 1;
                if (
                    !it->first.empty() &&
                    it->second.empty()
                    )
                {
                    it->second = value;
                    continue;
                }
            }

            m_items.push_back(Pair<String, String>(String(), value));
        }

        m_valid = true;
    }
}


} // namespace sketch {}
