#pragma once

#include "common.hpp"
#include <map>

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

enum class DeviceType {
  PLATFROM_GPU,
  PLATFORM_CPU,
  PLATFORM_OTHER
};
struct PlatformInfo{
  cl_platform_id platformId;
  std::vector<cl_device_type> devices;

  bool HasDeviceType(const cl_device_type& devType)
  {
    for(auto dev : devices)
    {
      if(dev == devType)
        return true;
    }
    return false;
  }
};
class OpenclFramework {
public:
  void QueryPlatforms();
  void CheckError(int error);
private:
  void ChoosePlatform(const std::string& platformName, const DeviceType& type);

  std::map<std::string, PlatformInfo> m_platforms;
  //unsigned int m_numberOfPlatforms;
  //std::vector<cl_platform_id> m_platformIds;
  //std::vector<std::string> m_platformNames;
  //std::vector<uint32_t> m_devicesPerPlatform;
};


