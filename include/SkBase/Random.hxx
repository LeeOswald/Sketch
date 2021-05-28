#pragma once

#include "../Sketch.hxx"


// cryptographic random generator


namespace sketch
{

namespace random
{

bool generate(void* buffer, size_t size) noexcept;

} // namespace random {}

} // namespace sketch {}
