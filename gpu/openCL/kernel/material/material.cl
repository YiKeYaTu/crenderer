#include "../../include/OpenCLGlobal.h"
#include "../../include/object/C_Object.h"

#define theta 20.0f / 180 * M_PI

cl_float3 getZeroEmission() {
    cl_float3 ret = (cl_float3) ( 0.f, 0.f, 0.f );
    return ret;
}

cl_float3 getCommonLightEmission(global C_Material* material, cl_float3* pointPos, cl_float3* lightPos, cl_float3* normal,
                                 global C_Object* light) {
    if (dot(*pointPos - *lightPos, *normal) < 0) {
        return getZeroEmission();
    }
    return material->emission;
}

cl_float3 getSpotLightEmission(global C_Material* material, cl_float3* pointPos, cl_float3* lightPos, cl_float3* normal,
                               global C_Object* light) {
    float a = normal->x, b = normal->y, c = normal->z;
    float x = pointPos->x, y = pointPos->y, z = pointPos->z;
    float x0 = light->centroid.x, y0 = light->centroid.y, z0 = light->centroid.z;

    float t = ( ( a*x0 + b*y0 + c*z0 ) - ( a*x + b*y + c*z )  )
               / ( length(*normal) * length(*normal) );

    float x1 = x + a*t, y1 = y + b*t , z1 = z + c*t;

    C_Bounds3 bounds3 = light->bounds3;

    // The projected point is on the surface
    if (x1 >= bounds3.min.x &&
        y1 >= bounds3.min.y &&
        z1 >= bounds3.min.z &&
        x1 <= bounds3.max.x &&
        y1 <= bounds3.max.y &&
        z1 <= bounds3.max.z
    ) {
        return material->emission;
    }

    x1 = clamp(x1, bounds3.min.x, bounds3.max.x);
    y1 = clamp(y1, bounds3.min.y, bounds3.max.y);
    z1 = clamp(z1, bounds3.min.z, bounds3.max.z);

    float3 nearestPointOnSurface = (float3) ( x1, y1, z1 );
    float cos1 = dot(*pointPos - nearestPointOnSurface, *normal) / length(*pointPos - nearestPointOnSurface) / length(*normal);

    if (cos1 >= cos(theta)) {
        return material->emission;
    }

    return material->emission * pow(
        max(
            dot(*pointPos - nearestPointOnSurface, *normal) / length(*pointPos - nearestPointOnSurface) / length(*normal),
            0.f
        ),
        100.f
    );
}

cl_float3 getEmission(global C_Material* material, cl_float3* pointPos, cl_float3* lightPos, cl_float3* normal, global C_Object* light) {
    switch (material->materialType) {
        case COMMON:
            return getZeroEmission();
        case COMMON_LIGHT:
            return getCommonLightEmission(material, pointPos, lightPos, normal, light);
        case SPOT_LIGHT:
            return getSpotLightEmission(material, pointPos, lightPos, normal, light);
    }
}

cl_bool hasEmission(global C_Material* material) {
    switch (material->materialType) {
        case COMMON_LIGHT:
        case SPOT_LIGHT:
            return 1;
    }
    return 0;
}
