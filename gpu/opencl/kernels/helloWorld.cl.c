struct TestStruct {
    int num;
};


__kernel void hello_kernel(__global const struct TestStruct *input,
                           __global struct TestStruct *output)
{
    int gid = get_global_id(0);
    output[gid].num = 2 * input[gid].num;
}
