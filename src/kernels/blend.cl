/**
* alpha * inputImageA + (1-alpha) *inputImageB
*/
__kernel void Blend(read_only image2d_t inputImageA, read_only image2d_t inputImageB,
                     write_only image2d_t outputImage)
{
  const int width = get_image_width(inputImageA);
  const int height = get_image_height(inputImageB);
  const int2 coord = {get_global_id(0), get_global_id(1)};

  uint4 pixelsA = read_imageui(inputImageA, coord);
  uint4 pixelsB = read_imageui(inputImageB, coord);
  write_imageui(outputImage, coord, ((pixelsA / 2) + (pixelsB / 2)));
}
