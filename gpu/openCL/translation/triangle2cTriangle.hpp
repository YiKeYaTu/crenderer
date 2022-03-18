//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_TRIANGLE2CTRIANGLE_HPP
#define CRENDERER_TRIANGLE2CTRIANGLE_HPP

#include "../include/object/C_Triangle.h"
#include "../../../object/Triangle.hpp"
#include "vec32float3.hpp"

C_Triangle triangle2cTriangle(Triangle* triangle) {
    C_Triangle cTriangle;

    cTriangle.v0 = vec32float3(triangle->v0_);
    cTriangle.v1 = vec32float3(triangle->v1_);
    cTriangle.v2 = vec32float3(triangle->v2_);

    cTriangle.e0 = vec32float3(triangle->e0_);
    cTriangle.e1 = vec32float3(triangle->e1_);
    cTriangle.e2 = vec32float3(triangle->e2_);

    cTriangle.n = vec32float3(triangle->normal_);

    return cTriangle;
}

#endif //CRENDERER_TRIANGLE2CTRIANGLE_HPP
