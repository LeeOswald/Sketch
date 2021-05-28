#pragma once

#ifdef SKETCH_CXX_EXCEPTIONS

#include "Error.hxx"

#ifdef SKETCH_STL_AVAILABLE
#include <exception>
#endif


namespace sketch
{

class Exception
#ifdef SKETCH_STL_AVAILABLE
    : public std::exception
#endif
{
public:
    ~Exception() noexcept
    {
    }

    Exception() noexcept
    {
    }

    Exception(Error::Ref e)
        : m_error(e)
    {
    }

    Error::Ref error() const noexcept
    {
        return m_error;
    }

#ifdef SKETCH_STL_AVAILABLE
    const char* what() const override
    {
        if (!m_error)
        {
            return "sketch::Exception";
        }

        if (m_cachedText.empty())
        {
#ifdef SKETCH_WIN
            m_cachedText = AnsiString(m_error->errorText());
#else
            m_cachedText = Utf8String(m_error->errorText());
#endif
        }

        return m_cachedText.str();
    }
#endif

private:
    Error::Ref m_error;
#ifdef SKETCH_STL_AVAILABLE

#ifdef SKETCH_WIN
    mutable AnsiString m_cachedText;
#else
    mutable Utf8String m_cachedText;
#endif

#endif
};


} // namespace sketch {}


#endif // SKETCH_CXX_EXCEPTIONS
