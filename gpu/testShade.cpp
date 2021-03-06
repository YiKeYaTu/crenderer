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
#include "openCL/OpenCLRenderer.hpp"
#include <vector>

int main() {
    auto start = std::chrono::system_clock::now();
    std::shared_ptr<Scene> cornellVolumeBoxScene = DefaultScene::getCornellVolumeBox();
    OpenCLRenderer openClRenderer(*cornellVolumeBoxScene, "../gpu/openCL/kernel/trace.cl", 256);
    openClRenderer.render(100 * 100);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";


    return 0;
}