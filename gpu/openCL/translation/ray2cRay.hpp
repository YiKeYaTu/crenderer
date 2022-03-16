//
// Created by lc on 2022/3/16.
//

#ifndef CRENDERER_RAY2CRAY_HPP
#define CRENDERER_RAY2CRAY_HPP

#include "../include/C_Ray.h"
#include "../../../Ray.hpp"
#include "vec32float3.hpp"

C_Ray ray2cRay(Ray& ray) {
    C_Ray cRay = {
        .recDirection = vec32float3(ray.recDirection),
        .origin = vec32float3(ray.origin),
        .direction = vec32float3(ray.direction),
        .scale = 1.0
    };

    if (cRay.direction.x == 0.0) {
        const_cast<cl_float3&>(cRay.recDirection).x = FLT_MAX;
    }

    if (cRay.direction.y == 0.0) {
        const_cast<cl_float3&>(cRay.recDirection).y = FLT_MAX;
    }

    if (cRay.direction.z == 0.0) {
        const_cast<cl_float3&>(cRay.recDirection).z = FLT_MAX;
    }

    return cRay;
}

#endif //CRENDERER_RAY2CRAY_HPP
