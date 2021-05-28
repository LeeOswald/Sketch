#include <Sketch.hxx>

#ifdef SKETCH_CXX_EXCEPTIONS

namespace sketch
{


void throwOutOfMemoryException(const char* file, int line)
{
    throw Exception(GenericError::make(GenericError::EOutOfMemory, String(), file, line));
}

} // namespace sketch {}

#endif // SKETCH_CXX_EXCEPTIONS
