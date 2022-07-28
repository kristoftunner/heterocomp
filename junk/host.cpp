#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include "CL/cl.h"
#include <vector>

#include "utils.hpp"
#include <Windows.h>

// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller
std::string ReadSourceFromFile(const std::experimental::filesystem::path &path)
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

int main()
{
  LARGE_INTEGER perfFrequency;
  LARGE_INTEGER performanceCountNDRangeStart;
  LARGE_INTEGER performanceCountNDRangeStop;
  cl_uint numberOfPlatforms;
  int err = clGetPlatformIDs(0, NULL, &numberOfPlatforms);
  if (err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  if (numberOfPlatforms == 0)
  {
    throw std::runtime_error("not enough platforms");
  }

  std::vector<cl_platform_id> platforms(numberOfPlatforms);

  err = clGetPlatformIDs(numberOfPlatforms, platforms.data(), NULL);
  std::vector<std::string> platformNames;
  for (cl_platform_id platform : platforms)
  {
    size_t nameLength = 0;
    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0,NULL, &nameLength);
    if (err != CL_SUCCESS)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
    std::string platformName;
    platformName.resize(nameLength);
    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, nameLength, const_cast<char*>(platformName.data()), NULL);
    platformNames.emplace_back(platformName);
  }

  cl_uint numberOfDevices;
  err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &numberOfDevices);
  if (err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }

  cl_platform_id selectedPlatformId = platforms[0];
  cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)selectedPlatformId,0 };
  cl_context context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
  if (err != CL_SUCCESS || context == NULL)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  cl_device_id selectedDevice;
  err = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &selectedDevice, NULL);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
  }
  
  const cl_command_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  cl_command_queue commandQueue = clCreateCommandQueueWithProperties(context, selectedDevice, properties, &err);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
  }

  /* allocate the required buffer, 4K page size*/
  const int arrayWidth = 1024;
  const int arrayHeight = 1024;
  cl_uint optimizedSize = ((sizeof(cl_int) * arrayWidth * arrayHeight - 1)/64 + 1) * 64;
  cl_int* inputA = (cl_int*)_aligned_malloc(optimizedSize, 4096);
  cl_int* inputB = (cl_int*)_aligned_malloc(optimizedSize, 4096);
  cl_int* outputC = (cl_int*)_aligned_malloc(optimizedSize, 4096);
  if(inputA == nullptr || inputB == nullptr || outputC == nullptr)
    throw std::runtime_error("no success");

  /* create the openCL buffers */
  cl_mem srcA = clCreateBuffer((context), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_int) * arrayWidth * arrayHeight, inputA, &err);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  cl_mem srcB = clCreateBuffer((context), CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_int) * arrayWidth * arrayHeight, inputB, &err);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  cl_mem destC = clCreateBuffer((context), CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_int) * arrayWidth * arrayHeight, outputC, &err);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }

  /* generate here some inputs and oututs */
  int valA = 1;
  int valB = 10;
  for(auto i = 0; i < arrayHeight*arrayWidth; i++)
  {
    inputA[i] = valA;
    inputB[i] = valB;
  }  
  std::string source = ReadSourceFromFile("../src/kernels/add.cl");
  const size_t sourceSize = source.size();
  const char* sourceData = source.data();
  cl_program program = clCreateProgramWithSource(context, 1, &sourceData, &sourceSize, &err);
  /* build the program explicitly*/
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  err = clBuildProgram(program, 1, const_cast<const cl_device_id*>(&selectedDevice), "", nullptr, nullptr);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    if(err == CL_BUILD_PROGRAM_FAILURE)
    {
      size_t logSize = 0;
      clGetProgramBuildInfo(program, selectedDevice, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
      std::string buildLog;
      buildLog.resize(logSize);
      clGetProgramBuildInfo(program, selectedDevice, CL_PROGRAM_BUILD_LOG, logSize, const_cast<char*>(buildLog.data()), nullptr);
      //std::cout << buildLog << std::endl;
    }
    throw std::runtime_error("no success");
  }

  cl_kernel kernel = clCreateKernel(program, "Add", &err);
  if(err != CL_SUCCESS)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  
  /* setting the kernel arguments */
  err  =  clSetKernelArg(kernel, 0, sizeof(cl_mem), reinterpret_cast<const void*>(&srcA));
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  err  =  clSetKernelArg(kernel, 1, sizeof(cl_mem), reinterpret_cast<const void*>(&srcB));
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  err  =  clSetKernelArg(kernel, 2, sizeof(cl_mem), reinterpret_cast<const void*>(&destC));
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }

  bool queueProfilingEnable = true;
  if (queueProfilingEnable)
      QueryPerformanceCounter(&performanceCountNDRangeStart);

  /* execute the kernel */
  size_t globalWorkSize[2] = {arrayWidth, arrayHeight};
  err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }

  err = clFinish(commandQueue);
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  if (queueProfilingEnable)
    QueryPerformanceCounter(&performanceCountNDRangeStop);

  cl_int *resultPtr = reinterpret_cast<cl_int*>(clEnqueueMapBuffer(commandQueue, destC, true, CL_MAP_READ, 0, sizeof(cl_uint)*arrayWidth*arrayHeight, 0, nullptr, nullptr, &err));
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }

  err = clFinish(commandQueue);
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  for(auto i = 0; i < arrayHeight*arrayHeight; i++)
  {
    if(resultPtr[i] != inputA[i] + inputB[i])
    {
      std::cout << "verification failed" << std::endl; 
    }
  }

  /* unmap the resultPtr before releasing */
  err = clEnqueueUnmapMemObject(commandQueue, destC, resultPtr, 0, nullptr, nullptr);
  if (CL_SUCCESS != err)
  {
    std::cout << TranslateOpenCLError(err) << std::endl;
    throw std::runtime_error("no success");
  }
  QueryPerformanceFrequency(&perfFrequency);
  std::cout << "NDRange performance counter time ms. : "<< (1000.0f*static_cast<float>(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / static_cast<float>(perfFrequency.QuadPart)) << std::endl;
  _aligned_free(inputA);
  _aligned_free(inputB);
  _aligned_free(outputC);

  /* release everything*/
  if(kernel)
  {
    err = clReleaseKernel(kernel);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(program)
  {
    err = clReleaseProgram(program);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(srcA)
  {
    err = clReleaseMemObject(srcA);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(srcB)
  {
    err = clReleaseMemObject(srcB);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(destC)
  {
    err = clReleaseMemObject(destC);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(commandQueue)
  {
    err = clReleaseCommandQueue(commandQueue);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(selectedDevice)
  {
    err = clReleaseDevice(selectedDevice);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  if(context)
  {
    err = clReleaseContext(context);
    if (CL_SUCCESS != err)
    {
      std::cout << TranslateOpenCLError(err) << std::endl;
      throw std::runtime_error("no success");
    }
  }
  std::cout << "end" << std::endl;
  return 0;
}