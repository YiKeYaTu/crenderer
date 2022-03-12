//
// Created by lc on 2022/3/12.
//

#include <iostream>
#include "./opencl/OpenCLProgramContext.hpp"
#include "./opencl/kernels/TestStruct.hpp"

int main() {
    cl_uint numPlatforms = OpenCLProgramContext::getNumPlatforms();
    std::vector<cl_platform_id> platformIDs = OpenCLProgramContext::getPlatformIDs(numPlatforms);
    OpenCLProgramContext openClProgramContext = OpenCLProgramContext::createGPUContext(platformIDs[0]);

    openClProgramContext.useDevice({0});
    openClProgramContext.useKernel("/Users/liangchen/CLionProjects/crenderer/gpu/opencl/kernels/helloStruct.cl.c",
                                   "hello_struct");

    struct TestStruct input[2];
    input[0].num = 10;
    input[1].num = 100;

    struct TestStruct output[2];

    openClProgramContext.setKernelArguments(0, input, 2);
    openClProgramContext.setReturnValues(1, output, 2);

    size_t globalWorkSize[1] = { 2 };
    size_t localWorkSize[1]  = { 1 };

    openClProgramContext.setWorkSize(globalWorkSize, localWorkSize);
    openClProgramContext.execute();

    for (int i = 0; i < 2; ++i) {
        std::cout << output[i].num << std::endl;
    }


//    openClProgramContext.useKernel("/Users/liangchen/CLionProjects/crenderer/gpu/opencl/kernels/helloWorld.cl.c", "hello_kernel");

//    int a[2][2] = {
//        { 1, 3 },
//        { 1, 3 }
//    };
//    int b[2][2] = {
//        { 2, 4 },
//        { 3, 3 }
//    };
//    int c[2][2];
//
//    openClProgramContext.setKernelArguments(0, a, 4);
//    openClProgramContext.setKernelArguments(1, b, 4);
//    openClProgramContext.setReturnValues(2, c, 4);
//
//    size_t globalWorkSize[1] = { 4 };
//    size_t localWorkSize[1]  = { 1 };

//    openClProgramContext.setWorkSize(globalWorkSize, localWorkSize);
//    openClProgramContext.execute();
//
//    for (int i = 0; i < 2; ++i) {
//        for (int j = 0; j < 2; ++j) {
//            std::cout << c[i][j] << std::endl;
//        }
//    }


    return 0;
}