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
  framework.BuildKernel("../src/kernels/blend.cl", 6, "Blend");

  /* read the image into a cl_mem 2d image */
  std::vector<uint8_t> astroImage = ReadFileIntoMemory("../junk/astro.bin");
  std::vector<uint8_t> pigeonImage = ReadFileIntoMemory("../junk/pigeon.bin");
  constexpr size_t imageWidth = 1024;
  constexpr size_t imageHeight = 800;
  constexpr size_t imageDepth = 3;
  const cl_image_format imgFormat = {CL_RGB, CL_UNSIGNED_INT8};
  const cl_image_desc desc = { 
    CL_MEM_OBJECT_IMAGE2D,
    imageWidth, 
    imageHeight,
    0,
    0,
    0,
    0,
    0,
    0
  };
  
  cl_int err;
  cl_mem astroImage2D = clCreateImage2D(framework.GetContext(), CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, &imgFormat, imageWidth, imageHeight, imageWidth * imageHeight*imageDepth, reinterpret_cast<void*>(astroImage.data()), &err);
  CLUtils::CheckError(err);
  cl_mem pigeonImage2D = clCreateImage2D(framework.GetContext(), CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, &imgFormat, imageWidth, imageHeight, imageWidth * imageHeight*imageDepth, reinterpret_cast<void*>(pigeonImage.data()), &err);
  CLUtils::CheckError(err);
  size_t optimizedSize = ((sizeof(uint8_t) * imageWidth*imageHeight*imageDepth - 1) / 64 + 1) * 64;
  uint8_t* destPtr = reinterpret_cast<uint8_t*>(EnvUtils::AllocAligned(optimizedSize, 4096));
  
  cl_mem destImage = clCreateImage2D(framework.GetContext(), CL_MEM_WRITE_ONLY | CL_MEM_HOST_WRITE_ONLY, &imgFormat, imageWidth, imageHeight, imageWidth * imageHeight*imageDepth, &destPtr, &err);
  CLUtils::CheckError(err);

  /* Arguments for Blend kernel */
  err = clSetKernelArg(framework.GetKernel(0), 2, sizeof(cl_mem), &astroImage2D);
  CLUtils::CheckError(err);
  err = clSetKernelArg(framework.GetKernel(0), 3, sizeof(cl_mem), &pigeonImage2D);
  CLUtils::CheckError(err);
  err = clSetKernelArg(framework.GetKernel(0), 4, sizeof(cl_mem), &destImage);
  CLUtils::CheckError(err);
  std::vector<size_t> globalWorkSize = {imageDepth, imageWidth, imageHeight};
  framework.RunKernel(globalWorkSize, 1);
  for(auto i = 0; i < imageDepth*imageWidth*imageHeight; i++)
  {
    if(destPtr[i] != astroImage[i]/2 + pigeonImage[i]/2)
    {
      std::cout << "fail, dest is: " << destPtr[i] << std::endl; 
    }
  }
  
  return 0;
}