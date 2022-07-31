#include "opencl_framework.hpp"


int main()
{
  CLFramework framework;
  framework.QueryPlatforms();
  const std::string platformName = "Intel(R) OpenCL HD Graphics";
  framework.ChoosePlatform(platformName, DeviceType::PLATFROM_GPU);
  framework.CreateContext();
  framework.BuildKernel("kernels/add.cl", 3);
  
  /* read the image into a cl_mem 2d image */
  std::vector<uint8_t> imageData;
  constexpr int imageWidth = 512;
  constexpr int imageHeight = 400;
  const cl_image_format imgFormat = {CL_RGB, CL_UNSIGNED_INT8};
  cl_mem im = clCreateImage2D(framework.GetContext(), CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, &imgFormat, imageWidth, imageHeight, imageWidth * imageHeight, reinterpret_cast<void*>(imageData.data()));
  /* working with arrays */
  const size_t arrayWidth = 1024;
  const size_t  arrayHeight = 1024;
  OpenclMemory<cl_int> srcA(framework.GetContext(),CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight);
  OpenclMemory<cl_int> srcB(framework.GetContext(),CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight);
  OpenclMemory<cl_int> destC(framework.GetContext(),CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, arrayWidth * arrayHeight);
  /* generate here some inputs and oututs */
  int valA = 1;
  int valB = 10;
  for(auto i = 0; i < arrayHeight*arrayWidth; i++)
  {
    srcA.hostPtr[i] = valA;
    srcB.hostPtr[i] = valB;
  }  
  
  framework.SetKernelBufferArg<cl_int>(0, srcA);
  framework.SetKernelBufferArg<cl_int>(1, srcB);
  framework.SetKernelBufferArg<cl_int>(2, destC);
  std::vector<size_t> globalWorkSize = { arrayWidth, arrayHeight };
  framework.RunKernel(globalWorkSize);
  cl_int err;
  for(auto i = 0; i < arrayHeight*arrayHeight; i++)
  {
    if(destC.hostPtr[i] != srcA.hostPtr[i] + srcB.hostPtr[i])
    {
      std::cout << "verification failed" << std::endl; 
    }
  }
  std::cout << "herllo";
  srcA.~OpenclMemory();
  srcB.~OpenclMemory();
  destC.~OpenclMemory();

}