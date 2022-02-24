//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_RAY_HPP
#define CRENDERER_RAY_HPP

#include "Vector.hpp"

class Ray {
public:
    const Vec3f origin;
    const Vec3f direction;
    const Vec3f recDirection;

    Ray(const Vec3f &origin, const Vec3f &direction)
    : origin(origin), direction(direction), recDirection(1 / direction.x(), 1 / direction.y(), 1 / direction.z()) {}
};

#endif //CRENDERER_RAY_HPP
