#include "opencl_framework.hpp"


int main()
{
  CLFramework framework;
  framework.QueryPlatforms();
  const std::string platformName = "Intel(R) OpenCL HD Graphics";
  framework.ChoosePlatform(platformName, DeviceType::PLATFROM_GPU);
  framework.CreateContext();
  framework.BuildKernel("kernels/add.cl", 3);
  
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
  
  framework.SetKernelBufferArg<cl_int>(0, srcA);
  framework.SetKernelBufferArg<cl_int>(1, srcB);
  framework.SetKernelBufferArg<cl_int>(2, destC);
  std::vector<size_t> globalWorkSize = { arrayDepth, arrayWidth, arrayHeight };
  framework.RunKernel(globalWorkSize);
  cl_int err;
  for(auto i = 0; i < arrayHeight*arrayHeight*arrayDepth; i++)
  {
    if(destC.hostPtr[i] != srcA.hostPtr[i] + srcB.hostPtr[i])
    {
      std::cout << "fail, dest is: " << destC.hostPtr[i] << std::endl; 
    }
  }
  std::cout << "herllo";
  srcA.~CLMemory();
  srcB.~CLMemory();
  destC.~CLMemory();

}