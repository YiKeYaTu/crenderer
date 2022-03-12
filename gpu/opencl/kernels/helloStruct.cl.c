//struct TestStruct {
//    int num;
//};
#include "TestStruct.hpp"


__kernel void hello_struct(__global const struct TestStruct *input,
                           __global struct TestStruct *output)
{
    int gid = get_global_id(0);
    output[gid].num = input[gid].num + 111;
}
