//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_VEC32FLOAT3_HPP
#define CRENDERER_VEC32FLOAT3_HPP

#include "../include/OpenCLGlobal.h"
#include "../../../Vector.hpp"

cl_float3 vec32float3(const Vec3f& vec) {
    cl_float3 float3 = {
        static_cast<float>(vec.x()),
        static_cast<float>(vec.y()),
        static_cast<float>(vec.z())
    };
    return float3;
}

#endif //CRENDERER_VEC32FLOAT3_HPP
