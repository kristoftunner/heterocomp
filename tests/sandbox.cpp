#include "opencl_framework.hpp"
#include <fstream>
#include <iterator>

static std::vector<uint8_t> ReadFileIntoMemory(const std::string filePath)
{
  std::ifstream file(filePath, std::ios::binary);
  if (file.good()) {
    file.unsetf(std::ios::skipws);
    std::vector<uint8_t> result;
    result.assign(std::istream_iterator<uint8_t>(file),
                  std::istream_iterator<uint8_t>());
    return result;
  } else {
    throw std::runtime_error("wrong filename!");
  }
}

int main()
{
  CLFramework framework;
  framework.QueryPlatforms();
  const std::string platformName = "Intel(R) OpenCL HD Graphics";
  framework.ChoosePlatform(platformName, DeviceType::PLATFROM_GPU);
  framework.CreateContext();
  framework.BuildKernel("../src/kernels/add.cl", 3, "Add");

  /* working with arrays */ 
  const size_t arrayWidth = 100;
  const size_t arrayHeight = 100;
  const size_t arrayDepth = 3;
  CLMemory<cl_int> srcA(framework.GetContext(),CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight * arrayDepth);
  CLMemory<cl_int> srcB(framework.GetContext(),CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight * arrayDepth);
  CLMemory<cl_int> destC(framework.GetContext(),CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight * arrayDepth);
  
  /* generate here some inputs and oututs */
  int valA = 1;
  int valB = 10;
  for(auto i = 0; i < arrayHeight*arrayWidth*arrayDepth; i++)
  {
    srcA.hostPtr[i] = valA;
    srcB.hostPtr[i] = valB;
  }  

  /* arguments for Add kernel */ 
  framework.SetKernelBufferArg<cl_int>(0, 0, srcA);
  framework.SetKernelBufferArg<cl_int>(0, 1, srcB);
  framework.SetKernelBufferArg<cl_int>(0, 2, destC);
  /* Arguments for Blend kernel */
  cl_int err = clSetKernelArg(framework.GetKernel(0), 0, sizeof(cl_int), &srcA);
  CLUtils::CheckError(err);
  err = clSetKernelArg(framework.GetKernel(0), 1, sizeof(cl_int), &srcB);
  CLUtils::CheckError(err);
  err = clSetKernelArg(framework.GetKernel(0), 2, sizeof(cl_mem), &destC);
  CLUtils::CheckError(err);
  std::vector<size_t> globalWorkSize = { arrayDepth, arrayWidth, arrayHeight };
  framework.RunKernel(globalWorkSize,0);
  for(auto i = 0; i < arrayHeight*arrayHeight*arrayDepth; i++)
  {
    if(destC.hostPtr[i] != srcA.hostPtr[i] + srcB.hostPtr[i])
    {
      std::cout << "fail, dest is: " << destC.hostPtr[i] << std::endl; 
    }
  }
  framework.RunKernel(globalWorkSize, 0);
  
  srcA.~CLMemory();
  srcB.~CLMemory();
  destC.~CLMemory();


  return 0;
}

