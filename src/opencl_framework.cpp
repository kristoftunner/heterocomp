#include "opencl_framework.hpp"

void OpenclFramework::QueryPlatforms()
{
  /* get the platforms and the names*/
  uint32_t numberOfPlatforms;
  int err = clGetPlatformIDs(0, NULL, &numberOfPlatforms);
  CheckError(err);
  m_platformIds.resize(numberOfPlatforms);
  err = clGetPlatformIDs(numberOfPlatforms, m_platformIds.data(), NULL);
  for(auto platform : m_platformIds)
  {
    size_t nameLength = 0;
    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &nameLength);
    CheckError(err);
    std::string platformName;
    platformName.resize(nameLength);
    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, nameLength, const_cast<char*>(platformName.data()), NULL);
    m_platformNames.emplace_back(platformName);
    uint32_t numberOfDevices;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numberOfDevices);
    CheckError(err);
    m_devicesPerPlatform.push_back(numberOfDevices);
  }
}