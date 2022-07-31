#pragma once
/**
 * @brief placeholder class for OS specific function wrappers 
 * 
 */
class EnvUtils {
public:
  static void* AllocAligned(size_t size, size_t alignemnt);
};