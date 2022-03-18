//
// Created by lc on 2022/3/17.
//

#ifndef CRENDERER_C_SAMPLEDDIR_H
#define CRENDERER_C_SAMPLEDDIR_H

#include "../OpenCLGlobal.h"

typedef struct {
    cl_float3 dir;
    cl_float pdf;
} C_SampledDir;

#ifdef __OPENCL_C_VERSION__

C_SampledDir initSampledDir(cl_float3* dir, float pdf) {
    C_SampledDir ret = {
            .dir = normalize(*dir),
            .pdf = pdf
    };
    return ret;
}

#endif

#endif //CRENDERER_C_SAMPLEDDIR_H
