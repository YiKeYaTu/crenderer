//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_BVH_H
#define OPENCL_C_BVH_H

#include "OpenCLGlobal.h"
#include "C_Ray.h"
#include "C_Bounds3.h"

typedef struct {
    C_Bounds3 bounds3;
    cl_int objectIndex;
    cl_int leftChildIndex;
    cl_int rightChildIndex;
    cl_int parentIndex;
} C_BVH;

#endif //OPENCL_C_BVH_H
