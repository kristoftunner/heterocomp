#include "env.hpp"
#include <cstdlib>

void* EnvUtils::AllocAligned(size_t size, size_t alignment)
{
  return reinterpret_cast<void*>(std::aligned_alloc(alignment, size));
}
