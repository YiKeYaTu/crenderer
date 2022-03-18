//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_SPHERE2CSPHERE_HPP
#define CRENDERER_SPHERE2CSPHERE_HPP

#include "../include/object/C_Sphere.h"
#include "../../../object/Sphere.hpp"
#include "vec32float3.hpp"
#include "bounds32cBounds3.hpp"

C_Sphere sphere2cSphere(Sphere* sphere) {
    C_Sphere cSphere = {
            .radius = static_cast<cl_float>(sphere->radius),
            .center = vec32float3(sphere->center),
            .bounds3 = bounds32cBounds3(sphere->bounds3()),
            .centroid = vec32float3(sphere->centroid())
    };
    return cSphere;
}

#endif //CRENDERER_SPHERE2CSPHERE_HPP
