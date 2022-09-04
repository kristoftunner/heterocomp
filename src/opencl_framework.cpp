#include "opencl_framework.hpp"

void CLFramework::QueryPlatforms()
{
  /* get the platforms and the names*/
  uint32_t numberOfPlatforms;
  int err = clGetPlatformIDs(0, nullptr, &numberOfPlatforms);
  CLUtils::CheckError(err);
  std::vector<cl_platform_id> platformIds;
  platformIds.resize(numberOfPlatforms);
  err = clGetPlatformIDs(numberOfPlatforms, platformIds.data(), nullptr);
  for(auto platformId : platformIds)
  {
    PlatformInfo info;
    info.platformId = platformId;
    size_t nameLength = 0;
    err = clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, nullptr, &nameLength);
    CLUtils::CheckError(err);
    std::string platformName;
    platformName.resize(nameLength);
    err = clGetPlatformInfo(platformId, CL_PLATFORM_NAME, nameLength, const_cast<char*>(platformName.data()), nullptr);
    platformName.erase(platformName.size()-1);
    uint32_t numberOfDevices = 0;
    err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, nullptr, &numberOfDevices);
    if(err == CL_SUCCESS)
    {
      std::vector<cl_device_id> devices(numberOfDevices);
      err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, numberOfDevices, devices.data(), nullptr);

      for(auto dev : devices)
      {
        cl_device_type devType;
        err = clGetDeviceInfo(dev, CL_DEVICE_TYPE, sizeof(cl_device_type), &devType, nullptr);
        info.devices.push_back({devType, dev});
      }
    }
    CLUtils::CheckError(err);
    m_platforms[platformName] = info;
  }
}


void CLFramework::ChoosePlatform(const std::string& platformName, const DeviceType& type)
{
  if(m_platforms.find(platformName) != m_platforms.end())
  {
    if(m_platforms.at(platformName).HasDeviceType(type))
    {
      PlatformInfo info = m_platforms.at(platformName);
      m_selectedPlatformId = info.platformId;
      m_selectedDeviceId = info.GetDevice(type);
      return;
    }
  }
  
  throw std::runtime_error("Choosing OpenCL platform failed");
}

void CLFramework::CreateContext()
{
  /* create context and command queue, in order to create kernels and */
  cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)m_selectedPlatformId, 0 };
  cl_int err;
  m_context = clCreateContext(contextProperties, 1, &m_selectedDeviceId, nullptr, nullptr, &err);
  CLUtils::CheckError(err);
  
  const cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  m_commandQueue = clCreateCommandQueueWithProperties(m_context, m_selectedDeviceId, properties, &err);
  CLUtils::CheckError(err);
}

void CLFramework::BuildKernel(const std::string& path, const int numberOfArguments, const std::string& kernelName)
{
  m_numberOfKernelArguments = numberOfArguments;
  const std::string source = ReadSourceFromFile(path);
  const size_t sourceSize = source.size();
  const char* sourceData = source.data();
  cl_int err;
  cl_program program = clCreateProgramWithSource(m_context, 1, &sourceData, &sourceSize, &err);
  /* build the program explicitly*/
  CLUtils::CheckError(err);
  err = clBuildProgram(program, 1, const_cast<const cl_device_id*>(&m_selectedDeviceId), "", nullptr, nullptr);
  if(err != CL_SUCCESS)
  {
    CLUtils::CheckError(err);
    if(err == CL_BUILD_PROGRAM_FAILURE)
    {
      size_t logSize = 0;
      clGetProgramBuildInfo(program, m_selectedDeviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
      std::string buildLog;
      buildLog.resize(logSize);
      clGetProgramBuildInfo(program, m_selectedDeviceId, CL_PROGRAM_BUILD_LOG, logSize, const_cast<char*>(buildLog.data()), nullptr);
      std::cout << buildLog << std::endl;
    }
    throw std::runtime_error("Unable to build program");
  }
  cl_kernel kernel = clCreateKernel(program, kernelName.c_str(), &err);
  m_programs.push_back(program);
  m_kernels.push_back(kernel);
  CLUtils::CheckError(err);
}

void CLFramework::RunKernel(std::vector<size_t>& globalWorkSize, const int kernelIndex)
{
  cl_int err;
  m_benchmark.StartMark("kernel run");
  /* TODO: this can be pimped later */
  err = clEnqueueNDRangeKernel(m_commandQueue, m_kernels[kernelIndex], globalWorkSize.size(), nullptr, globalWorkSize.data(), nullptr, 0, nullptr, nullptr);
  CLUtils::CheckError(err);
  clFinish(m_commandQueue);
  m_benchmark.StopMark();
}

CLFramework::~CLFramework()
{
  cl_int err;
  for(auto kernel : m_kernels)
  {
    if(kernel)
    {
      err = clReleaseKernel(kernel);
      CLUtils::CheckError(err);
    }
  }
  for(auto program : m_programs)
  {
    if(program)
    {
      err = clReleaseProgram(program);
      CLUtils::CheckError(err);
    }
  }
  if (m_commandQueue)
  {
    err = clReleaseCommandQueue(m_commandQueue);
    CLUtils::CheckError(err);
  }
  if (m_selectedDeviceId)
  {
    err = clReleaseDevice(m_selectedDeviceId);
    CLUtils::CheckError(err);
  }
  if (m_context)
  {
    err = clReleaseContext(m_context);
    CLUtils::CheckError(err);
  }
}
