#pragma once

#include <Sketch.hxx>

namespace sketch
{

namespace nt
{

class FloatingPointSaveScope
    : public NonCopyable
{
public:
    ~FloatingPointSaveScope() noexcept
    {
        if (m_valid)
        {
            SK_VERIFY(NT_SUCCESS(KeRestoreFloatingPointState(&m_save)));
        }
    }

    FloatingPointSaveScope() noexcept
    {
        m_valid = NT_SUCCESS(KeSaveFloatingPointState(&m_save));
        SK_ASSERT(m_valid);
    }

    bool valid() const noexcept
    {
        return m_valid;
    }

private:
    KFLOATING_SAVE m_save;
    bool m_valid;
};


} // namespace nt {}

} // namespace sketch {}


#ifdef SKETCH_64

__inline long int
lrint(double flt)
{
    return _mm_cvtsd_si32(_mm_load_sd(&flt));
}

__inline long int
lrintf(float flt)
{
    return _mm_cvtss_si32(_mm_load_ss(&flt));
}

#else

__inline long int
lrint(double flt)
{
    int intgr;

    _asm
    {	fld flt
        fistp intgr
    };

    return intgr;
}

__inline long int
lrintf(float flt)
{
    int intgr;

    _asm
    {	fld flt
        fistp intgr
    };

    return intgr;
}

#endif

