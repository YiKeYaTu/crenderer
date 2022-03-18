//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_OPENCLGLOBAL_H
#define CRENDERER_OPENCLGLOBAL_H

#ifdef __OPENCL_C_VERSION__

typedef int cl_int;
typedef float cl_float;
typedef float3 cl_float3;
typedef bool cl_bool;
typedef short cl_short;

#else

#include <OpenCL/cl.h>
#include <float.h>
#include "../../../object/ObjectTypes.hpp"

#endif

#define TRIANGLE 1
#define SPHERE 2
#define MESH_TRIANGLE 3
#define M_PI 3.14159274101257f

#endif //CRENDERER_OPENCLGLOBAL_H
