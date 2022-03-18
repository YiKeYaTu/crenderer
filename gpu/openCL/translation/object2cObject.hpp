//
// Created by lc on 2022/3/17.
//

#ifndef CRENDERER_OBJECT2COBJECT_HPP
#define CRENDERER_OBJECT2COBJECT_HPP

#include "../include/object/C_Object.h"
#include "../../../object/Object.hpp"
#include "bounds32cBounds3.hpp"
#include "vec32float3.hpp"
#include "material2cMaterial.hpp"
#include "sphere2cSphere.hpp"
#include "triangle2cTriangle.hpp"

C_Object object2cObject(Object* object) {
    C_Object cObject = {
            .bounds3 = bounds32cBounds3(object->bounds3()),
            .centroid = vec32float3(object->centroid()),
            .normal = vec32float3(object->normal()),
            .subType = object->type(),
            .material = material2cMaterial(object->material())
    };

    switch (cObject.subType) {
        case TRIANGLE:
            cObject.triangle = triangle2cTriangle(static_cast<Triangle*>(object));
            break;
        case SPHERE:
            cObject.sphere = sphere2cSphere(static_cast<Sphere*>(object));
            break;
    }

    return cObject;
}

#endif //CRENDERER_OBJECT2COBJECT_HPP
