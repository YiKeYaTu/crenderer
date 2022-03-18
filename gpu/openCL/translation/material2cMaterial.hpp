//
// Created by lc on 2022/3/17.
//

#ifndef CRENDERER_MATERIAL2CMATERIAL_HPP
#define CRENDERER_MATERIAL2CMATERIAL_HPP

#include "../include/material/C_Material.h"
#include "../../../material/CommonMaterial.hpp"
#include "vec32float3.hpp"

C_Material material2cMaterial(const CommonMaterial* commonMaterial) {
    C_Material cMaterial = {
            .kd = vec32float3(commonMaterial->kd()),
            .ks = vec32float3(commonMaterial->ks()),
            .emission = vec32float3(commonMaterial->emission()),
            .materialType = commonMaterial->materialType(),
            .propertyType = commonMaterial->propertyType(),
    };
    return cMaterial;
}

#endif //CRENDERER_MATERIAL2CMATERIAL_HPP
