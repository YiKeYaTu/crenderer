//
// Created by lc on 2022/3/12.
//

#include <iostream>
#include <chrono>

#include "openCL/OpenCLProgramContext.hpp"
#include "openCL/OpenCLKernel.hpp"
#include "openCL/include/C_BVH.h"
#include "openCL/include/C_Bounds3.h"
#include "openCL/include/C_Ray.h"
#include "openCL/include/object/C_Object.h"
#include "openCL/include/C_Intersection.h"
#include "openCL/translation/bvh2cBvh.hpp"
#include "openCL/translation/ray2cRay.hpp"

#include "../object/ObjectTypes.hpp"
#include "../DefaultScene.hpp"
#include "../Ray.hpp"

int main() {
    std::shared_ptr<Scene> cornellVolumeBoxScene = DefaultScene::getCornellVolumeBox();
    auto sceneBvh = cornellVolumeBoxScene->bvh();
    auto sceneVolumeBvh = cornellVolumeBoxScene->volumeBvh();

    auto [ cBvhs, objects ] = bvh2cBvh(sceneBvh.get());
    int numBvhNodes = cBvhs.size();
    int numObjects = objects.size();

    Ray ray({ 50, 50, -800 }, { 0.1, 0.2, 1 });

    C_Ray cRay = ray2cRay(ray);
    size_t size = 1920 * 1080;
    Intersection interCPU = cornellVolumeBoxScene->intersect(ray);
    std::vector<C_Ray> cRays(size);
    std::vector<C_Intersection> interGPUs(size);

    for (int i = 0; i < size; ++i) {
        cRays[i] = cRay;
    }

    cl_uint numPlatforms = OpenCLProgramContext::getNumPlatforms();
    std::vector<cl_platform_id> platformIDs = OpenCLProgramContext::getPlatformIDs(numPlatforms);
    OpenCLProgramContext openClProgramContext = OpenCLProgramContext::createGPUContext(platformIDs[0]);

    openClProgramContext.useDevice({0});
    openClProgramContext.useProgram(
            "/Users/liangchen/CLionProjects/crenderer/gpu/openCL/kernel/trace.cl",
            "-I /Users/liangchen/CLionProjects/crenderer/gpu/openCL/ -D __OPENCL_C_VERSION__=\"120\""
    );

    OpenCLKernel kernel1 = openClProgramContext.createKernel("castRay");
    kernel1.setWorkSize({ size }, { 1 });

    kernel1.setInputArguments(0, &cRays[0], cRays.size());
    kernel1.setInputArguments(1, &cBvhs[0], cBvhs.size());
    kernel1.setInputArguments(2, &numBvhNodes, 1);
    kernel1.setInputArguments(3, &objects[0], objects.size());
    kernel1.setInputArguments(4, &numObjects, 1);

    kernel1.setOutputArguments(5, &interGPUs[0], interGPUs.size());

    auto tStart = std::chrono::high_resolution_clock::now();
    openClProgramContext.execute(kernel1);
    auto tEnd = std::chrono::high_resolution_clock::now();
    double elapsedTimeMs = std::chrono::duration<double, std::milli>(tEnd - tStart).count();

    std::cout << elapsedTimeMs << std::endl;

    return 0;
}