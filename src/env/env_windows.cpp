#include "env.hpp"
#include <malloc.h>

void* EnvUtils::AllocAligned(size_t size, size_t alignment)
{
  return reinterpret_cast<void*>(_aligned_malloc(size, alignment));
}