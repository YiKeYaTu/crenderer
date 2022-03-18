#ifndef OPENCL_OBJECT_CL
#define OPENCL_OBJECT_CL

#include "include/OpenCLGlobal.h"
#include "include/object/C_Object.h"

float3 computeTriangleNormal(global C_Object* object, float3* pos) {
    return normalize(object->normal);
}
float3 computeSphereNormal(global C_Object* object, float3* pos) {
    return normalize(*pos - object->sphere.centroid);
}
float3 computeMeshTriangleNormal(global C_Object* object, float3* pos) {
    return normalize(object->normal);
}

float3 computeNormal(global C_Object* object, float3* pos) {
    switch (object->subType) {
        case TRIANGLE:
            return computeTriangleNormal(object, pos);
        case SPHERE:
            return computeSphereNormal(object, pos);
        case MESH_TRIANGLE:
            return computeMeshTriangleNormal(object, pos);
    }
}

#endif //OPENCL_OBJECT_CL
