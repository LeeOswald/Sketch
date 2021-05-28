#include <Sketch.hxx>


namespace sketch
{


String Error::format(const UChar* format) const noexcept
{
    SK_ASSERT(format);
    if (!format)
    {
        return String();
    }

    String result;

    auto p = format;
    while (*p)
    {
        if (*p == u'{')
        {
            if (*(p + 1) == u'{') // {{
            {
                result.append(u"{");
                p += 2;
            }
            else if ((*(p + 1) == u'T') && (*(p + 2) == u'}')) // {T}
            {
                result.append(typeName());
                p += 3;
            }
            else if ((*(p + 1) == u'C') && (*(p + 2) == u'd') && (*(p + 3) == u'}')) // {Cd}
            {
                if (code() != 0)
                {
                    result.append(String::format(u"%d", code()));
                }

                p += 4;
            }
            else if ((*(p + 1) == u'C') && (*(p + 2) == u'x') && (*(p + 3) == u'}')) // {Cx}
            {
                if (code() != 0)
                {
                    result.append(String::format(u"%08x", code()));
                }

                p += 4;
            }
            else if ((*(p + 1) == u'M') && (*(p + 2) == u'}')) // {M}
            {
                result.append(m_message);
                p += 3;
            }
            else if ((*(p + 1) == u'S') && (*(p + 2) == u'}')) // {S}
            {
                result.append(errorText());
                p += 3;
            }
            else if ((*(p + 1) == u'M') && (*(p + 2) == u'S') && (*(p + 3) == u'}')) // {MS}
            {
                auto es = errorText();
                if (!m_message.empty() && !es.empty())
                {
                    result.append(m_message);
                    result.append(u" [");
                    result.append(es);
                    result.append(u"]");
                }
                else // append whatever is present
                {
                    result.append(m_message);
                    result.append(es);
                }

                p += 4;
            }
            else if ((*(p + 1) == u'F') && (*(p + 2) == u'}')) // {F}
            {
                if (m_file)
                {
                    result.append(m_file);
                }

                p += 3;
            }
            else if ((*(p + 1) == u'L') && (*(p + 2) == u'}')) // {L}
            {
                if (m_line)
                {
                    result.append(String::format(u"%d", m_line));
                }

                p += 3;
            }
            else if ((*(p + 1) == u'F') && (*(p + 2) == u'L') && (*(p + 3) == u'}')) // {FL}
            {
                if (m_file)
                {
                    result.append(String::format(u"%S:%d", m_file, m_line));
                }

                p += 4;
            }
            else // some unrecognized tag
            {
                result.append(p, 1);
                p++;
            }
        }
        else
        {
            result.append(p, 1);
            p++;
        }
    }

    return result;
}


String GenericError::errorText() const noexcept
{
    switch (code())
    {
    case EUnknown: return SkU16("Unknown error");
    case EOutOfMemory: return SkU16("Out of memory");
    case ETimedOut: return SkU16("Timed out");
    case EIvalidArg: return SkU16("Invalid argument");
    case ENotImplemented: return SkU16("Not implemented");
    case EInvalidRequest: return SkU16("Invalid request");
    }

    return String();
}


void logError(const Error::Ref& r) noexcept
{
    auto u = r;
    while (u)
    {
        String s;
        if (u->code() != 0)
        {
            if (u->file())
            {
                s = u->format(u"{Cx} {MS} [{FL}]");
            }
            else
            {
                s = u->format(u"{Cx} {MS}");
            }
        }
        else
        {
            if (u->file())
            {
                s = u->format(u"{MS} [{FL}]");
            }
            else
            {
                s = u->format(u"{MS}");
            }
        }

        SketchError("%s", toSystemLocale(s).str());
        u = u->inner();
    }
}


} // namespace sketch {}
