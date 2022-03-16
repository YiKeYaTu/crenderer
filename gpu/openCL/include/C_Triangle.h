//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_TRIANGLE_H
#define OPENCL_C_TRIANGLE_H

#include "OpenCLGlobal.h"

typedef struct {
    cl_float3 v0, v1, v2;
    cl_float3 e0, e1, e2;
    cl_float3 n;
} C_Triangle;

#endif //OPENCL_C_TRIANGLE_H
