//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_GEOMETRY_HPP
#define CRENDERER_GEOMETRY_HPP

#include "../Intersection.hpp"
#include "../core/Vector.hpp"
#include "../acceleration/Bounds3.hpp"

class Geometry {
public:
    virtual Intersection intersect() = 0;

    Geometry(const Vec3f& centroid, const Bounds3& bounds3)
        : centroid(centroid), bounds3(bounds3) {}

    Bounds3 bounds3;
    Vec3f centroid;
};

#endif //CRENDERER_GEOMETRY_HPP
