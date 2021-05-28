#include <Sketch.hxx>


namespace sketch
{

namespace
{
#ifdef SKETCH_DBG
#include "ntstatus.inl"
#endif

} // namespace {}

namespace nt
{

String getErrorText(error_t r) noexcept
{
#ifdef SKETCH_DBG
    return String(lookupNTSTATUSName(r));
#else
    SK_UNUSED(r);
    return String();
#endif
}


} // namespace nt {}

} // namespace sketch {}
