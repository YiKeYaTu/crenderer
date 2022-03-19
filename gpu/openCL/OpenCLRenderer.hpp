//
// Created by lc on 2022/3/17.
//

#ifndef CRENDERER_OPENCLRENDERER_HPP
#define CRENDERER_OPENCLRENDERER_HPP

#include <vector>

#include "../../Scene.hpp"
#include "include/object/C_Object.h"
#include "include/C_BVH.h"
#include "translation/object2cObject.hpp"

class OpenCLRenderer {
    std::vector<C_Object> objects_;
    std::vector<C_Object> lights_;
    std::vector<C_Object> volumes_;

    u_int width_;
    u_int height_;
    cl_float fov_;

    std::vector<cl_float3> frameBuffer_;
    cl_float3 cameraPos_;

    std::vector<C_BVH> bvh_;
    std::vector<C_BVH> volumeBvh_;
    cl_uint spp_ = 32;

public:
    OpenCLRenderer(Scene& scene)
        : width_(scene.width()), height_(scene.height()), fov_(scene.fov()) {
        Vec3f& cameraPos = scene.cameraPos();

        cameraPos_.x = cameraPos.x();
        cameraPos_.y = cameraPos.y();
        cameraPos_.z = cameraPos.z();

        auto sceneBvh = scene.bvh();
        auto sceneVolumeBvh = scene.volumeBvh();

        auto lightObjects = scene.lights();

        for (const auto& item : lightObjects) {
            lights_.push_back(object2cObject(item));
        }

        auto [ cBvhs, cObjects ] = bvh2cBvh(sceneBvh.get());
        auto [ cVolumeBvhs, cVolumeObjects ] = bvh2cBvh(sceneVolumeBvh.get());

        bvh_ = cBvhs;
        volumeBvh_ = cVolumeBvhs;
        objects_ = cObjects;
        volumes_ = cVolumeObjects;

        frameBuffer_.resize(width_ * height_);
    }

    void render() {
        OpenCLProgramContext openClProgramContext = createOpenCLProgramContext_();
        OpenCLKernel rayGenKernel = openClProgramContext.createKernel("generateMultipleRays");

        cl_float aspectRatio = static_cast<cl_float>(width_) / height_;
        cl_float scale = std::tan((fov_ / 2) / 180 * util::MY_PI);
        int numSplit = 10;

        C_Ray* rays = new C_Ray[width_ * height_];
        rayGenKernel.setWorkSize( { width_ * height_ }, { 1 } );
        rayGenKernel.setInterArguments(0, rays, width_ * height_);
        rayGenKernel.setInputArguments(1, &cameraPos_, 1);
        rayGenKernel.setInputArguments(2, &width_, 1);
        rayGenKernel.setInputArguments(3, &height_, 1);
        rayGenKernel.setInputArguments(4, &aspectRatio, 1);
        rayGenKernel.setInputArguments(5, &scale, 1);

        openClProgramContext.execute(rayGenKernel);

        OpenCLKernel rayTraceKernel = openClProgramContext.createKernel("trace");

        cl_float3* frameBuffer = new cl_float3[width_ * height_];
//        char* heapBuffer = (char*) malloc(width_ * height_ * spp_ * bvh_.size() * sizeof(int));

        size_t numObjects = objects_.size();
        size_t numVolumeObjects = volumes_.size();
        size_t numLightObjects = lights_.size();

        rayTraceKernel.setWorkSize( { width_ * height_ * spp_ }, { spp_ } );

        rayTraceKernel.setInterArguments(0, rays, width_ * height_);

        rayTraceKernel.setInputArguments(1, &bvh_[0], bvh_.size());
        rayTraceKernel.setInputArguments(2, &volumeBvh_[0], volumeBvh_.size());

        rayTraceKernel.setInputArguments(3, &objects_[0], objects_.size());
        rayTraceKernel.setInputArguments(4, &volumes_[0], volumes_.size());
        rayTraceKernel.setInputArguments(5, &lights_[0], lights_.size());

        rayTraceKernel.setInputArguments(6, &numObjects, 1);
        rayTraceKernel.setInputArguments(7, &numVolumeObjects, 1);
        rayTraceKernel.setInputArguments(8, &numLightObjects, 1);

        rayTraceKernel.setOutputArguments(9, frameBuffer, width_ * height_);
//        rayTraceKernel.setInputArguments(10, heapBuffer, width_ * height_ * spp_ * bvh_.size() * sizeof(int));

        openClProgramContext.execute(rayTraceKernel);

        saveFrameBufferAsPPM(frameBuffer);

        delete[] frameBuffer;
        delete[] rays;
    }

private:
    void saveFrameBufferAsPPM(cl_float3* frameBuffer) {
        FILE* fp = fopen("test_gpu.ppm", "wb");
        (void)fprintf(fp, "P6\n%d %d\n255\n", width_, height_);
        for (auto i = 0; i < height_ * width_; ++i) {
            static unsigned char color[3];
            color[0] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].x), 0.6f));
            color[1] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].y), 0.6f));
            color[2] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].z), 0.6f));
            fwrite(color, 1, 3, fp);
        }
        fclose(fp);
    }
    OpenCLProgramContext createOpenCLProgramContext_() {
        cl_uint numPlatforms = OpenCLProgramContext::getNumPlatforms();
        std::vector<cl_platform_id> platformIDs = OpenCLProgramContext::getPlatformIDs(numPlatforms);
        OpenCLProgramContext openClProgramContext = OpenCLProgramContext::createGPUContext(platformIDs[0]);

        std::string optionStr;
        optionStr.append("-I /Users/liangchen/CLionProjects/crenderer/gpu/openCL/ -D HOST_SPP=");
        optionStr.append(std::to_string(spp_));
        optionStr.append(" -D __OPENCL_C_VERSION__=\"120\"");
        optionStr.append(" -D START_RUSSIA_DEPTH=1");
        optionStr.append(" -D MAX_DEPTH=5");

        openClProgramContext.useDevice({0});
        openClProgramContext.useProgram(
                "/Users/liangchen/CLionProjects/crenderer/gpu/openCL/kernel/trace.cl",
                optionStr.c_str()
        );
        return openClProgramContext;
    }

};

#endif //CRENDERER_OPENCLRENDERER_HPP
