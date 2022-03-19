//
// Created by lc on 2022/3/19.
//

#include "openCL/OpenCLProgramContext.hpp"
#include "openCL/OpenCLKernel.hpp"

int main() {
    auto start = std::chrono::system_clock::now();


    auto programContext = OpenCLProgramContext::createGPUContext();
    programContext.useDevice(0);
    programContext.useProgram(
        "/Users/liangchen/CLionProjects/crenderer/gpu/openCL/kernel/test.cl"
    );
    auto testKernel1 = programContext.createKernel("test1");
    auto testKernel2 = programContext.createKernel("test2");

    size_t size = 1920 * 1080 * 32;
    int* nums = new int[size];
    testKernel1.setWorkSize({ size }, { 1 });
//    testKernel1.setInputArguments(0, nums, 10);
    cl_mem& input = testKernel1.setArguments(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, size * sizeof(int), NULL, 0);
    int* mapPtr = programContext.mapWritingBuffer<int>(input, size);

    for (int i = 0; i < size; ++i) {
        mapPtr[i] = i;
    }

    auto event1 = programContext.execute(testKernel1);
    programContext.waitForEvent(event1);

    int* mapReadingPtr = programContext.mapReadingBuffer<int>(input, size);

    for (int i = 0; i < size; ++i) {
        nums[i] = mapReadingPtr[i];
    }

//    for (int i = 0; i < size; ++i) {
//        std::cout << nums[i] << std::endl;
//    }

    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";


    return 0;
}

