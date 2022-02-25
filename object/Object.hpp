//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_GEOMETRY_HPP
#define CRENDERER_GEOMETRY_HPP

#include "../Vector.hpp"
#include "../acceleration/Bounds3.hpp"
#include "../Ray.hpp"

extern class Intersection;

class Object {
public:
    virtual Intersection intersect(const Ray& ray) const = 0;

    Object() {}

    const Bounds3& bounds3() const { return bounds3_; };
    const Vec3f& centroid() const { return centroid_; }
    const Vec3f& normal() const { return normal_; }
    const Material* material() const { return material_; }

protected:
    Bounds3 bounds3_;
    Vec3f centroid_;
    Vec3f normal_;
    mutable const Material* material_;
};

#endif //CRENDERER_GEOMETRY_HPP
