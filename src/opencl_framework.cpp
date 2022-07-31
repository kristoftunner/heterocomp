#include "opencl_framework.hpp"

void CLFramework::QueryPlatforms()
{
  /* get the platforms and the names*/
  uint32_t numberOfPlatforms;
  int err = clGetPlatformIDs(0, nullptr, &numberOfPlatforms);
  CheckError(err);
  std::vector<cl_platform_id> platformIds;
  platformIds.resize(numberOfPlatforms);
  err = clGetPlatformIDs(numberOfPlatforms, platformIds.data(), nullptr);
  for(auto platformId : platformIds)
  {
    PlatformInfo info;
    info.platformId = platformId;
    size_t nameLength = 0;
    err = clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, nullptr, &nameLength);
    CheckError(err);
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
    CheckError(err);
    m_platforms[platformName] = info;
  }
}

static std::string TranslateError(int error)
{
  switch (error)
  {
  case CL_SUCCESS:                            return "CL_SUCCESS";
  case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
  case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
  case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
  case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
  case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
  case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
  case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
  case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
  case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
  case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
  case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
  case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
  case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
  case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
  case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
  case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
  case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
  case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
  case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
  case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
  case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
  case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
  case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
  case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
  case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
  case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
  case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
  case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
  case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
  case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
  case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
  case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
  case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
  case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
  case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
  case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
  case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
  case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
  case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
  case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
  case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
  case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
  case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
  case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
  case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
  case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
  case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
  case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
  case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
  case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
  case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
//    case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
//    case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70    

  default:
    return "UNKNOWN ERROR CODE";
  }
}

void CLFramework::CheckError(int error)
{
  if (error != CL_SUCCESS)
  {
    std::cout << TranslateError(error) << std::endl;
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
  CheckError(err);
  
  const cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  m_commandQueue = clCreateCommandQueueWithProperties(m_context, m_selectedDeviceId, properties, &err);
  CheckError(err);
}

void CLFramework::BuildKernel(const std::experimental::filesystem::path& path, const int numberOfArguments)
{
  m_numberOfKernelArguments = numberOfArguments;
  const std::string source = ReadSourceFromFile("../src/kernels/add.cl");
  const size_t sourceSize = source.size();
  const char* sourceData = source.data();
  cl_int err;
  m_program = clCreateProgramWithSource(m_context, 1, &sourceData, &sourceSize, &err);
  /* build the program explicitly*/
  CheckError(err);
  err = clBuildProgram(m_program, 1, const_cast<const cl_device_id*>(&m_selectedDeviceId), "", nullptr, nullptr);
  if(err != CL_SUCCESS)
  {
    CheckError(err);
    if(err == CL_BUILD_PROGRAM_FAILURE)
    {
      size_t logSize = 0;
      clGetProgramBuildInfo(m_program, m_selectedDeviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
      std::string buildLog;
      buildLog.resize(logSize);
      clGetProgramBuildInfo(m_program, m_selectedDeviceId, CL_PROGRAM_BUILD_LOG, logSize, const_cast<char*>(buildLog.data()), nullptr);
    }
    throw std::runtime_error("Unable to build program");
  }
  m_kernel = clCreateKernel(m_program, "Add", &err);
  CheckError(err);
}

void CLFramework::RunKernel(std::vector<size_t>& globalWorkSize)
{
  cl_int err;
  m_benchmark.StartMark("kernel run");
  /* TODO: this can be pimped later */
  err = clEnqueueNDRangeKernel(m_commandQueue, m_kernel, globalWorkSize.size(), nullptr, globalWorkSize.data(), nullptr, 0, nullptr, nullptr);
  CheckError(err);
  clFinish(m_commandQueue);
  m_benchmark.StopMark();
}

CLFramework::~CLFramework()
{
  cl_int err = clReleaseKernel(m_kernel);
  CheckError(err);
  if (m_program)
  {
    err = clReleaseProgram(m_program);
    CheckError(err);
  }
  if (m_commandQueue)
  {
    err = clReleaseCommandQueue(m_commandQueue);
    CheckError(err);
  }
  if (m_selectedDeviceId)
  {
    err = clReleaseDevice(m_selectedDeviceId);
    CheckError(err);
  }
  if (m_context)
  {
    err = clReleaseContext(m_context);
    CheckError(err);
  }
}
