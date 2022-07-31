__kernel void Add(__global int* pA, __global int* pB, __global int* pC)
{
    const int z     = get_global_id(0);
    const int x     = get_global_id(1);
    const int y     = get_global_id(2);
    const int depth = get_global_size(0);
    const int width = get_global_size(1);

    const int id = y * width * depth + x * depth + z;

    pC[id] = pA[id] + pB[id];
}
