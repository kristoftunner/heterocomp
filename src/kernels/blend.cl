/** This is a basic implementation using global work size and host memory 
 */
__kernel void Blend(__global uint8_t* pA, __global uint8_t* pB, __global uint8_t* pC)
{
  const int x     = get_global_id(0);
  const int y     = get_global_id(1);
  const int width = get_global_size(0);
}