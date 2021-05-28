#pragma once

#include "../Sketch.hxx"


namespace sketch
{

struct ErrorState
{
    enum class Mode
    {
        throwIfExceptionsSupported,
        neverThrow
    };

    static bool areExceptionsSupported() noexcept
    {
#ifdef SKETCH_CXX_EXCEPTIONS
        return true;
#else
        return false;
#endif
    }

    explicit ErrorState(Mode mode = Mode::throwIfExceptionsSupported) noexcept
#ifdef SKETCH_CXX_EXCEPTIONS
        : m_mode(mode)
#endif
    {
        SK_UNUSED(mode);
    }

    Error::Ref lastError() const noexcept
    {
        return m_error;
    }

    bool anyErrors() const noexcept
    {
        return !!m_error;
    }

    Mode mode() const noexcept
    {
#ifdef SKETCH_CXX_EXCEPTIONS
        return m_mode;
#else
        return Mode::neverThrow;
#endif
    }

    void setMode(Mode mode) noexcept
    {
#ifdef SKETCH_CXX_EXCEPTIONS
        m_mode = mode;
#else
        SK_UNUSED(mode);
#endif
    }

protected:
    void setLastError(Error::Ref e) const
    {
        m_error = e;

#ifdef SKETCH_CXX_EXCEPTIONS
        if (m_mode == Mode::throwIfExceptionsSupported)
        {
            throw Exception(e);
        }
#endif
    }

    void clearLastError() noexcept
    {
        m_error.release();
    }

private:
    mutable Error::Ref m_error;
#ifdef SKETCH_CXX_EXCEPTIONS
    Mode m_mode;
#endif
};


} // namespace sketch {}
