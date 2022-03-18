//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_RAY_H
#define OPENCL_C_RAY_H

#include "OpenCLGlobal.h"

typedef struct {
    cl_float scale;
    cl_float3 origin;
    cl_float3 direction;
    cl_float3 recDirection;
} C_Ray;

#endif //OPENCL_C_RAY_H
