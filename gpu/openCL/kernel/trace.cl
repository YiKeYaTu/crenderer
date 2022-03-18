#ifndef OPENCL_TRACE_CL
#define OPENCL_TRACE_CL

#include "include/C_Bounds3.h"
#include "include/C_BVH.h"
#include "include/C_Intersection.h"
#include "include/object/C_Object.h"
#include "include/C_Ray.h"
#include "kernel/intersect.cl"
#include "kernel/random.cl"
#include "kernel/shade.cl"

kernel void generateMultipleRays(
    global C_Ray* emptyRays,
    global float3* origin,
    global size_t* width,
    global size_t* height,
    global float* aspectRatio,
    global float* scale
){
    size_t gid = get_global_id(0);

    size_t j = gid / *width;
    size_t i = gid % *width;
    
    float x = (2.0f * (i + 0.5f) / (float)(*width) - 1.0f) * (*aspectRatio) * (*scale);
    float y = (1.0f - 2.0f * (j + 0.5f) / (float)(*height)) * (*scale);
    
    
    float3 dir = (float3) (-x, y, 1);
    float3 localOrigin = *origin;
    
    C_Ray ray = initCray(&localOrigin, &dir);
    
    *(emptyRays + gid) = ray;
}

kernel void trace(
//    global uint* offset,
    global C_Ray* ray,
                  
    global C_BVH* bvhNodes,
    global C_BVH* volumeBvhNodes,
                  
    global C_Object* objects,
    global C_Object* volumeObjects,
    global C_Object* lightObjects,
                  
    global uint* numObjects,
    global uint* numVolumeObjects,
    global uint* numLightObjects,
                  
    global float3* frameBuffer
) {
    size_t globalGid = get_global_id(0);
    size_t localGid = get_local_id(0);
    size_t globalSize = get_global_size(0);
    size_t localSize = get_local_size(0);
    
//    size_t idx = (globalGid + *offset) / localSize;
//    size_t rayIdx = (globalGid + *offset) / localSize;
    size_t idx = globalGid / localSize;
    
    local float3 pixelColors[HOST_SPP];
    
    pixelColors[localGid] = shade(
        &ray[idx],

        bvhNodes,
        volumeBvhNodes,

        objects,
        volumeObjects,
        lightObjects,

        numObjects,
        numVolumeObjects,
        numLightObjects,

        globalGid);
//    pixelColors[localGid] = (float3) (.5f, .5f, .5f);
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if (localGid == 0) {
        float3 pixelColor = (float3) (0.f, 0.f, 0.f);
        for (int i = 0; i < HOST_SPP; i ++) {
            pixelColor += pixelColors[i];
        }
        pixelColor /= HOST_SPP;
        frameBuffer[idx] = pixelColor;
    }
}

#endif
