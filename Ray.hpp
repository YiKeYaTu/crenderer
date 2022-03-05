//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_RAY_HPP
#define CRENDERER_RAY_HPP

#include "Vector.hpp"

class Ray {
public:
    static const double s;
    const Vec3f origin;
    const Vec3f direction;
    const Vec3f recDirection;

    Ray(const Vec3f &origin, const Vec3f &d)
    : origin(origin), direction(d.normalized() * s), recDirection(1.0f / direction.x(), 1.0f / direction.y(), 1.0f / direction.z()) {

    }
};

const double Ray::s = 100.0;

#endif //CRENDERER_RAY_HPP
