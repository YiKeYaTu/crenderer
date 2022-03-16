//
// Created by lc on 2022/3/13.
//

#ifndef OPENCL_C_INTERSECTION_H
#define OPENCL_C_INTERSECTION_H

#include "C_Object.h"
#include "C_Ray.h"
#include "C_BVH.h"

typedef struct {
    cl_bool happened;
    cl_float tMin;
    cl_float tMax;
    cl_float distance;
    cl_float3 hitPoint;
    cl_int intersectedObjectIndex;
} C_Intersection;

const C_Intersection getEmptyIntersection() {
    const C_Intersection emptyIntersection = {
            .happened = 0,
            .tMin = FLT_MAX,
            .tMax = FLT_MAX,
            .intersectedObjectIndex = 0
    };
    return emptyIntersection;
}

C_Intersection initIntersection(
        cl_float tMin,
        cl_float tMax,
        cl_float distance,
        cl_float3* hitPoint,
        cl_int intersectedObjectIndex
) {
    C_Intersection intersection = {
            .happened = 1,
            .tMin = tMin,
            .tMax = tMax,
            .distance = distance,
            .hitPoint = *hitPoint,
            .intersectedObjectIndex = intersectedObjectIndex
    };
    return intersection;
}

#endif //OPENCL_C_INTERSECTION_H
