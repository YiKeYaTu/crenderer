//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_OBJECT_H
#define OPENCL_C_OBJECT_H

#include "OpenCLGlobal.h"
#include "C_Bounds3.h"
#include "C_Triangle.h"
#include "C_Sphere.h"

typedef struct {
    const cl_int subType;
    union {
        C_Triangle triangle;
        C_Sphere sphere;
    };
    C_Bounds3 bounds3;
    cl_float3 centroid;
    cl_float3 normal;
} C_Object;

#endif //OPENCL_C_OBJECT_H
