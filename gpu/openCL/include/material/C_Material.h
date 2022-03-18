//
// Created by lc on 2022/3/16.
//

#ifndef CRENDERER_C_MATERIAL_H
#define CRENDERER_C_MATERIAL_H

#include "../OpenCLGlobal.h"
#include "../object/C_Object.h"
#include "../../../../material/MaterialType.hpp"

typedef struct {
    cl_float3 emission;
    cl_float3 kd;
    cl_float3 ks;
    MaterialType materialType;
    PropertyType propertyType;
} C_Material;


#endif //CRENDERER_C_MATERIAL_H
