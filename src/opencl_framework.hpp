#pragma once

#include "common.hpp"
#include <map>

#include "CL/cl.h"
#include "utils.hpp"
#include "env/env.hpp"

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
  std::vector<std::pair<cl_device_type, cl_device_id>> devices;

  bool HasDeviceType(const DeviceType& devType)
  {
    for(auto dev : devices)
    {
      switch(devType)
      {
        case DeviceType::PLATFORM_CPU:
          if(dev.first == CL_DEVICE_TYPE_CPU)
            return true;
          break;
        case DeviceType::PLATFROM_GPU:
          if(dev.first == CL_DEVICE_TYPE_GPU)
            return true;
          break;
        default:
          break;
      }
    }
    return false;
  }

  cl_device_id GetDevice(const DeviceType& devType)
  {
    for(auto dev : devices)
    {
      switch (devType)
      {
        case DeviceType::PLATFORM_CPU:
          if(dev.first == CL_DEVICE_TYPE_CPU)
            return dev.second;
          break;
        case DeviceType::PLATFROM_GPU:
          if(dev.first == CL_DEVICE_TYPE_GPU)
            return dev.second;
          break;
        default:
          break;
      }
    }
  }
};

class CLFramework;

template<typename T>
struct CLMemory {
  T* hostPtr;
  size_t hostSize;
  cl_mem destPtr;

  /* propagate the memory flags somehow to the ctor*/
  CLMemory(cl_context context, uint64_t flags, size_t size)
  {
    size_t optimizedSize = ((sizeof(T) * size - 1) / 64 + 1) * 64;
    hostPtr = reinterpret_cast<T*>(EnvUtils::AllocAligned(optimizedSize, 4096));
    memset(reinterpret_cast<void*>(hostPtr), 0, size);
    cl_int err;
    destPtr = clCreateBuffer(context, flags, sizeof(T) * size, hostPtr, &err);
    CLFramework::CheckError(err);
  }

  ~CLMemory()
  {
    //_aligned_free(hostPtr);
    cl_int err = clReleaseMemObject(destPtr);
  }
  
  /* cctor and copy assignment cannot be done because of the memory release */
  CLMemory(const CLMemory&) = delete;
  CLMemory& operator=(const CLMemory&) = delete;
};

class CLFramework {
public:
  ~CLFramework();
  void QueryPlatforms();
  static void CheckError(int error);
  void ChoosePlatform(const std::string& platformName, const DeviceType& type);
  void CreateContext();
  void BuildKernel(const std::experimental::filesystem::path& path, const int numberOfArguments);
  template<typename T>
  void SetKernelBufferArg(const int index, CLMemory<T>& mem);
  template<typename T>
  const T* GetKernelOutput(CLMemory<T> clMemory);
  void RunKernel(std::vector<size_t>& globalWorkSize);
  cl_context GetContext(){return m_context;}
private:
  std::map<std::string, PlatformInfo> m_platforms;
  cl_context_properties m_properties;
  cl_device_id m_selectedDeviceId;
  cl_platform_id m_selectedPlatformId;
  cl_context m_context;
  cl_command_queue m_commandQueue;
  cl_kernel m_kernel;
  int m_numberOfKernelArguments;
  cl_program m_program;
  Benchmark m_benchmark;
};


template<typename T>
void CLFramework::SetKernelBufferArg(const int index, CLMemory<T>& mem)
{
  if (index < m_numberOfKernelArguments)
  {
    cl_int err = clSetKernelArg(m_kernel, index, sizeof(cl_mem), reinterpret_cast<const void*>(&(mem.destPtr)));
    CheckError(err);
  }
  else
  {
    throw std::runtime_error("Wrong kernel argument index");
  }
}

