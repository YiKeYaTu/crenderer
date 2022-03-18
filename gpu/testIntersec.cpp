//
// Created by lc on 2022/3/16.
//

#include "openCL/OpenCLProgramContext.hpp"
#include "openCL/OpenCLKernel.hpp"
#include "openCL/include/C_Ray.h"
#include "openCL/include/C_Intersection.h"
#include "openCL/translation/bvh2cBvh.hpp"
#include "openCL/translation/ray2cRay.hpp"
#include "../DefaultScene.hpp"

OpenCLProgramContext createOpenCLProgramContext() {
    cl_uint numPlatforms = OpenCLProgramContext::getNumPlatforms();
    std::vector<cl_platform_id> platformIDs = OpenCLProgramContext::getPlatformIDs(numPlatforms);
    OpenCLProgramContext openClProgramContext = OpenCLProgramContext::createGPUContext(platformIDs[0]);

    openClProgramContext.useDevice({0});
    openClProgramContext.useProgram(
            "/Users/liangchen/CLionProjects/crenderer/gpu/openCL/kernel/trace.cl",
            "-I /Users/liangchen/CLionProjects/crenderer/gpu/openCL/ -D __OPENCL_C_VERSION__=\"120\""
    );

    return openClProgramContext;
}

int main() {
    Ray ray({ 50, 50, -800 }, { 0.1, 0.2, 1 });
    C_Ray cRay = ray2cRay(ray);

    std::shared_ptr<Scene> cornellVolumeBoxScene = DefaultScene::getCornellVolumeBox();
    auto sceneVolumeBvh = cornellVolumeBoxScene->volumeBvh();
    auto [ cBvhs, cObjects ] = bvh2cBvh(sceneVolumeBvh.get());

    OpenCLProgramContext openClProgramContext = createOpenCLProgramContext();
    OpenCLKernel kernel1 = openClProgramContext.createKernel("testIntersect");

    auto interVolumeCPU = cornellVolumeBoxScene->intersectVolume(ray);
    C_Intersection interVolumeGPU;

    kernel1.setInputArguments(0, &cRay, 1);
    kernel1.setInputArguments(1, &cBvhs[0], cBvhs.size());
    kernel1.setInputArguments(2, &cObjects[0], cObjects.size());
    kernel1.setOutputArguments(3, &interVolumeGPU, 1);

    openClProgramContext.execute(kernel1);

    return 0;
}