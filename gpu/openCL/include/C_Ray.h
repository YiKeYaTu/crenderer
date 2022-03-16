//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_RAY_H
#define OPENCL_C_RAY_H

#include "OpenCLGlobal.h"

typedef struct {
    const cl_float scale;
    const cl_float3 origin;
    const cl_float3 direction;
    const cl_float3 recDirection;
} C_Ray;

#endif //OPENCL_C_RAY_H
