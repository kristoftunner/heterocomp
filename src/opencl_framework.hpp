#pragma once

#include "common.hpp"
#include "CL/cl.h"

static std::string ReadSourceFromFile(const std::experimental::filesystem::path &path)
{
  std::string source;
  std::ifstream file(path, std::ios::in);
  if(file.good())
  {
    file.unsetf(std::ios::skipws);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  }
  else
    throw std::runtime_error("wrong .cl file");
}

enum class PlatformType {
  PLATFROM_GPU,
  PLATFORM_CPU
};

class OpenclFramework {
public:
  void QueryPlatforms();
  void CheckError(int error);
private:
  void ChoosePlatform();

  unsigned int m_numberOfPlatforms;
  std::vector<cl_platform_id> m_platformIds;
  std::vector<std::string> m_platformNames;
  std::vector<uint32_t> m_devicesPerPlatform;
};


