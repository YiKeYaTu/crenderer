//
// Created by lc on 2022/3/12.
//

#include <iostream>
#include "openCL/OpenCLProgramContext.hpp"
#include "openCL/OpenCLKernel.hpp"
#include "openCL/include/C_BVH.h"
#include "openCL/include/C_Bounds3.h"
#include "openCL/include/C_Ray.h"
#include "openCL/include/C_Object.h"
#include "openCL/include/C_Intersection.h"
#include "openCL/translation/bvh2cBvh.hpp"
#include "openCL/translation/ray2cRay.hpp"

#include "../object/ObjectTypes.hpp"
#include "../DefaultScene.hpp"
#include "../Ray.hpp"

int main() {
    std::shared_ptr<Scene> cornellVolumeBoxScene = DefaultScene::getCornellVolumeBox();
    auto sceneBvh = cornellVolumeBoxScene->bvh();

    auto [ cBvhs, objects ] = bvh2cBvh(sceneBvh.get());
    int numBvhNodes = cBvhs.size();
    int numObjects = objects.size();

    Ray ray({ 50, 50, -800 }, { 0, 0, 1 });

    C_Ray cRay = ray2cRay(ray);

    auto interCPU = cornellVolumeBoxScene->intersect(ray);

    std::cout << "min x: " << cBvhs[0].bounds3.min.x
              << "min y: " << cBvhs[0].bounds3.min.y
              << "min z: " << cBvhs[0].bounds3.min.z << std::endl;
    std::cout << "max x: " << cBvhs[0].bounds3.max.x
              << "max y: " << cBvhs[0].bounds3.max.y
              << "max z: " << cBvhs[0].bounds3.max.z << std::endl;

    cl_uint numPlatforms = OpenCLProgramContext::getNumPlatforms();
    std::vector<cl_platform_id> platformIDs = OpenCLProgramContext::getPlatformIDs(numPlatforms);
    OpenCLProgramContext openClProgramContext = OpenCLProgramContext::createGPUContext(platformIDs[0]);

    openClProgramContext.useDevice({0});
    openClProgramContext.useProgram(
            "/Users/liangchen/CLionProjects/crenderer/gpu/openCL/kernel/trace.cl",
            "-I /Users/liangchen/CLionProjects/crenderer/gpu/openCL/include -D __OPENCL_C_VERSION__=\"120\""
    );

    OpenCLKernel kernel1 = openClProgramContext.createKernel("castRay");
    kernel1.setWorkSize({ 1 }, { 1 });

    C_Intersection interGPU;

    kernel1.setInputArguments(0, &cRay, 1);
    kernel1.setInputArguments(1, &cBvhs[0], numBvhNodes);
    kernel1.setInputArguments(2, &numBvhNodes, 1);
    kernel1.setInputArguments(3, &objects[0], numObjects);
    kernel1.setInputArguments(4, &numObjects, numObjects);
    kernel1.setOutputArguments(5, &interGPU, 1);

    openClProgramContext.execute(kernel1);

    return 0;
}