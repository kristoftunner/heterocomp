#pragma once
#include "../common.hpp"
/**
 * @brief placeholder class for OS specific function wrappers 
 * 
 */
class EnvUtils {
public:
  void Print();
  static void* AllocAligned(size_t size, size_t alignemnt);
};