//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_C_SPHERE_H
#define CRENDERER_C_SPHERE_H

#include "OpenCLGlobal.h"

typedef struct {
    cl_float3 center;
    cl_float radius;
    cl_float3 centroid;
    cl_float3 bounds3;
} C_Sphere;

#endif //CRENDERER_C_SPHERE_H
