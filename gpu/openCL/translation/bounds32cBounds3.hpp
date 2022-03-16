//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_BOUNDS32CBOUNDS3_HPP
#define CRENDERER_BOUNDS32CBOUNDS3_HPP

#include "../../../acceleration/Bounds3.hpp"
#include "../include/C_Bounds3.h"
#include "vec32float3.hpp"

C_Bounds3 bounds32cBounds3(const Bounds3& bounds3) {
    C_Bounds3 cBounds3 = {
        .min = vec32float3(bounds3.min()),
        .max = vec32float3(bounds3.max())
    };
    return cBounds3;
}

#endif //CRENDERER_BOUNDS32CBOUNDS3_HPP
